#include "../../include/my_torch.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <numeric>

Network::Network() {}

Network::Network(const std::vector<int> &topology)
{
    for (size_t i = 1; i < topology.size(); ++i)
    {
        bool isLast = (i == topology.size() - 1);
        layers.emplace_back(Layer(topology[i - 1], topology[i], isLast));
    }
}

std::vector<double> Network::predict(const std::vector<double> &input)
{
    std::vector<double> current_output = input;
    for (auto &layer : layers)
    {
        current_output = layer.forward(current_output);
    }
    return current_output;
}

void Network::train(const std::vector<TrainingData> &dataset, int epochs, double learning_rate)
{
    std::cout << "Starting training on " << dataset.size() << " examples..." << std::endl;

    std::vector<size_t> indices(dataset.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::random_device rd;
    std::mt19937 g(rd());

    for (int epoch = 0; epoch < epochs; ++epoch)
    {
        double totalError = 0.0;

        std::shuffle(indices.begin(), indices.end(), g);

        for (size_t idx : indices)
        {
            const auto &data = dataset[idx];

            std::vector<double> finalOutput = predict(data.input);

            Layer &outputLayer = layers.back();
            std::vector<double> &outputDeltas = outputLayer.getDeltas();
            const std::vector<double> &outputvalue = outputLayer.getOutputs();

            for (size_t k = 0; k < finalOutput.size(); ++k)
            {
                double err = data.target[k] - finalOutput[k];
                totalError += err * err;
                outputDeltas[k] = err * Layer::activationDerivative(outputvalue[k]);
            }

            for (int k = layers.size() - 2; k >= 0; --k)
            {
                Layer &currentLayer = layers[k];
                Layer &nextLayer = layers[k + 1];

                std::vector<double> &currentDeltas = currentLayer.getDeltas();
                const std::vector<double> &currentOutputs = currentLayer.getOutputs();

                for (size_t j = 0; j < currentDeltas.size(); ++j)
                {
                    double error = 0.0;
                    for (size_t l = 0; l < nextLayer.getDeltas().size(); ++l)
                    {
                        error += nextLayer.getDeltas()[l] * nextLayer.getWeights()[l][j];
                    }

                    if (currentLayer.isOutputLayer())
                    {
                        currentDeltas[j] = error * Layer::activationDerivative(currentOutputs[j]);
                    }
                    else
                    {
                        currentDeltas[j] = error * Layer::reluDerivative(currentOutputs[j]);
                    }
                }
            }

            for (size_t k = 0; k < layers.size(); ++k)
            {
                Layer &layer = layers[k];
                const std::vector<double> &layerInput = (k == 0) ? data.input : layers[k - 1].getOutputs();

                for (size_t j = 0; j < layer.getDeltas().size(); ++j)
                {
                    double delta = layer.getDeltas()[j];
                    layer.getBiases()[j] += learning_rate * delta;
                    for (size_t l = 0; l < layerInput.size(); ++l)
                    {
                        layer.getWeights()[j][l] += learning_rate * delta * layerInput[l];
                    }
                }
            }
        }

        if ((epoch + 1) % 10 == 0 || epoch == 0 || epoch == epochs - 1)
        {
            std::cout << "Epoch " << epoch + 1 << "/" << epochs
                      << " - MSE Error: " << totalError / dataset.size() << std::endl;
        }
        
        if ((epoch + 1) % 50 == 0 && epoch > 0)
        {
            learning_rate *= 0.9;
        }
    }
}

void Network::save(const std::string &filename) const
{
    std::ofstream file(filename);
    if (!file.is_open())
        return;

    file << layers.size() + 1 << "\n";
    if (!layers.empty())
        file << layers[0].getInputSize();
    for (const auto &layer : layers)
    {
        file << " " << layer.getNeuronCount();
    }
    file << "\n";

    for (const auto &layer : layers)
    {
        for (double b : layer.getBiases())
            file << b << " ";
        file << "\n";
        for (const auto &row : layer.getWeights())
        {
            for (double w : row)
                file << w << " ";
            file << "\n";
        }
    }
    file.close();
    std::cout << "Network saved to " << filename << std::endl;
}

bool Network::load(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return false;

    int nb_layers;
    if (!(file >> nb_layers))
        return false;

    std::vector<int> topology;
    int size;
    for (int i = 0; i < nb_layers; ++i)
    {
        file >> size;
        topology.push_back(size);
    }

    layers.clear();
    for (size_t i = 1; i < topology.size(); ++i)
    {
        bool isLast = (i == topology.size() - 1);
        layers.emplace_back(Layer(topology[i - 1], topology[i], isLast));
    }

    for (auto &layer : layers)
    {
        for (double &b : layer.getBiases())
            file >> b;
        for (auto &row : layer.getWeights())
        {
            for (double &w : row)
                file >> w;
        }
    }
    return true;
}