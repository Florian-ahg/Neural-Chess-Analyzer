#include "serialisation_reseau.hpp"
#include <fstream>
#include <iostream>

bool sauvegarder_reseau(const Reseau &reseau, const std::string &chemin_fichier)
{
  std::ofstream fichier(chemin_fichier);
  if (!fichier.is_open())
  {
    std::cerr << "Error: cannot open file for writing: "
              << chemin_fichier << std::endl;
    return false;
  }

  int nb_couches = static_cast<int>(reseau.tailles_couches.size());
  if (nb_couches < 2)
  {
    std::cerr << "Error: the network must have at least 2 layers.\n";
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
    std::cerr << "Error: bias/weight vector sizes are inconsistent.\n";
    return false;
  }

  for (int c = 0; c < nb_couches - 1; ++c)
  {
    int nb_neurones_couche = reseau.tailles_couches[c + 1];
    int nb_neurones_precedente = reseau.tailles_couches[c];

    if (reseau.biais[c].size() != static_cast<std::size_t>(nb_neurones_couche) ||
        reseau.poids[c].size() != static_cast<std::size_t>(nb_neurones_couche))
    {
      std::cerr << "Error: inconsistent bias/weight size for layer " << c + 1
                << ".\n";
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
        std::cerr << "Error: invalid number of weights for layer "
                  << c + 1 << ", neuron " << n << ".\n";
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
    std::cerr << "Error: cannot open file for reading: "
              << chemin_fichier << std::endl;
    return false;
  }

  int nb_couches = 0;
  if (!(fichier >> nb_couches) || nb_couches < 2)
  {
    std::cerr << "Error: invalid number of layers in the file.\n";
    return false;
  }

  reseau.tailles_couches.clear();
  reseau.tailles_couches.resize(nb_couches);

  for (int i = 0; i < nb_couches; ++i)
  {
    if (!(fichier >> reseau.tailles_couches[i]))
    {
      std::cerr << "Error: failed to read the size of layer " << i << ".\n";
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
        std::cerr << "Error: failed to read the bias of layer " << c + 1
                  << ", neuron " << n << ".\n";
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
          std::cerr << "Error: failed to read weight (layer " << c + 1
                    << ", neuron " << n << ", weight " << p
                    << ").\n";
          return false;
        }
      }
    }
  }

  return true;
}
