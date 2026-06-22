#pragma once

#include <vector>

struct Reseau
{
    std::vector<int> tailles_couches;

    std::vector<std::vector<double>> biais;

    std::vector<std::vector<std::vector<double>>> poids;
};

Reseau creer_reseau(const std::vector<int> &tailles);

std::vector<double> propagation_avant(const Reseau &reseau,
                                      const std::vector<double> &entree);

void entrainer_sur_un_exemple(Reseau &reseau,
                              const std::vector<double> &entree,
                              const std::vector<double> &sortie_voulue,
                              double taux_apprentissage);
