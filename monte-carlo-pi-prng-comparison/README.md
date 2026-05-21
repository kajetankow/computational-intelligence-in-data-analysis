# Monte Carlo Pi Estimation and PRNG Comparison

## 🇵🇱 Opis projektu

Projekt dotyczy estymacji wartości liczby π metodą Monte Carlo oraz porównania wpływu różnych generatorów liczb pseudolosowych na dokładność, stabilność i czas obliczeń.

Symulacje przeprowadzono dla wielu liczebności prób oraz 10 niezależnych serii, co pozwoliło ocenić zbieżność estymatora, rozrzut wyników i zachowanie błędu bezwzględnego. Porównano generatory: NumPy, liniowy, afiniczny, wielomianowy oraz inwersyjny.

## 🇺🇸 Project Description

This project focuses on estimating the value of π using the Monte Carlo method and comparing the impact of different pseudorandom number generators on accuracy, stability, and execution time.

Simulations were performed for multiple sample sizes and 10 independent runs, allowing analysis of estimator convergence, result variability, and absolute error behavior. The compared generators include NumPy, linear, affine, polynomial, and inverse generators.

---

## Features

- Monte Carlo estimation of π
- Comparison of multiple pseudorandom number generators
- Convergence analysis for increasing sample sizes
- Absolute error analysis
- Variance and boxplot-based stability comparison
- Execution time comparison

---

## Tech Stack

- Python
- NumPy
- pandas
- Matplotlib

---

## Project Structure

```text
monte-carlo-pi-prng-comparison/
├── README.md
├── notebooks/
│   └── monte_carlo_pi_prng_comparison.ipynb
└── reports/
    └── report.pdf
