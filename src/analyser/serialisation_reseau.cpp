#include "serialisation_reseau.hpp"
#include <fstream>
#include <iostream>

bool sauvegarder_reseau(const Reseau &reseau, const std::string &chemin_fichier)
{
  std::ofstream fichier(chemin_fichier);
  if (!fichier.is_open())
  {
    std::cerr << "Erreur : impossible d'ouvrir le fichier pour ecriture : "
              << chemin_fichier << std::endl;
    return false;
  }

  int nb_couches = static_cast<int>(reseau.tailles_couches.size());
  if (nb_couches < 2)
  {
    std::cerr << "Erreur : le reseau doit avoir au moins 2 couches.\n";
    return false;
  }

  fichier << nb_couches << "\n";

  for (int i = 0; i < nb_couches; ++i)
  {
    fichier << reseau.tailles_couches[i];
    if (i + 1 < nb_couches)
      fichier << " ";
  }
  fichier << "\n";

  if (reseau.biais.size() != static_cast<std::size_t>(nb_couches - 1) ||
      reseau.poids.size() != static_cast<std::size_t>(nb_couches - 1))
  {
    std::cerr << "Erreur : tailles des vecteurs biais/poids incoherentes.\n";
    return false;
  }

  for (int c = 0; c < nb_couches - 1; ++c)
  {
    int nb_neurones_couche = reseau.tailles_couches[c + 1];
    int nb_neurones_precedente = reseau.tailles_couches[c];

    if (reseau.biais[c].size() != static_cast<std::size_t>(nb_neurones_couche) ||
        reseau.poids[c].size() != static_cast<std::size_t>(nb_neurones_couche))
    {
      std::cerr << "Erreur : taille des biais/poids pour la couche " << c + 1
                << " incoherente.\n";
      return false;
    }

    for (int n = 0; n < nb_neurones_couche; ++n)
    {
      fichier << reseau.biais[c][n];
      if (n + 1 < nb_neurones_couche)
        fichier << " ";
    }
    fichier << "\n";

    for (int n = 0; n < nb_neurones_couche; ++n)
    {
      const auto &poids_neurone = reseau.poids[c][n];

      if (poids_neurone.size() != static_cast<std::size_t>(nb_neurones_precedente))
      {
        std::cerr << "Erreur : nombre de poids invalide pour la couche "
                  << c + 1 << ", neurone " << n << ".\n";
        return false;
      }

      for (int p = 0; p < nb_neurones_precedente; ++p)
      {
        fichier << poids_neurone[p];
        if (p + 1 < nb_neurones_precedente)
          fichier << " ";
      }
      fichier << "\n";
    }
  }

  return true;
}

bool charger_reseau(Reseau &reseau, const std::string &chemin_fichier)
{
  std::ifstream fichier(chemin_fichier);
  if (!fichier.is_open())
  {
    std::cerr << "Erreur : impossible d'ouvrir le fichier pour lecture : "
              << chemin_fichier << std::endl;
    return false;
  }

  int nb_couches = 0;
  if (!(fichier >> nb_couches) || nb_couches < 2)
  {
    std::cerr << "Erreur : nombre de couches invalide dans le fichier.\n";
    return false;
  }

  reseau.tailles_couches.clear();
  reseau.tailles_couches.resize(nb_couches);

  for (int i = 0; i < nb_couches; ++i)
  {
    if (!(fichier >> reseau.tailles_couches[i]))
    {
      std::cerr << "Erreur : lecture de la taille de la couche " << i << " echouee.\n";
      return false;
    }
  }

  reseau.biais.clear();
  reseau.poids.clear();
  reseau.biais.resize(nb_couches - 1);
  reseau.poids.resize(nb_couches - 1);

  for (int c = 0; c < nb_couches - 1; ++c)
  {
    int nb_neurones_couche = reseau.tailles_couches[c + 1];
    int nb_neurones_precedente = reseau.tailles_couches[c];

    reseau.biais[c].resize(nb_neurones_couche);
    reseau.poids[c].resize(nb_neurones_couche);

    for (int n = 0; n < nb_neurones_couche; ++n)
    {
      if (!(fichier >> reseau.biais[c][n]))
      {
        std::cerr << "Erreur : lecture du biais de la couche " << c + 1
                  << ", neurone " << n << " echouee.\n";
        return false;
      }
    }

    for (int n = 0; n < nb_neurones_couche; ++n)
    {
      reseau.poids[c][n].resize(nb_neurones_precedente);
      for (int p = 0; p < nb_neurones_precedente; ++p)
      {
        if (!(fichier >> reseau.poids[c][n][p]))
        {
          std::cerr << "Erreur : lecture du poids (couche " << c + 1
                    << ", neurone " << n << ", poids " << p
                    << ") echouee.\n";
          return false;
        }
      }
    }
  }

  return true;
}
