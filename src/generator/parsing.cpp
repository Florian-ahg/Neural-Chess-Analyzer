#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include "lib.hpp"

static void processLine(const std::string& line) {
    bool hasNonSpace = false;
    for (char ch : line) {
        if (!std::isspace(static_cast<unsigned char>(ch))) { hasNonSpace = true; break; }
    }
    if (!hasNonSpace) return;

    std::string fen, label;
    parseLine(line, fen, label);
}

static int processFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::cerr << "Cannot open file: " << path << "\n";
        return 84;
    }

    std::string line;
    while (std::getline(in, line)) {
        processLine(line);
    }
    return 0;
}

int runParsing(const std::string& path) {
    return processFile(path);
}
