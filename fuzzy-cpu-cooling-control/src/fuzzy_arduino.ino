#include "OneWire.h"
#include "DallasTemperature.h"

#define DS18B20_PIN 3
#define FAN_PWM_PIN 10
#define MIC_PIN A0
#define BUTTON_PIN 4

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

bool targetMode = false;
bool lastButtonState = HIGH;

float targetTemp = 36.0;

inline float trapmf(float x, float a, float b, float c, float d) {
  if (x < a || x > d) return 0.0f;

  if (x >= b && x <= c) return 1.0f;

  if (x >= a && x < b) {
    if (b == a) return 1.0f;
    return (x - a) / (b - a);
  }

  if (x > c && x <= d) {
    if (d == c) return 1.0f;
    return (d - x) / (d - c);
  }

  return 0.0f;
}

inline float trimf(float x, float a, float b, float c) {
  if (x <= a || x >= c) return 0.0f;
  if (x <= b) return (x - a) / (b - a);
  return (c - x) / (c - b);
}

inline float cpu_niska(float x)     { return trapmf(x, 25, 25, 32.5, 35.5); }
inline float cpu_normalna(float x)  { return trimf(x, 33.5, 36.5, 39.5); }
inline float cpu_wysoka(float x)    { return trimf(x, 37.5, 40.5, 44.5); }
inline float cpu_krytyczna(float x) { return trapmf(x, 43.5, 47.5, 70, 70); }

inline float amb_chlodno(float x)   { return trapmf(x, 16, 16, 20, 22); }
inline float amb_normalnie(float x) { return trapmf(x, 21, 22.5, 25.5, 26.5); }
inline float amb_cieplo(float x)    { return trapmf(x, 25.5, 26.5, 40, 40); }

inline float noi_cicho(float x)     { return trapmf(x, 0, 0, 35, 50); }
inline float noi_srednio(float x)   { return trimf(x, 45, 60, 75); }
inline float noi_glosno(float x)    { return trapmf(x, 70, 82, 100, 100); }

float pwmMF(uint8_t idx, float x) {
  if (idx == 0) return trapmf(x, 0,  0,  8,  27);   // off
  if (idx == 1) return trimf (x, 22, 42, 58);        // low
  if (idx == 2) return trimf (x, 50, 68, 84);        // medium
  if (idx == 3) return trimf (x, 72, 88, 98);        // high
  if (idx == 4) return trapmf(x, 90, 97, 100, 100);  // max
  return 0.0f;
}

static const uint8_t rule_table[4][3][3] = {
  // CPU: niska
  {
    {0, 0, 0},  // chlodno
    {0, 0, 0},  // normalnie
    {1, 1, 0},  // cieplo
  },
  // CPU: normalna
  {
    {1, 1, 0},  // chlodno
    {2, 1, 1},  // normalnie
    {2, 2, 1},  // cieplo
  },
  // CPU: wysoka 
  {
    {3, 2, 2},  // chlodno
    {3, 3, 2},  // normalnie 
    {4, 3, 3},  // cieplo
  },
  // CPU: krytyczna
  {
    {4, 4, 4},  // chlodno
    {4, 4, 4},  // normalnie
    {4, 4, 4},  // cieplo
  },
};

float fuzzyInfer(float t_cpu, float t_amb, float t_noi) {
  float mu_cpu[4] = {
    cpu_niska(t_cpu),
    cpu_normalna(t_cpu),
    cpu_wysoka(t_cpu),
    cpu_krytyczna(t_cpu)
  };

  float mu_amb[3] = {
    amb_chlodno(t_amb),
    amb_normalnie(t_amb),
    amb_cieplo(t_amb)
  };

  float mu_noi[3] = {
    noi_cicho(t_noi),
    noi_srednio(t_noi),
    noi_glosno(t_noi)
  };

  float num = 0.0f;
  float den = 0.0f;
  // defuzyfikacja centroid, dyskretna
  for (int u = 0; u <= 100; u++) {
    float aggregated = 0.0f;

    for (uint8_t ci = 0; ci < 4; ci++) {
      for (uint8_t ai = 0; ai < 3; ai++) {
        for (uint8_t ni = 0; ni < 3; ni++) {
          float firing = min(mu_cpu[ci], min(mu_amb[ai], mu_noi[ni]));

          if (firing < 0.001f) continue;

          uint8_t pwm_idx = rule_table[ci][ai][ni];
          float clipped = min(firing, pwmMF(pwm_idx, (float)u));

          if (clipped > aggregated) {
            aggregated = clipped;
          }
        }
      }
    }

    num += (float)u * aggregated;
    den += aggregated;
  }

  if (den < 0.001f) return 0.0f;

  return num / den;
}

