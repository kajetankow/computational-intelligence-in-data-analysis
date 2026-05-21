# Fuzzy CPU Cooling Control

## 🇵🇱 Opis projektu

Projekt dotyczy zastosowania logiki rozmytej do sterowania układem chłodzenia procesora. Celem było zaprojektowanie regulatora, który dynamicznie dobiera wysterowanie wentylatora PWM na podstawie temperatury procesora, temperatury otoczenia oraz poziomu hałasu.

Model rozmyty został przygotowany i przetestowany w Pythonie, a następnie zaimplementowany sprzętowo na Arduino. Układ został porównany z chłodzeniem fabrycznym oraz prostym sterowaniem do zadanej temperatury.

## 🇺🇸 Project Description

This project applies fuzzy logic to CPU cooling control. The goal was to design a controller that dynamically adjusts PWM fan speed based on CPU temperature, ambient temperature, and noise level.

The fuzzy model was developed and tested in Python, then implemented on Arduino hardware. The system was compared against stock cooling and a simple target-temperature controller.

---

## Features

- Mamdani fuzzy inference model
- CPU, ambient temperature, and noise-based control
- PWM fan control
- Arduino-based physical implementation
- Serial data logging
- Comparison with stock cooling and target-temperature control
- Temperature and PWM analysis in Jupyter Notebook

---

## Hardware Requirements

The Jupyter Notebook can be used for simulation, analysis, and visualization without physical hardware.

To operate the real cooling control system, the following hardware setup is required:

- Arduino Nano
- DS18B20 temperature sensors (3x)
- MAX9814 microphone module
- PWM-compatible fan
- 12 V fan power supply
- 4.7 kΩ pull-up resistor for the 1-Wire temperature sensor bus
- USB connection between Arduino and the computer
- Proper wiring according to the diagram in `schemas/schemat.png`

The physical wiring scheme is available in:

```text
schemas/schemat.png

---

## Source Files

The `src/` directory contains the source code used for the physical implementation and data collection:

- `fuzzy_arduino.ino` – Arduino sketch uploaded to the Arduino Nano. It reads sensor values, applies the fuzzy control logic, and generates the PWM signal for the fan.
- `read_serial_csv.py` – Python script for reading measurement data from the Arduino serial port and saving it to a CSV file. The script was adjusted during testing depending on the current experiment configuration; the included version corresponds to the final setup.
