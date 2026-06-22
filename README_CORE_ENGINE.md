# 🧠 Core Engine - Neural Network Implementation

## Architecture Générale

Ce moteur neuronal implémente un **réseau de neurones feedforward multicouche** avec rétropropagation du gradient pour la classification d'échecs (Check/Checkmate/Nothing avec couleurs).

### Stack Technique
- **Langage** : C++17
- **Paradigme** : Programmation Orientée Objet
- **Algorithme** : Stochastic Gradient Descent (SGD) avec backpropagation
- **Activations** : ReLU (couches cachées) + Sigmoid (sortie)
- **Loss** : Mean Squared Error (MSE)

---

## 📦 Classes Principales

### `Network` (Network.cpp)

**Responsabilité** : Orchestration du réseau neuronal complet (topologie, entraînement, prédiction).

#### Attributs
```cpp
std::vector<Layer> layers;  // Couches du réseau
```

#### Méthodes Clés

##### 1. **Constructeur**
```cpp
Network(const std::vector<int> &topology)
```
- **Entrée** : `[64, 512, 256, 128, 5]` (architecture du réseau)
- **Action** :
  - Crée `layers.size() - 1` couches
  - Marque la dernière couche comme `is_output = true` (activation Sigmoid)
  - Toutes les autres : `is_output = false` (activation ReLU)

**Exemple** :
```cpp
Network net({64, 512, 256, 128, 5});
// Crée 4 couches :
// Layer 1: 64  → 512  (ReLU)
// Layer 2: 512 → 256  (ReLU)
// Layer 3: 256 → 128  (ReLU)
// Layer 4: 128 → 5    (Sigmoid)
```

