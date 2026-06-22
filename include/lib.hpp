#pragma once
#include <vector>
#include <string>

double pieceToValue(char c);
std::vector<double> labelToVector(const std::string &label);
void parseLine(const std::string &line, std::string &fen, std::string &label);
int runParsing(const std::string& path);
std::vector<double> fenToVector(const std::string &fen);