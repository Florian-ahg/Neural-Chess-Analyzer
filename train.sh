#!/bin/bash

# Couleurs pour l'affichage
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
NETWORK_CONFIG="chess_medium.conf"
NETWORK_FILE="my_torch_network.nn"
DATASET="train_full.txt"
EPOCHS=50
LEARNING_RATE=0.1

# Étape 1 : Compilation du projet
echo -e "${YELLOW}[1/6] Compilation du projet...${NC}"
make clean > /dev/null 2>&1
if make 2>&1 | grep -q "error:"; then
    echo -e "${RED}✗ Erreur de compilation${NC}"
    make
    exit 1
else
    echo -e "${GREEN}✓ Compilation réussie${NC}"
    echo "  - my_torch_analyzer : Programme d'entraînement et prédiction"
    echo "  - my_torch_generator : Générateur de réseaux"
fi
echo ""

# Étape 2 : Vérification du dataset
echo -e "${YELLOW}[2/6] Vérification du dataset...${NC}"
if [ ! -f "$DATASET" ]; then
    echo -e "${BLUE}  Dataset train_full.txt non trouvé, création...${NC}"
    cat dataset/check/*.txt dataset/checkmate/*.txt dataset/nothing/*.txt > train_full.txt
fi

TOTAL_EXAMPLES=$(wc -l < "$DATASET")
CHECK_COUNT=$(grep -c "Check" "$DATASET" || echo 0)
CHECKMATE_COUNT=$(grep -c "Checkmate" "$DATASET" || echo 0)
NOTHING_COUNT=$(grep -c "Nothing" "$DATASET" || echo 0)

echo -e "${GREEN}✓ Dataset chargé : $TOTAL_EXAMPLES exemples${NC}"
echo "  - Check     : $CHECK_COUNT exemples"
echo "  - Checkmate : $CHECKMATE_COUNT exemples"
echo "  - Nothing   : $NOTHING_COUNT exemples"
echo ""

# Étape 3 : Génération du réseau vierge
echo -e "${YELLOW}[3/6] Génération du réseau de neurones vierge...${NC}"
./my_torch_generator "$NETWORK_CONFIG" 1 > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Réseau initialisé : chess_medium.nn${NC}"
    TOPOLOGY=$(head -3 "$NETWORK_CONFIG" | tr '\n' ' ')
    echo "  Architecture : $TOPOLOGY"
    echo "  Activation : ReLU (couches cachées) + Sigmoid (sortie)"
else
    echo -e "${RED}✗ Erreur lors de la génération du réseau${NC}"
    exit 1
fi
echo ""

# Étape 4 : Entraînement
echo -e "${YELLOW}[4/6] Entraînement du réseau (${EPOCHS} époques, LR=${LEARNING_RATE})...${NC}"
echo -e "${BLUE}Cela peut prendre plusieurs minutes...${NC}"
echo ""

START_TIME=$(date +%s)
./my_torch_analyzer --train --save "$NETWORK_FILE" chess_medium.nn "$DATASET" 2>&1 | tee train_output.log

if [ ${PIPESTATUS[0]} -eq 0 ] && [ -f "$NETWORK_FILE" ]; then
    END_TIME=$(date +%s)
    DURATION=$((END_TIME - START_TIME))
    echo ""
    echo -e "${GREEN}✓ Entraînement terminé en ${DURATION}s${NC}"
    echo "  Réseau sauvegardé : $NETWORK_FILE"
    
    # Afficher les dernières époques
    echo ""
    echo "Progression de l'erreur :"
    grep "Epoch" train_output.log | tail -5
else
    echo -e "${RED}✗ Erreur lors de l'entraînement${NC}"
    exit 1
fi
echo ""

# Étape 5 : Création du dataset de test balancé
echo -e "${YELLOW}[5/6] Préparation du dataset de test...${NC}"
head -1000 dataset/check/10_pieces.txt > test_validation.txt
head -1000 dataset/checkmate/10_pieces.txt >> test_validation.txt
head -1000 dataset/nothing/10_pieces.txt >> test_validation.txt
echo -e "${GREEN}✓ Dataset de test créé : 3000 exemples (1000 par classe)${NC}"
echo ""

# Étape 6 : Test de précision
echo -e "${YELLOW}[6/6] Évaluation de la précision...${NC}"
./my_torch_analyzer --predict "$NETWORK_FILE" test_validation.txt 2>&1 | tee predict_output.log

echo ""
echo "Résultats détaillés"
grep -A5 "METRIQUES" predict_output.log
echo ""

# Résumé final
echo -e "${GREEN}  ENTRAÎNEMENT TERMINÉ ${NC}"

# Nettoyage optionnel
rm -f chess_medium.nn test_validation.txt
echo -e "${GREEN}✓ Fichiers temporaires nettoyés${NC}"