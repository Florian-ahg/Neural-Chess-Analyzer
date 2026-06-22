#pragma once

#include <string>
#include <vector>
#include "reseau.hpp"

bool convertir_fen_en_entrees(const std::string &fen,
                              const Reseau &reseau,
                              std::vector<double> &entree);
