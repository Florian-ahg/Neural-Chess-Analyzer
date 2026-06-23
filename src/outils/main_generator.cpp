#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "reseau.hpp"
#include "serialisation_reseau.hpp"

static void afficher_usage()
{
  std::cout << "USAGE" << std::endl;
  std::cout << "    ./my_torch_generator config_file_1 nb_1 [config_file_2 nb_2...]" << std::endl;
  std::cout << "DESCRIPTION" << std::endl;
  std::cout << "    config_file_i Configuration file containing description of a neural network we want" << std::endl;
  std::cout << "                  to generate." << std::endl;
  std::cout << "    nb_i          Number of neural networks to generate based on the configuration file." << std::endl;
}

static bool lire_topologie(const std::string &chemin, std::vector<int> &tailles)
{
  std::ifstream fichier(chemin);
  if (!fichier.is_open())
  {
    std::cerr << "Error: cannot open config file: "
              << chemin << std::endl;
    return false;
  }

  tailles.clear();
  int valeur = 0;

  while (fichier >> valeur)
  {
    if (valeur <= 0)
    {
      std::cerr << "Error: invalid layer size (" << valeur
                << ") in " << chemin << std::endl;
      return false;
    }
    tailles.push_back(valeur);
  }

  if (tailles.size() < 2)
  {
    std::cerr << "Error: at least one input and one output layer are required in "
              << chemin << std::endl;
    return false;
  }

  return true;
}

static std::string extraire_nom_base(const std::string &chemin)
{
  std::size_t pos_slash = chemin.find_last_of("/\\");
  std::string nom = (pos_slash == std::string::npos)
                        ? chemin
                        : chemin.substr(pos_slash + 1);

  std::size_t pos_point = nom.find_last_of('.');
  if (pos_point != std::string::npos)
    nom = nom.substr(0, pos_point);

  return nom;
}

static int generer_pour_config(const std::string &fichier_config, int nb)
{
  std::vector<int> tailles;
  if (!lire_topologie(fichier_config, tailles))
    return 84;

  std::string base = extraire_nom_base(fichier_config);

  for (int i = 0; i < nb; ++i)
  {
    Reseau reseau = creer_reseau(tailles);

    std::string nom_fichier;
    if (nb == 1)
      nom_fichier = base + ".nn";
    else
      nom_fichier = base + "_" + std::to_string(i + 1) + ".nn";

    if (!sauvegarder_reseau(reseau, nom_fichier))
    {
      std::cerr << "Error: failed to save the network to "
                << nom_fichier << std::endl;
      return 84;
    }

    std::cout << "Network generated: " << nom_fichier << std::endl;
  }

  return 0;
}

int main(int argc, char **argv)
{
  if (argc == 2 && std::string(argv[1]) == "--help")
  {
    afficher_usage();
    return 0;
  }

  if (argc < 3 || ((argc - 1) % 2) != 0)
  {
    afficher_usage();
    return 84;
  }

  for (int i = 1; i < argc; i += 2)
  {
    std::string fichier_config = argv[i];
    int nb = 0;

    try
    {
      nb = std::stoi(argv[i + 1]);
    }
    catch (const std::exception &)
    {
      std::cerr << "Error: '" << argv[i + 1]
                << "' is not a valid number." << std::endl;
      return 84;
    }

    if (nb <= 0)
    {
      std::cerr << "Error: the number of networks to generate must be > 0." << std::endl;
      return 84;
    }

    int code = generer_pour_config(fichier_config, nb);
    if (code != 0)
      return code;
  }

  return 0;
}