int readAmplitude() {
  int minVal = 1023;
  int maxVal = 0;

  for (int i = 0; i < 300; i++) {
    int value = analogRead(MIC_PIN);

    if (value < minVal) minVal = value;
    if (value > maxVal) maxVal = value;

    delayMicroseconds(200);
  }

  return maxVal - minVal;
}

int readNoise() {
  int amplitude = readAmplitude();
  int noise = map(amplitude, 80, 500, 0, 100);
  return constrain(noise, 0, 100);
}

class EMA {
  float val;
  float alpha;

public:
  EMA(float a, float init = 0.0f) {
    alpha = a;
    val = init;
  }

  float update(float x) {
    val = alpha * x + (1.0f - alpha) * val;
    return val;
  }
};

EMA pwmFilter(0.25f, 0.0f);
EMA noiseFilter(0.6f, 30.0f);

void setupFanPWM() {
  pinMode(FAN_PWM_PIN, OUTPUT);

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  TCCR1A = (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);

  ICR1 = 639;
  OCR1B = 0;
}

void setFanPWM(float pwm_pct) {
  pwm_pct = constrain(pwm_pct, 0, 100);
  OCR1B = (uint16_t)(ICR1 * pwm_pct / 100.0f);
}

float targetController(float cpuTemp) {
  float error = cpuTemp - targetTemp;
  if (error <= -0.5f) return 0.0f;
  float pwm = 25.0f + error * 25.0f;

  return constrain(pwm, 0.0f, 100.0f);
}

void setup() {
  Serial.begin(115200);
  sensors.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setupFanPWM();

  Serial.println("T_CPU,T_CASE,T_AMBIENT,noise,pwm,mode");
}

void loop() {
  bool buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && buttonState == LOW) {
    targetMode = !targetMode;
    delay(250);
  }

  lastButtonState = buttonState;

  sensors.requestTemperatures();

  float T_CPU = sensors.getTempCByIndex(0);
  float T_CASE = sensors.getTempCByIndex(1);
  float T_AMBIENT = sensors.getTempCByIndex(2);

  if (T_CPU < -100 || T_CASE < -100 || T_AMBIENT < -100) {
    setFanPWM(100);
    Serial.println("ERROR_SENSOR,ERROR_SENSOR,ERROR_SENSOR,0,100,ERROR");
    delay(1000);
    return;
  }

  int rawNoise = readNoise();
  float filteredNoise = noiseFilter.update(rawNoise);

  float pwm;

  if (targetMode) {
    pwm = targetController(T_CPU);
  } else {
    pwm = fuzzyInfer(T_CPU, T_AMBIENT, filteredNoise);
  }

  if (pwm < 30.0f){
    pwm = 0.0f;
  }

  float filteredPwm = pwmFilter.update(pwm);

  setFanPWM(filteredPwm);
  
  Serial.print(T_CPU);
  Serial.print(",");
  Serial.print(T_CASE);
  Serial.print(",");
  Serial.print(T_AMBIENT);
  Serial.print(",");
  Serial.print(filteredNoise);
  Serial.print(",");
  Serial.print(filteredPwm);
  Serial.print(",");

  if (targetMode) {
    Serial.println("TARGET_36");
  } else {
    Serial.println("FUZZY");
  }

  delay(1000);
}