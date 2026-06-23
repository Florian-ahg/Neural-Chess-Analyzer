#!/usr/bin/env bash
#
# Functional test suite for my_torch.
# Builds the project, then checks the three tools without any (slow) training:
#   - the analyzer predicts with the provided pre-trained network
#   - the generator creates a blank network from a config file
#   - the dataset generator translates a FEN+label line into vectors
#
# Usage: ./tests/test.sh
# Exit code: 0 if all tests pass, 1 otherwise.

set -u
cd "$(dirname "$0")/.."

PASS=0
FAIL=0
green() { printf "\033[32m%s\033[0m" "$1"; }
red()   { printf "\033[31m%s\033[0m" "$1"; }

ok()   { echo "  [$(green PASS)] $1"; PASS=$((PASS + 1)); }
ko()   { echo "  [$(red FAIL)] $1"; [ -n "${2:-}" ] && echo "        $2"; FAIL=$((FAIL + 1)); }

echo "==> Building project"
make re >/dev/null 2>&1 || { echo "$(red 'Build failed')"; exit 1; }

echo "==> Binaries produced"
for b in my_torch_analyzer my_torch_generator my_torch_dataset_generator; do
    [ -x "./$b" ] && ok "$b built" || ko "$b built" "missing binary"
done

echo "==> Analyzer: predict with the pre-trained network"
out=$(timeout 60 ./my_torch_analyzer --predict my_torch_network.nn chessboards.txt 2>/dev/null)
n_in=$(grep -c . chessboards.txt)
n_out=$(printf "%s\n" "$out" | grep -c .)
if [ "$n_out" = "$n_in" ]; then
    ok "one prediction per chessboard ($n_out/$n_in)"
else
    ko "one prediction per chessboard" "got $n_out predictions for $n_in boards"
fi
# every prediction must be a known class
if printf "%s\n" "$out" | grep -qvE '^(Check|Checkmate|Nothing)'; then
    ko "predictions are valid classes" "unexpected label in output"
else
    ok "predictions are valid classes"
fi

echo "==> Generator: create a blank network from a config"
rm -f config.nn
timeout 30 ./my_torch_generator config.conf 1 >/dev/null 2>&1
if ls *.nn 2>/dev/null | grep -qv "^my_torch_network.nn$"; then
    ok "blank network file generated"
    rm -f $(ls *.nn | grep -v "^my_torch_network.nn$")
else
    ko "blank network file generated" "no .nn produced"
fi

echo "==> Dataset generator: translate a FEN+label line"
printf '%s Nothing\n' "$(head -1 chessboards.txt | awk '{print $1}')" > /tmp/torch_ds_in.txt
dsout=$(./my_torch_dataset_generator /tmp/torch_ds_in.txt 2>/dev/null)
rm -f /tmp/torch_ds_in.txt
if echo "$dsout" | grep -q "fen_vector:" && echo "$dsout" | grep -q "label_vector:"; then
    ok "produces fen_vector and label_vector"
else
    ko "produces fen_vector and label_vector" "$dsout"
fi

echo
echo "Results: $(green "$PASS passed"), $([ "$FAIL" -eq 0 ] && green "$FAIL failed" || red "$FAIL failed")"
[ "$FAIL" -eq 0 ]
