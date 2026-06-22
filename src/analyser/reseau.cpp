#include "reseau.hpp"
#include "my_torch.hpp"

Network reseau_vers_network(const Reseau &r)
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

Reseau creer_reseau(const std::vector<int> &tailles)
{
  Network net(tailles);

  Reseau r;
  r.tailles_couches = tailles;

  auto &layers = net.getLayers();
  for (const auto &layer : layers)
  {
    r.biais.push_back(layer.getBiases());
    r.poids.push_back(layer.getWeights());
  }

  return r;
}

std::vector<double> propagation_avant(const Reseau &reseau,
                                      const std::vector<double> &entree)
{
  Network net = reseau_vers_network(reseau);

  return net.predict(entree);
}

void entrainer_sur_un_exemple(Reseau &reseau,
                              const std::vector<double> &entree,
                              const std::vector<double> &sortie_voulue,
                              double taux_apprentissage)
{
  Network net = reseau_vers_network(reseau);

  TrainingData data;
  data.input = entree;
  data.target = sortie_voulue;
  std::vector<TrainingData> batch = {data};

  net.train(batch, 1, taux_apprentissage);

  auto &layers = net.getLayers();
  for (size_t i = 0; i < layers.size(); ++i)
  {
    reseau.biais[i] = layers[i].getBiases();
    reseau.poids[i] = layers[i].getWeights();
  }
}
