#!/usr/bin/env bash
#
# Scripted demo of my_torch, meant to be screenshotted.
# Uses the provided pre-trained network (no slow training involved).
#
# Usage: ./screenshots/demo.sh

cd "$(dirname "$0")/.."
[ -x ./my_torch_analyzer ] || make >/dev/null 2>&1

BLUE='\033[1;34m'; GREEN='\033[1;32m'; DIM='\033[2m'; RST='\033[0m'

run() {
    printf "${BLUE}\$ ${GREEN}%s${RST}\n" "$1"
    sleep 0.5
    eval "$1"
    echo
    sleep 1
}

clear
printf "${DIM}# my_torch — neural network chessboard analyzer${RST}\n\n"

printf "${DIM}# 1. The chessboards to analyze (FEN notation)${RST}\n"
run "cat chessboards.txt"

printf "${DIM}# 2. Predict the game state with the pre-trained network${RST}\n"
run "./my_torch_analyzer --predict my_torch_network.nn chessboards.txt"

printf "${DIM}# 3. Generate a blank network from a config file${RST}\n"
run "./my_torch_generator config.conf 1"

printf "${DIM}# 4. Translate a board+label into network input/output vectors${RST}\n"
run "echo '$(head -1 chessboards.txt | awk '{print $1}') Nothing' | tee /tmp/one_board.txt >/dev/null; ./my_torch_dataset_generator /tmp/one_board.txt"

# cleanup demo artifacts
rm -f /tmp/one_board.txt $(ls *.nn 2>/dev/null | grep -v '^my_torch_network.nn$') 2>/dev/null
printf "${DIM}# Done.${RST}\n"