##### 2. **Forward Pass (Prédiction)**
```cpp
std::vector<double> predict(const std::vector<double> &input)
```
- **Entrée** : Vecteur de 64 valeurs (position d'échecs)
- **Processus** :
  1. Propage l'input à travers chaque `Layer::forward()`
  2. Chaque couche applique : `output = activation(W·input + b)`
  3. La sortie d'une couche devient l'entrée de la suivante
- **Sortie** : Vecteur de 5 probabilités `[0,1]` (Sigmoid)

**Exemple** :
```cpp
std::vector<double> board = {...};  // 64 valeurs
auto output = net.predict(board);   // [0.01, 0.03, 0.92, 0.02, 0.04]
// Interprétation : 92% de probabilité de "Check White" (index 2)
```

##### 3. **Training Loop (Entraînement)**
```cpp
void train(const std::vector<TrainingData> &dataset, int epochs, double learning_rate)
```

**Algorithme complet** :

```
POUR chaque époque (1 à 150) :
    1. Mélanger les indices du dataset (évite l'ordre fixe)
    2. Initialiser totalError = 0
    
    POUR chaque exemple (449 300 positions) :
        
        === FORWARD PASS ===
        3. finalOutput = predict(data.input)
        
        === COMPUTE OUTPUT LAYER ERROR ===
        4. POUR chaque neurone de sortie k :
            err = target[k] - output[k]
            totalError += err²
            delta[k] = err × sigmoid'(output[k])
        
        === BACKPROPAGATION (couches cachées) ===
        5. POUR chaque couche (de l'avant-dernière à la première) :
            POUR chaque neurone j :
                error = Σ(delta_suivant[i] × weights_suivant[i][j])
                
                SI couche de sortie :
                    delta[j] = error × sigmoid'(output[j])
                SINON :
                    delta[j] = error × relu'(output[j])
        
        === WEIGHT UPDATE (SGD) ===
        6. POUR chaque couche :
            POUR chaque neurone j :
                bias[j] += learning_rate × delta[j]
                POUR chaque poids w[i] :
                    weight[j][i] += learning_rate × delta[j] × input[i]
    
    === MONITORING ===
    7. SI epoch % 10 == 0 :
        Afficher MSE = totalError / dataset.size()
    
    === LEARNING RATE DECAY ===
    8. SI epoch % 50 == 0 :
        learning_rate *= 0.9
```

**Paramètres critiques** :
- **Epochs** : 150 (nombre de passages complets sur le dataset)
- **Learning Rate** : **0.001** ⚠️ (ancien bug : 0.05 causait explosion des gradients)
- **Decay** : ×0.9 tous les 50 epochs (stabilisation progressive)

---

### `Layer` (Layer.cpp)

**Responsabilité** : Représente une seule couche de neurones avec poids, biais et activation.

#### Attributs
```cpp
int input_size;                          // Taille de l'entrée
int neuronCount;                         // Nombre de neurones
bool is_output;                          // true = Sigmoid, false = ReLU
std::vector<std::vector<double>> weights; // Matrice [neuronCount][input_size]
std::vector<double> biases;              // Vecteur [neuronCount]
std::vector<double> outputs;             // Cache des sorties
std::vector<double> deltas;              // Gradients pour backprop
```

#### Méthodes Clés

##### 1. **Constructeur avec Initialisation**
```cpp
Layer(int input_size, int neuronCount, bool is_output)
```

**Stratégie d'initialisation** (cruciale pour la convergence) :

- **Couches cachées (ReLU)** : **He Initialization**
  ```cpp
  limit = sqrt(2.0 / input_size)
  weights ~ Uniform(-limit, +limit)
  ```
  - Adapté à ReLU (neurones "morts" si trop petit)
  - Variance préservée à travers les couches

- **Couche de sortie (Sigmoid)** : **Xavier Initialization**
  ```cpp
  limit = sqrt(6.0 / (input_size + neuronCount))
  weights ~ Uniform(-limit, +limit)
  ```
  - Adapté à Sigmoid (saturation si trop grand)
  - Équilibre entre entrée et sortie

- **Biais** : Initialisés à `0.01` (légère activation initiale)

##### 2. **Forward Pass**
```cpp
std::vector<double> forward(const std::vector<double> &input)
```

**Algorithme** :
```
POUR chaque neurone i :
    1. sum = bias[i]
    2. POUR chaque entrée j :
        sum += weights[i][j] × input[j]
    
    3. SI is_output :
        output[i] = sigmoid(sum)  // 1 / (1 + e^(-sum))
       SINON :
        output[i] = relu(sum)     // max(0, sum)

RETOURNER outputs
```

**Exemple** :
```cpp
// Couche 64 → 512 (ReLU)
Layer layer1(64, 512, false);
auto out1 = layer1.forward(board);  // [512 valeurs] entre [0, +∞)

// Couche 128 → 5 (Sigmoid)
Layer output_layer(128, 5, true);
auto out5 = output_layer.forward(hidden); // [5 valeurs] entre [0, 1]
```

##### 3. **Fonctions d'Activation**

**ReLU (Rectified Linear Unit)** :
```cpp
double relu(double x) {
    return std::max(0.0, x);
}

double reluDerivative(double output) {
    return output > 0.0 ? 1.0 : 0.0;
}
```
- **Avantages** :
  - Pas de saturation pour valeurs positives
  - Gradient = 1 (pas de vanishing gradient)
  - Calcul ultra-rapide
- **Inconvénient** : "Dying ReLU" si LR trop élevé (neurones bloqués à 0)

**Sigmoid** :
```cpp
double activation(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

double activationDerivative(double output) {
    return output * (1.0 - output);
}
```
- **Avantages** :
  - Sortie dans [0,1] → interprétable comme probabilité
  - Dérivation simple avec output seulement
- **Inconvénient** : Vanishing gradient si utilisé partout (ancien problème du réseau)

---

## 🔄 Flux de Données Complet

### Exemple : Classification d'une position d'échecs

#### **INPUT** : Position FEN
```
"r1bk3r/p2pBpNp/n4n2/1p1NP2P/6P1/3P4/P1P1K3/q5b1"
```

#### **ÉTAPE 1 : Conversion FEN → Vecteur**
```cpp
// Traduction des pièces en valeurs
[
  -0.5,  0.0,  -0.35, -1.0,  0.0, ...,  // Rangée 8
   0.1,  0.0,   0.1,   0.0,  1.0, ...   // Rangée 1
] // 64 valeurs au total
```

#### **ÉTAPE 2 : Forward Pass**
```
Input [64]
    ↓ (Layer 1: 64×512 weights + ReLU)
Hidden 1 [512]
    ↓ (Layer 2: 512×256 weights + ReLU)
Hidden 2 [256]
    ↓ (Layer 3: 256×128 weights + ReLU)
Hidden 3 [128]
    ↓ (Layer 4: 128×5 weights + Sigmoid)
Output [5]
```

#### **ÉTAPE 3 : Output Interpretation**
```cpp
[0.01, 0.03, 0.92, 0.02, 0.04]
  ↓
argmax = 2 → "Check White"
```

### Training : Backpropagation Détaillée

#### **Exemple concret** :
```
Target : [0, 0, 1, 0, 0]  (Check White attendu)
Output : [0.01, 0.03, 0.92, 0.02, 0.04]

=== OUTPUT LAYER ERROR ===
err[0] = 0.00 - 0.01 = -0.01
err[1] = 0.00 - 0.03 = -0.03
err[2] = 1.00 - 0.92 = +0.08  ← Neurone à renforcer !
err[3] = 0.00 - 0.02 = -0.02
err[4] = 0.00 - 0.04 = -0.04

MSE = (-0.01² + -0.03² + 0.08² + -0.02² + -0.04²) / 5 = 0.0018

delta[2] = 0.08 × sigmoid'(0.92)
         = 0.08 × (0.92 × 0.08)
         = 0.00589

=== WEIGHT UPDATE (neurone 2 de la sortie) ===
SI input_prev[42] = 0.75 (activation du neurone 42 de la couche précédente)
ALORS :
    weight[2][42] += 0.001 × 0.00589 × 0.75
                   = +0.0000044

    bias[2] += 0.001 × 0.00589
             = +0.00000589
```

**Répété 449 300 fois par époque !**

---

## ⚠️ Problèmes Résolus

### 1. **Network Recreation Bug** (Ancien)
```cpp
// ❌ AVANT (catastrophique) :
for (epoch : epochs) {
    for (example : dataset) {
        Network net = convertir_en_moteur(reseau);  // 220k créations !
        net.train(...);
    }
}

// ✅ APRÈS (corrigé) :
Network net = convertir_en_moteur(reseau);  // 1 seule création
net.train(dataset, epochs, lr);
```

### 2. **Vanishing Gradient** (Sigmoid partout)
```cpp
// ❌ AVANT :
Layer layer(64, 512, true);  // Sigmoid → gradient × 0.25 à chaque couche
// Après 4 couches : gradient × 0.25⁴ = 0.0039 → apprentissage mort

// ✅ APRÈS :
Layer layer(64, 512, false);  // ReLU → gradient × 1.0
// Gradient préservé sur toute la profondeur
```

### 3. **Dying ReLU** (LR trop élevé)
```cpp
// ❌ AVANT :
learning_rate = 0.05;
// Epoch 40: MSE = 0.27 ✅
// Epoch 80: MSE = 0.77 💀 (neurones morts)

// ✅ APRÈS :
learning_rate = 0.001;
// Descente lente mais stable sans explosion
```

### 4. **Class Imbalance**
```cpp
// ❌ AVANT :
// Check: 60k, Checkmate: 100k, Nothing: 300k
// → Réseau apprend à prédire "Nothing" (statistiquement optimal)

// ✅ APRÈS :
// Check: 350k, Checkmate: 100k, Nothing: 100k (train_optimized.txt)
// → Représentation équilibrée
```

---

## 📊 Métriques de Performance

### Configuration Optimale (Actuelle)
```
Architecture : 64 → 512 → 256 → 128 → 5
Dataset      : 450k exemples (équilibré)
Epochs       : 150
Learning Rate: 0.001 (avec decay ×0.9/50 epochs)
Activations  : ReLU (hidden) + Sigmoid (output)
Optimizer    : SGD avec shuffle par époque
```

### Attentes de Convergence
```
Epoch   1 : MSE ~0.50 (réseau aléatoire)
Epoch  20 : MSE ~0.35 (début d'apprentissage)
Epoch  50 : MSE ~0.25 (bonnes performances)
Epoch 100 : MSE ~0.20 (convergence)
Epoch 150 : MSE ~0.18 (optimal)
```

**⚠️ Red Flags** :
- MSE remonte après epoch 40 → **LR trop élevé**
- MSE plateau à 0.65 → **Vanishing gradient** (Sigmoid partout)
- MSE stable à 0.77 → **Dying ReLU** (gradients explosés)

---

## 🔧 Optimisations Appliquées

### Compilation
```makefile
CXXFLAGS = -O3 -march=native -ffast-math -funroll-loops
```
- **-march=native** : Utilise les instructions SIMD du CPU (AVX2/AVX-512)
- **-ffast-math** : Optimisations mathématiques agressives
- **-funroll-loops** : Déplie les boucles (moins de sauts CPU)
- **Gain** : ~1.5-2x vitesse

### Data Shuffling
```cpp
std::shuffle(indices.begin(), indices.end(), g);
```
- Évite l'ordre fixe Check → Checkmate → Nothing
- Force le réseau à généraliser au lieu de mémoriser la séquence

### Learning Rate Decay
```cpp
if ((epoch + 1) % 50 == 0)
    learning_rate *= 0.9;
```
- Début : Exploration (LR = 0.001)
- Milieu : Raffinement (LR = 0.0009)
- Fin : Stabilisation (LR = 0.00073)

---

## 📝 API Usage

### Entraînement
```cpp
// 1. Créer le réseau
Network net({64, 512, 256, 128, 5});

// 2. Préparer les données
std::vector<TrainingData> dataset;
dataset.push_back({
    .input = {0.5, -0.3, ...},  // 64 valeurs
    .target = {0, 0, 1, 0, 0}   // One-hot encoding
});

// 3. Entraîner
net.train(dataset, 150, 0.001);

// 4. Sauvegarder
net.save("my_network.nn");
```

### Prédiction
```cpp
// 1. Charger le réseau
Network net;
net.load("my_network.nn");

// 2. Prédire
std::vector<double> board = {...};  // 64 valeurs
auto output = net.predict(board);   // [0.01, 0.03, 0.92, 0.02, 0.04]

// 3. Interpréter
int predicted_class = std::distance(
    output.begin(),
    std::max_element(output.begin(), output.end())
);
// predicted_class = 2 → "Check White"
```

---

## 🎯 Conclusion

Ce moteur neuronal implémente un **réseau feedforward classique optimisé pour la classification multi-classes**. Les choix techniques (ReLU + Sigmoid, He/Xavier init, LR decay, shuffle) suivent les **best practices modernes** du Deep Learning tout en restant suffisamment simple pour être compréhensible et debuggable.

**Points forts** :
- ✅ Architecture modulaire (Network ↔ Layer)
- ✅ Implémentation correcte de la backpropagation
- ✅ Gestion hybride des activations (performance + stabilité)
- ✅ Prévention des gradients explosifs/disparaissants

**Pour aller plus loin** :
- [ ] Implémenter Adam optimizer (momentum + adaptive LR)
- [ ] Ajouter Batch Normalization entre les couches
- [ ] Dropout pour régularisation
- [ ] Early stopping basé sur validation set
