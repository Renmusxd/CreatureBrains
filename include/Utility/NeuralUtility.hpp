//
//  NeuralUtility.hpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/19/15.
//
//

#ifndef NeuralUtility_hpp
#define NeuralUtility_hpp

#include <cmath>
#include <cblas.h>
#include <random>

namespace neuronutility {
    
static std::default_random_engine generator(time(0));
    
    struct NeuronLayer{
        double* layer; // (num_neurons)x(num_inputs)
        double* bias;  // (num_neurons)x(1)
        int num_neurons;
        int num_inputs;
    };

    /* Initlializes the values of the array to optimize learning */
    void initializeNeuronLayer(NeuronLayer* n);

    /* Create a copy of the neuron layer and mutate it slightly */
    NeuronLayer mutateNeuronLayer(NeuronLayer n, double wmut, double bmut);

    /* Sets each value in array to be the sigmoid of itself */
    void sigmoidEach(double* array, int len);

    /* Computes the output through the neuron layers */
    void computeNeurons(NeuronLayer* layers, int num_layers,
                           double* input, int inputlen,
                           double* output, int outputlen,
                           int max_n);

    /* Computes the output through the neuron layers */
    void computeNeurons(NeuronLayer* layers, int num_layers,
                           double* input, int inputlen,
                           double* output, int outputlen);
    
}

#endif /* NeuralUtility_hpp */
