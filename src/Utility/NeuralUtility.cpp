#include <cmath>
#include <cblas.h>
#include <random>
#include "Utility/NeuralUtility.hpp"


namespace neuronutility {

    /* Initlializes the values of the array to optimize learning */
    void initializeNeuronLayer(NeuronLayer* n){

        std::normal_distribution<double> distribution(0,1.0/sqrt(n->num_inputs));

        for (int i = 0; i < (n->num_neurons) * (n->num_inputs); i++){
            n->layer[i] = distribution(generator);
        }
        for (int i = 0; i< (n->num_neurons); i++){
            n->bias[i] = distribution(generator);
        }
    }

    /* Create a copy of the neuron layer and mutate it slightly */
    NeuronLayer mutateNeuronLayer(NeuronLayer n, double wmut, double bmut){
        std::normal_distribution<double> wdist(-wmut,wmut);
        std::normal_distribution<double> bdist(-bmut,bmut);
        NeuronLayer new_n;
        new_n.layer = new double[n.num_neurons * n.num_inputs];
        new_n.bias = new double[n.num_neurons];
        new_n.num_inputs = n.num_inputs;
        new_n.num_neurons = n.num_neurons;

        for (int i = 0; i < (n.num_neurons) * (n.num_inputs); i++){
            new_n.layer[i] = n.layer[i] + wdist(generator);
        }
        for (int i = 0; i<(n.num_neurons); i++){
            new_n.bias[i] = n.bias[i] + bdist(generator);
        }

        return new_n;
    }

    /* Sets each value in array to be the sigmoid of itself */
    void sigmoidEach(double* array, int len){
        for (int i = 0; i<len; i++)
            array[i] = 1.0/(1.0 + exp(array[i]));
    }

    /* Computes the output through the neuron layers */
    void computeNeurons(NeuronLayer* layers, int num_layers,
                               double* input, int inputlen,
                               double* output, int outputlen,
                               int max_n){
        // inputlen is the size of the starting array, which means the first
        // layer should be a (n1)x(inputlen) matrix
        // Need some scratch space
        if (num_layers > 0){
            // Will alternate reading from scratchX and writing to scratchY
            double* scratch1 = new double[max_n]; memset(scratch1,0,sizeof(double)*max_n);
            double* scratch2 = new double[max_n]; memset(scratch2,0,sizeof(double)*max_n);

            // First copy input into scratch1
            memcpy(scratch1,input,inputlen * sizeof(double));

            int last_size = inputlen;
            for (int i = 0; i<num_layers-1; i++){
                if (layers[i].num_inputs != last_size){perror("Input size mismatch!\n");exit(1);}
                int n = layers[i].num_neurons;
                double* mat = layers[i].layer; // (n)x(last_size)

                // mat is (n)x(last_size)
                //
                if ( (i & 1) == 0){
                    memcpy(scratch2, layers[i].bias, n*sizeof(double));
                    // scratch2 <= mat             * scratch1        + scratch2
                    // (n)x(1)  <= (n)x(last_size) * (last_size)x(1) + (n)x(1)
                    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                                n, 1, last_size, 1.0, mat, last_size, scratch1, 1, 1.0, scratch2, 1);
                    sigmoidEach(scratch2, n);
                } else {
                    memcpy(scratch1, layers[i].bias, n*sizeof(double));
                    // scratch1 <= mat             * scratch2        + scratch1
                    // (n)x(1)  <= (n)x(last_size) * (last_size)x(1) + (n)x(1)
                    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                                n, 1, last_size, 1, mat, last_size, scratch2, 1, 0, scratch1, 1);
                    sigmoidEach(scratch1, n);
                }
                last_size = n;
            }
            if (layers[num_layers-1].num_inputs != last_size){perror("Input size mismatch!\n");exit(1);}
            // Last layer
            double* last_ptr = (num_layers%2 == 0)? scratch2 : scratch1;
            double* mat = layers[num_layers-1].layer;
            // output          <= mat                     * last_ptr
            // (outputlen)x(1) <= (outputlen)x(last_size) * (last_size)x(1)

            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                        outputlen, 1, last_size, 1.0, mat, last_size, last_ptr, 1, 0.0, output, 1);

            delete[] scratch1;
            delete[] scratch2;
        }
    }

    /* Computes the output through the neuron layers */
    void computeNeurons(NeuronLayer* layers, int num_layers,
                               double* input, int inputlen,
                               double* output, int outputlen){
        int max_n = inputlen;
        for (int i = 0; i<num_layers; i++){
            int t = layers[i].num_neurons;
            max_n = (t>max_n)? t : max_n;
        }
        computeNeurons(layers, num_layers, input, inputlen, output, outputlen, max_n);
    }

}


