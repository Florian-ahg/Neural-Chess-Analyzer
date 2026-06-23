#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <sstream>
#include "../../include/my_torch.hpp"
#include "reseau.hpp"
#include "serialisation_reseau.hpp"
#include "parseur_fen.hpp"
#include "encodage_sorties.hpp"

Network convertir_en_moteur(const Reseau &r)
{
    Network net(r.tailles_couches);
    auto &layers = net.getLayers();
    for (size_t i = 0; i < layers.size(); ++i)
    {
        layers[i].getBiases() = r.biais[i];
        layers[i].getWeights() = r.poids[i];
    }
    return net;
}

void sauvegarder_dans_structure(Reseau &r, Network &net)
{
    auto &layers = net.getLayers();
    for (size_t i = 0; i < layers.size(); ++i)
    {
        r.biais[i] = layers[i].getBiases();
        r.poids[i] = layers[i].getWeights();
    }
}

void afficher_usage()
{
    std::cout << "USAGE" << std::endl;
    std::cout << "    ./my_torch_analyzer [--predict | --train [--save SAVEFILE]] LOADFILE CHESSFILE" << std::endl;
}

bool parser_ligne_dataset(const std::string &ligne, std::string &fen_out, std::string &label_out)
{
    if (ligne.empty())
        return false;
    std::stringstream ss(ligne);
    std::string mot1, mot2;
    ss >> mot1;

    bool est_un_id = !mot1.empty() && std::all_of(mot1.begin(), mot1.end(), ::isdigit);
    if (est_un_id)
    {
        if (!(ss >> mot2))
            return false;
        fen_out = mot2;
    }
    else
    {
        fen_out = mot1;
    }

    if (ligne.find("Checkmate White") != std::string::npos)
        label_out = "Checkmate White";
    else if (ligne.find("Checkmate Black") != std::string::npos)
        label_out = "Checkmate Black";
    else if (ligne.find("Check White") != std::string::npos)
        label_out = "Check White";
    else if (ligne.find("Check Black") != std::string::npos)
        label_out = "Check Black";
    else if (ligne.find("Nothing") != std::string::npos)
        label_out = "Nothing";
    else
        label_out = "";

    return !label_out.empty();
}

int mode_predict(const std::string &fichier_reseau, const std::string &fichier_echiquiers)
{
    Reseau reseau_struct;
    if (!charger_reseau(reseau_struct, fichier_reseau))
        return 84;

    Network net = convertir_en_moteur(reseau_struct);

    std::ifstream fichier(fichier_echiquiers);
    if (!fichier.is_open())
        return 84;

    std::string ligne;
    int total = 0, correct = 0;
    std::map<std::string, int> correct_par_classe, total_par_classe;

    while (std::getline(fichier, ligne))
    {
        if (ligne.empty())
            continue;
        std::string fen, label_attendu;
        parser_ligne_dataset(ligne, fen, label_attendu);

        std::vector<double> entree;
        if (!convertir_fen_en_entrees(fen, reseau_struct, entree))
            continue;

        std::vector<double> sortie = net.predict(entree);
        std::string label_predit = sortie_vers_label(sortie);

        std::cout << label_predit << "\n";

        if (!label_attendu.empty())
        {
            total++;
            total_par_classe[label_attendu]++;
            if (label_predit == label_attendu)
            {
                correct++;
                correct_par_classe[label_attendu]++;
            }
        }
    }
    if (total > 0)
    {
        std::cerr << "\n=== METRIQUES ===" << std::endl;
        std::cerr << "Precision globale : " << (100.0 * correct / total) << "% ("
                  << correct << "/" << total << ")" << std::endl;
        for (const auto &p : total_par_classe)
        {
            int c = correct_par_classe[p.first];
            std::cerr << "  " << p.first << " : " << (100.0 * c / p.second) << "% ("
                      << c << "/" << p.second << ")" << std::endl;
        }
    }

    return 0;
}

int mode_train(const std::string &fichier_reseau, const std::string &fichier_echiquiers, const std::string &fichier_sauvegarde)
{
    Reseau reseau_struct;
    if (!charger_reseau(reseau_struct, fichier_reseau))
        return 84;

    std::cout << "Construction du moteur neuronal..." << std::endl;
    Network net = convertir_en_moteur(reseau_struct);

    std::cout << "Chargement du dataset..." << std::endl;
    std::vector<TrainingData> training_set;
    std::ifstream fichier(fichier_echiquiers);
    if (!fichier.is_open())
        return 84;

    std::string ligne;
    while (std::getline(fichier, ligne))
    {
        std::string fen, label;
        if (!parser_ligne_dataset(ligne, fen, label))
            continue;
        if (label.empty())
            continue;

        std::vector<double> entree;
        if (!convertir_fen_en_entrees(fen, reseau_struct, entree))
            continue;

        TrainingData data;
        data.input = entree;
        data.target = label_vers_sortie(label);
        training_set.push_back(data);
    }
    std::cout << "Dataset: " << training_set.size() << " examples." << std::endl;

    if (training_set.empty())
    {
        std::cerr << "Error: no valid example found." << std::endl;
        return 84;
    }

    std::cout << "\n=== TRAINING START ===" << std::endl;
    std::cout << "Epochs: 300" << std::endl;
    std::cout << "Learning rate: 0.05" << std::endl
              << std::endl;

    net.train(training_set, 300, 0.05);

    std::cout << "\nUpdating weights..." << std::endl;
    sauvegarder_dans_structure(reseau_struct, net);

    std::string dest = fichier_sauvegarde.empty() ? fichier_reseau : fichier_sauvegarde;
    if (!sauvegarder_reseau(reseau_struct, dest))
    {
        std::cerr << "Error: failed to save the network" << std::endl;
        return 84;
    }

    std::cout << "Saved to " << dest << std::endl;
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        afficher_usage();
        return 84;
    }

    std::string mode = argv[1];

    if (mode == "--predict")
    {
        if (argc != 4)
        {
            afficher_usage();
            return 84;
        }
        return mode_predict(argv[2], argv[3]);
    }
    else if (mode == "--train")
    {
        std::string save, load = argv[2], chess = argv[3];

        if (argc == 6 && std::string(argv[2]) == "--save")
        {
            save = argv[3];
            load = argv[4];
            chess = argv[5];
        }
        else if (argc != 4)
        {
            afficher_usage();
            return 84;
        }

        return mode_train(load, chess, save);
    }

    afficher_usage();
    return 84;
}
