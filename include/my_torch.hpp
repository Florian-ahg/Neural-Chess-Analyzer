#ifndef MY_TORCH_HPP
#define MY_TORCH_HPP

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <random>

struct TrainingData
{
    std::vector<double> input;
    std::vector<double> target;
};

class Layer
{
public:
    Layer(int input_size, int neuroneCount, bool is_output = false);

    std::vector<double> forward(const std::vector<double> &input);

    std::vector<std::vector<double>> &getWeights() { return weights; }
    const std::vector<std::vector<double>> &getWeights() const { return weights; }
    std::vector<double> &getBiases() { return biases; }
    const std::vector<double> &getBiases() const { return biases; }
    std::vector<double> &getOutputs() { return outputs; }
    const std::vector<double> &getOutputs() const { return outputs; }
    std::vector<double> &getDeltas() { return deltas; }
    const std::vector<double> &getDeltas() const { return deltas; }
    int getInputSize() const { return input_size; }
    int getNeuronCount() const { return neuronCount; }

    static double activation(double x);
    static double activationDerivative(double x);

    static double relu(double x);
    static double reluDerivative(double x);

    bool isOutputLayer() const { return is_output; }

private:
    int input_size;
    int neuronCount;
    bool is_output;

    std::vector<std::vector<double>> weights;
    std::vector<double> biases;

    std::vector<double> outputs;
    std::vector<double> deltas;
};

class Network
{
public:
    Network(const std::vector<int> &topology);
    Network();

    void save(const std::string &filename) const;
    bool load(const std::string &filename);

    std::vector<double> predict(const std::vector<double> &input);
    void train(const std::vector<TrainingData> &data, int epochs, double learning_rate);

    std::vector<Layer> &getLayers() { return layers; }
    const std::vector<Layer> &getLayers() const { return layers; }

private:
    std::vector<Layer> layers;
};

std::vector<double> fenToInput(const std::string &fen);

#endif