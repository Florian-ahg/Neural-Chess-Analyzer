#include "lib.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <fstream>

static std::string extractBoard(const std::string& fen) {
    std::string board;
    board.reserve(fen.size());
    for (char ch : fen) {
        if (ch == ' ') break;
        board.push_back(ch);
    }
    return board;
}

static void translateBoardToValues(const std::string& board, std::vector<double>& out) {
    for (char c : board) {
        if (c == '/') {
            continue;
        } else if (c >= '1' && c <= '8') {
            int emptyCount = c - '0';
            for (int i = 0; i < emptyCount && out.size() < 64; ++i) {
                out.push_back(0.0);
            }
        } else {
            out.push_back(pieceToValue(c));
            if (out.size() == 64) break;
        }
    }
}

std::vector<double> fenToVector(const std::string &fen) {
    std::vector<double> out;
    out.reserve(64);

    const std::string board = extractBoard(fen);
    translateBoardToValues(board, out);

    if (out.size() < 64) {
        out.resize(64, 0.0);
    } else if (out.size() > 64) {
        out.resize(64);
    }

    return out;
}

