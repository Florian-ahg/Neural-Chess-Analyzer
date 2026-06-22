#include "../../include/my_torch.hpp"

double Layer::activation(double x)
{
    return 1.0 / (1.0 + std::exp(-x));
}

double Layer::activationDerivative(double output)
{
    return output * (1.0 - output);
}

double Layer::relu(double x)
{
    return std::max(0.0, x);
}

double Layer::reluDerivative(double output)
{
    return output > 0.0 ? 1.0 : 0.0;
}

Layer::Layer(int input_size, int neuronCount, bool is_output)
    : input_size(input_size), neuronCount(neuronCount), is_output(is_output)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    double limit;
    if (is_output)
        limit = std::sqrt(6.0 / (input_size + neuronCount));
    else
        limit = std::sqrt(2.0 / input_size);

    std::uniform_real_distribution<> dis(-limit, limit);

    weights.resize(neuronCount, std::vector<double>(input_size));
    biases.resize(neuronCount);
    outputs.resize(neuronCount);
    deltas.resize(neuronCount);

    for (int i = 0; i < neuronCount; ++i)
    {
        biases[i] = 0.01;
        for (int j = 0; j < input_size; ++j)
        {
            weights[i][j] = dis(gen);
        }
    }
}

std::vector<double> Layer::forward(const std::vector<double> &input)
{
    if (input.size() != static_cast<size_t>(input_size))
    {
        return {};
    }

    for (int i = 0; i < neuronCount; ++i)
    {
        double sum = biases[i];
        for (int j = 0; j < input_size; ++j)
        {
            sum += weights[i][j] * input[j];
        }

        if (is_output)
            outputs[i] = activation(sum);
        else
            outputs[i] = relu(sum);
    }
    return outputs;
}