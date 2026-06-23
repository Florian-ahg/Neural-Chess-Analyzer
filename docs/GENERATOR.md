# 🧠 Module de Traduction des Données – Projet *my_torch*

**Auteur : Johann-André MAFOIRKAN**  
**Rôle : Personne B – Data Engineering / Traduction des données**

---

## 📌 Objectif du module

Ce module a pour rôle de **transformer des données textuelles issues d’un dataset d’échecs** en **données numériques exploitables par un réseau de neurones**.

Il constitue une **étape indispensable** du pipeline de machine learning :  
> *Un réseau de neurones ne peut pas apprendre directement à partir de texte, uniquement à partir de valeurs numériques.*

Ce travail correspond au rôle **Personne B (Data / Traduction)** dans l’architecture globale du projet *my_torch*.

---

## 🧩 Vue d’ensemble (accessible à tous)

Une ligne du dataset ressemble à ceci :

```
8/2B2k2/6b1/6PK/r7/8/8/8 w - - 1 71 Check Black
```

Cette ligne contient :
- une **position d’échecs** (notation FEN),
- un **résultat attendu** (Check, Checkmate ou Nothing).

Le module transforme cette ligne en :
- un **vecteur de 64 nombres** représentant l’échiquier,
- un **vecteur de 3 nombres** représentant la classe attendue.

Ces deux vecteurs sont ensuite utilisés par le réseau de neurones pour apprendre.

---

## 🔁 Pipeline de transformation (logique complète)

Le traitement d’une ligne suit systématiquement les étapes suivantes :

1. **Lecture de la ligne brute**
2. **Séparation de la FEN et du label**
3. **Traduction de la FEN en vecteur de 64 valeurs**
4. **Traduction du label en vecteur de 3 valeurs**
5. **Transmission au module d’entraînement (Personne C)**

Ce module ne fait **aucun apprentissage** : il prépare les données.

---

## ♟️ 1. Parsing : séparation FEN / Label

### Fonction
```cpp
void parseLine(const std::string &line, std::string &fen, std::string &label);
```

### Description

- La **FEN complète** est reconstruite à partir des 6 champs standards.
- Le **label** correspond à tout ce qui suit la FEN.

### Exemple

Entrée :
```
8/8/R2k4/4r1p1/8/5K2/5P2/8 b - - 7 59 Check White
```

Sortie :
```
fen   = "8/8/R2k4/4r1p1/8/5K2/5P2/8 b - - 7 59"
label = "Check White"
```

Cette étape garantit que la FEN reste valide et exploitable.

---

## 🔢 2. Normalisation des pièces d’échecs

### Fonction
```cpp
double pieceToValue(char c);
```

### Objectif

Convertir chaque pièce en une **valeur numérique normalisée**, comprise entre `-1` et `1`, afin d’assurer la stabilité de l’apprentissage.

### Table de conversion

| Pièce | Valeur |
|------|--------|
| P | +0.1 |
| N | +0.2 |
| B | +0.3 |
| R | +0.4 |
| Q | +0.5 |
| K | +0.6 |
| p | -0.1 |
| n | -0.2 |
| b | -0.3 |
| r | -0.4 |
| q | -0.5 |
| k | -0.6 |
| case vide | 0.0 |

### Pourquoi normaliser ?

- Éviter l’explosion des gradients
- Améliorer la convergence
- Garantir une échelle cohérente des données

---

## 🧮 3. Traduction FEN → vecteur de 64 valeurs

### Fonction
```cpp
std::vector<double> fenToVector(const std::string &fen);
```

### Principe

- Un échiquier contient **64 cases**.
- Chaque case est représentée par une valeur numérique.
- Le vecteur final contient **exactement 64 doubles**.

### Règles de traduction

- `/` → ignoré (changement de rangée)
- `1` à `8` → N cases vides (`0.0`)
- Lettre de pièce → appel à `pieceToValue(c)`

### Exemple

FEN (plateau uniquement) :
```
6b1
```

Vecteur obtenu :
```
0, 0, 0, 0, 0, 0, -0.3, 0
```

Des sécurités garantissent toujours un vecteur final de taille 64.

---

## 🏷️ 4. Traduction du label → vecteur de sortie

### Fonction
```cpp
std::vector<double> labelToVector(const std::string &label);
```

### Encodage one-hot

| Label | Vecteur |
|------|--------|
| Checkmate | [1, 0, 0] |
| Check | [0, 1, 0] |
| Nothing | [0, 0, 1] |

Seul le **premier mot** du label est pris en compte  
(ex. `Check Black` → `Check`).

---

## 📦 Résultat final produit

Pour chaque ligne du dataset :

- `inputVector` → 64 valeurs (entrée du réseau)
- `targetVector` → 3 valeurs (sortie attendue)

Ces vecteurs sont directement utilisables pour :
```cpp
net.train(inputVector, targetVector);
```

---

## 🧪 Validation et tests

- Vérification systématique de la taille des vecteurs (64 / 3)
- Tests sur plusieurs FEN réelles
- Validation rangée par rangée pour éviter tout décalage

---

## 🚫 Hors périmètre du module

Ce module ne gère volontairement pas :

- l’entraînement du réseau
- la rétropropagation
- la prédiction
- la sauvegarde du modèle

Ces responsabilités sont gérées par d’autres modules, conformément au principe de séparation des responsabilités.

---

## 🧠 Conclusion

Ce module garantit que les données envoyées au réseau de neurones sont :
- propres,
- normalisées,
- cohérentes,
- exploitables.

> **Sans cette étape de traduction, l’apprentissage du réseau serait impossible ou instable.**

---

**Auteur : Johann-André MAFOIRKAN**  
**Projet : my_torch – Module de traduction des données**
