#include "lib.hpp"
#include <cctype>

double pieceToValue(char c) {
    switch (c) {
        case 'P': return 0.1;
        case 'N': return 0.2;
        case 'B': return 0.3;
        case 'R': return 0.4;
        case 'Q': return 0.5;
        case 'K': return 0.6;
        case 'p': return -0.1;
        case 'n': return -0.2;
        case 'b': return -0.3;
        case 'r': return -0.4;
        case 'q': return -0.5;
        case 'k': return -0.6;
        default:  return 0.0;
    }
}

std::vector<double> labelToVector(const std::string &label) {
    std::string first;
    first.reserve(label.size());
    for (char ch : label) {
        if (std::isspace(static_cast<unsigned char>(ch))) break;
        first.push_back(ch);
    }
    for (char &ch : first) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    if (first == "checkmate") return {1.0, 0.0, 0.0};
    if (first == "check")     return {0.0, 1.0, 0.0};
    if (first == "nothing")   return {0.0, 0.0, 1.0};
    return {0.0, 0.0, 0.0};
}

static std::vector<std::string> tokenizeBySpaces(const std::string& line) {
    std::vector<std::string> tokens;
    tokens.reserve(16);
    std::string cur;
    for (char ch : line) {
        if (std::isspace(static_cast<unsigned char>(ch))) {
            if (!cur.empty()) {
                tokens.push_back(cur);
                cur.clear();
            }
        } else {
            cur.push_back(ch);
        }
    }
    if (!cur.empty()) tokens.push_back(cur);
    return tokens;
}

static std::string buildFenFromTokens(const std::vector<std::string>& tokens, std::size_t fenFields = 6) {
    if (tokens.size() < fenFields) {
        std::string line;
        for (std::size_t i = 0; i < tokens.size(); ++i) {
            if (i) line.push_back(' ');
            line += tokens[i];
        }
        return line;
    }
    std::string fen;
    for (std::size_t i = 0; i < fenFields; ++i) {
        if (i) fen.push_back(' ');
        fen += tokens[i];
    }
    return fen;
}

static std::string buildLabelFromTokens(const std::vector<std::string>& tokens, std::size_t fenFields = 6) {
    if (tokens.size() <= fenFields) return std::string{};
    std::string label;
    for (std::size_t i = fenFields; i < tokens.size(); ++i) {
        if (i != fenFields) label.push_back(' ');
        label += tokens[i];
    }
    return label;
}

void parseLine(const std::string &line, std::string &fen, std::string &label) {
    fen.clear();
    label.clear();
    if (line.empty()) return;
    const auto tokens = tokenizeBySpaces(line);
    fen = buildFenFromTokens(tokens, 6);
    label = buildLabelFromTokens(tokens, 6);
}
