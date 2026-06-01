# Parcel Locker TSP Optimization

## 🇵🇱 Opis projektu

Projekt dotyczy optymalizacji tras kurierskich pomiędzy automatami paczkowymi z wykorzystaniem rzeczywistych danych przestrzennych z OpenStreetMap. Problem został zamodelowany jako asymetryczny problem komiwojażera (ATSP), w którym koszt przejazdu między punktami wynika z rzeczywistej topologii miejskiej sieci drogowej.

W projekcie porównano dwa podejścia optymalizacyjne: algorytm genetyczny oraz algorytm mrówkowy. Celem było znalezienie możliwie najkrótszej i spójnej trasy odwiedzającej wybrane automaty paczkowe oraz wracającej do punktu startowego.

## 🇺🇸 Project Description

This project focuses on courier route optimization between parcel lockers using real-world spatial data from OpenStreetMap. The task was modeled as an asymmetric Travelling Salesman Problem (ATSP), where travel costs are based on the actual urban road network topology.

The project compares two optimization approaches: Genetic Algorithm and Ant Colony Optimization. The goal was to find a short and logistically consistent route visiting selected parcel lockers and returning to the starting point.

---

## Features

- OpenStreetMap-based parcel locker extraction
- Road network processing with OSMnx
- Mapping parcel lockers to driveable graph nodes
- Shortest-path cost matrix calculation
- Genetic Algorithm implementation
- Ant Colony Optimization implementation
- Route visualization with maps and animations

---

## Tech Stack

- Python
- OSMnx
- GeoPandas
- NetworkX
- NumPy
- pandas
- Matplotlib
- Folium

---

## Project Structure

```text
parcel-locker-tsp-optimization/
├── figures/
│   ├── aco_Kraków.gif
│   ├── best_ACO_map_Kraków_InPost.html
│   ├── best_GA_map_Kraków_InPost.html
│   └── ga_Kraków.gif
├── notebooks/
│   └── Kowalski_algorytmy_genetyczne.ipynb
├── reports/
│   └── Kowalski_Algorytmy_Ewolucyjne.pdf
└── README.md

---

## Environment Setup

The project uses a Conda environment. To recreate it, install Anaconda or Miniconda and run:

```bash
git clone git clone https://github.com/kajetankow/computational-intelligence-in-data-analysis.git
cd computational-intelligence-in-data-analysis/parcel-locker-tsp-optimization
conda env create -f environment.yml
conda activate AlGenIO26
jupyter notebook
```

The notebook is located in:

```text
notebooks/Kowalski_algorytmy_genetyczne.ipynb
```

To update the environment file after installing new packages, run:

```bash
conda env export --from-history > environment.yml
```