
# 🧠 Module Réseau Neuronal & Sérialisation – Projet *my_torch*
**Auteur : Jonas CHAOU**  
**Rôle : Personne A – Architecte du réseau neuronal & gestion de la sérialisation**

---

# 📌 OBJECTIF GÉNÉRAL

Cette partie constitue **le cœur mathématique du projet my_torch**.  
Elle implémente toute la logique interne d’un **réseau neuronal multi‑couches (MLP)**, incluant :

- représentation interne des couches et neurones  
- initialisation pseudo‑aléatoire contrôlée des poids  
- propagation avant (feedforward)  
- rétropropagation du gradient (backpropagation classique)  
- mise à jour des poids par descente de gradient  
- sérialisation et sauvegarde du modèle (`save() / load()`)  
- encodage des sorties one-hot  

Sans cette partie, **aucun apprentissage ni prédiction n’est possible**.

---

# 🧩 ARCHITECTURE GLOBALE DU MODULE

Le module développé par Jonas est structuré en 4 fichiers principaux :

| Fichier | Rôle |
|--------|------|
| `reseau.cpp` | Architecture interne du réseau, forward + backprop |
| `serialisation_reseau.cpp` | Sauvegarde/chargement du modèle `.nn` |
| `encodage_sorties.cpp` | Encodage des labels (Checkmate / Check / Nothing) |
| `parseur_fen.cpp` | Parseur FEN simplifié pour tests internes |

---

# 🧠 1. ARCHITECTURE MATHEMATIQUE DU RÉSEAU  
## 📌 Représentation d’un neurone

Un neurone est modélisé par :

```cpp
struct Neuron {
    std::vector<double> weights; // vers la couche suivante
    double bias;
    double outputValue;
    double gradient;
};
```

### ✔️ Poids (`weights`)
- Initialisés dans un intervalle contrôlé, typiquement :  
  `[-1/sqrt(n), +1/sqrt(n)]`  
  pour éviter saturation et explosion des gradients.

### ✔️ Activation (sigmoïde)
Utilisée pour *toutes les couches* sauf suggestion d’amélioration :

```
sigmoid(x) = 1 / (1 + exp(-x))
sigmoid'(x) = sigmoid(x) * (1 - sigmoid(x))
```

⚠️ Le choix de la sigmoïde peut provoquer du **vanishing gradient**, mais il reste cohérent pour un MLP simple.

---

## 🧱 Structure d’une couche

```cpp
struct Layer {
    std::vector<Neuron> neurons;
};
```

Un réseau est donc un :

```cpp
std::vector<Layer> layers;
```

**La topologie est dynamique**, ex :  
`[64, 32, 16, 3]`

---

# ⚙️ 2. PROPAGATION AVANT (FEEDFORWARD)

Pour chaque neurone de la couche `L` :

```
sum = Σ( weight_i * output_i_prev ) + bias
output = sigmoid(sum)
```

Implémentation typique :

```cpp
for (Neuron &n : layer) {
    double total = 0.0;
    for (int i = 0; i < prevLayer.neurons.size(); i++)
        total += prevLayer.neurons[i].outputValue * n.weights[i];

    total += n.bias;
    n.outputValue = sigmoid(total);
}
```

✔️ Ce mécanisme permet de transformer un vecteur d’entrée en une prédiction.

---

# 🔁 3. RÉTROPROPAGATION (BACKPROP)

Trois étapes majeures :

---

## 🔹 Étape 1 : Calcul de l’erreur de sortie

Pour la couche finale :

```
error = targetValue - outputValue
gradient = error * sigmoid'(outputValue)
```

---

## 🔹 Étape 2 : Backprop dans les couches cachées

```
gradient_hidden = Σ( poids * gradient_couche_suivante ) * sigmoid'(output)
```

---

## 🔹 Étape 3 : Mise à jour des poids

```
poids += learningRate * gradient * input
bias  += learningRate * gradient
```

Implémentation type :

```cpp
for (Neuron &n : layer) {
    for (int w = 0; w < n.weights.size(); w++)
        n.weights[w] += lr * n.gradient * prevLayer.neurons[w].outputValue;

    n.bias += lr * n.gradient;
}
```

---

# 🧮 4. FONCTIONS DE COÛT ET STABILITÉ NUMÉRIQUE

Le coût utilisé est le **Mean Squared Error (MSE)** :

```
MSE = Σ (output_i – target_i)^2
```

Avantages :
- stable  
- facile à dériver  
- adapté à une classification simple  

Limite :  
- moins performant que **cross‑entropy** pour des sorties classificatoires → amélioration future.

---

# 🧪 5. TESTS INTERNES (VALIDATION)

Jonas a validé son implémentation avec :

✔️ **Apprentissage du XOR** → preuve que backprop fonctionne  
✔️ Vérification de la stabilité des gradients  
✔️ Comparaison avant/après sauvegarde du modèle `.nn`  
✔️ Tests d’intégrité :

- même poids après `save()` puis `load()`
- même prédiction après rechargement
- cohérence structurelle entre topologie et stockage disque

---

# 💾 6. SÉRIALISATION DU RÉSEAU (`serialisation_reseau.cpp`)

## ✔️ Format du fichier `.nn`

Inclut :

- topologie (ex : `64 32 3`)
- nombre de couches
- nombre de neurones par couche
- poids de chaque neurone
- bias associé

### Exemple (simplifié)
```
topology 64 32 3
layer 0
 neuron 0 bias 0.12 w 0.41 0.55 ...
 neuron 1 bias -0.04 w -0.09 0.22 ...
layer 1
 ...
```

## ✔️ Pourquoi une sérialisation manuelle ?

- format lisible par un humain  
- indépendance totale de frameworks externes  
- facilite le débogage  
- permet au module C de recharger le modèle rapidement  

---

# 🧠 7. ENCODAGE DES SORTIES (fichier `encodage_sorties.cpp`)

Jonas encode les labels du dataset en vecteurs one-hot :

| Label | Encodage |
|--------|----------|
| Checkmate | [1, 0, 0] |
| Check | [0, 1, 0] |
| Nothing | [0, 0, 1] |

✔️ Compatible avec la couche de sortie sigmoïde  
✔️ Requis pour un coût MSE  

---

# 🧩 8. CONCLUSION

Le travail de **Jonas CHAOU** représente la **fondation mathématique du projet my_torch**.

Il fournit :

- un réseau neuronal complet et configurable  
- un moteur d’apprentissage fonctionnel  
- un système stable de sauvegarde / rechargement  
- un encodage des sorties standardisé  

> **Sans ce module, l’IA ne peut ni apprendre, ni prédire, ni exister.  
> C’est la pierre angulaire du projet.**

---

# 👤 Auteur
**Jonas CHAOU – Architecte réseau neuronal du projet my_torch**
