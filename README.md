# Neural Network - Chess Position Classifier

## Description

Moteur de réseau de neurones feedforward développé en **C++17** pour classifier des positions d'échecs selon trois catégories (avec couleurs) :
- **Check White / Black** (Échec)
- **Checkmate White / Black** (Mat)
- **Nothing** (Position normale)

Le réseau utilise une architecture multicouche avec **rétropropagation du gradient** et **activations hybrides** (ReLU + Sigmoid) pour atteindre des performances supérieures à 90% de précision.

---

## Fonctionnalités

- Architecture personnalisable (topologie définie par fichier de configuration)
- Entraînement par **Stochastic Gradient Descent** avec Learning Rate Decay
- Activations optimisées : **ReLU** (couches cachées) + **Sigmoid** (sortie)
- Support du format **FEN** (Forsyth-Edwards Notation) pour les positions d'échecs
- Génération automatique de datasets équilibrés
- Sauvegarde/chargement de réseaux entraînés
- Métriques détaillées par classe (précision, recall)

---

## Architecture

```
INPUT (64 valeurs)
    ↓
Layer 1: 64 → 512 (ReLU)
    ↓
Layer 2: 512 → 256 (ReLU)
    ↓
Layer 3: 256 → 128 (ReLU)
    ↓
Layer 4: 128 → 5 (Sigmoid)
    ↓
OUTPUT [Check W, Check B, Checkmate W, Checkmate B, Nothing]
```

---

## ⚡ Quick Start

### Compilation
```bash
make
```

### Entraînement
```bash
./train.sh
```

### Prédiction
```bash
./my_torch_analyzer --predict my_torch_network.nn test.txt
```

---

## Documentation Complète

Pour une documentation détaillée (architecture, algorithmes, optimisations, troubleshooting) :

**[Accéder à la documentation en ligne](https://area-12.gitbook.io/neural-net/)**

---

## Technologies

- **Langage** : C++17
- **Algorithme** : Backpropagation + SGD
- **Optimisations** : `-O3 -march=native -ffast-math -funroll-loops`
- **Dataset** : 450k positions d'échecs (équilibré)

---

## Performances

- **Précision globale** : ~75%
- **Vitesse d'entraînement** : ~1h pour 150 époques (450k exemples)
- **Taille du modèle** : ~2.5 MB (réseau entraîné)

---

## Auteurs
FLORIAN AHOUANGBE
JONAS CHAOU
JOHANN-ANDRE 

Projet développé dans le cadre du module **Neural Network** @ Epitech

---

## Licence

Ce projet est un projet éducatif.
