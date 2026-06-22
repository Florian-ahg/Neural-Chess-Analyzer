#pragma once

#include <string>
#include "reseau.hpp"

bool sauvegarder_reseau(const Reseau &reseau, const std::string &chemin_fichier);

bool charger_reseau(Reseau &reseau, const std::string &chemin_fichier);
