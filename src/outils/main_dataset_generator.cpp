#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include "lib.hpp"

void usage()
{
    std::cout << "USAGE\n";
    std::cout << "    ./my_torch_dataset_generator <dataset_file>\n";
    std::cout << "DESCRIPTION\n";
    std::cout << "    Lit un fichier dataset, extrait FEN et label par ligne,\n";
    std::cout << "    traduit la FEN en 64 valeurs et le label en one-hot (3 valeurs).\n";
}

static bool hasNonSpaceChars(const std::string &line)
{
    for (char ch : line)
    {
        if (!std::isspace(static_cast<unsigned char>(ch)))
            return true;
    }
    return false;
}

static void translateAndPrintLine(const std::string &line)
{
    std::string fen, label;
    parseLine(line, fen, label);

    std::vector<double> fenVec = fenToVector(fen);
    std::vector<double> labelVec = labelToVector(label);

    std::cout << "fen_vector:";
    for (size_t i = 0; i < fenVec.size(); ++i)
    {
        std::cout << (i == 0 ? " " : ", ") << fenVec[i];
    }
    std::cout << "\n";

    std::cout << "label_vector:";
    for (size_t i = 0; i < labelVec.size(); ++i)
    {
        std::cout << (i == 0 ? " " : ", ") << labelVec[i];
    }
    std::cout << "\n---\n";
}

static int translateFenAndLabelFromFile(const std::string &path)
{
    std::ifstream in(path);
    if (!in)
    {
        std::cerr << "Cannot open file: " << path << "\n";
        return 84;
    }

    std::string line;
    while (std::getline(in, line))
    {
        if (!hasNonSpaceChars(line))
            continue;
        translateAndPrintLine(line);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && std::string(argv[1]) == "--help")
    {
        usage();
        return 0;
    }
    if (argc < 2)
    {
        std::cerr << "No file path provided.\n";
        return 84;
    }
    return translateFenAndLabelFromFile(argv[1]);
}
