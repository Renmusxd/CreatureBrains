//
//  NeuralBrain.cpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/18/15.
//
//
#include <string.h>
#include <sstream>
#include <cassert>
#include "Utility/NeuralUtility.hpp"
#include "Brains/NeuralBrain.hpp"

#define DEFAULT_NN 3

BrainIO NeuralBrain::init(int inputlen, int outputlen, int statelen, bool sharedmem){
    // Make one array that holds both senses and output,
    // return pointer to top for input, pointer to remaining for out
    // This allows speedup (less memory copying).

    BrainIO bio;
    m_sharedmem = sharedmem;
    m_inputlen = inputlen + statelen;
    m_statelen = statelen;
    m_outl = outputlen;
    max_n = m_outl; // Since there must be one layer

    if (layers == nullptr) {
        // Input array is length [inputlen + outputlen].
        // The length of the input neurons considered is [inputlen + statelen]
        // First input array starts after [outputlen - statelen] indices;
        if (m_sharedmem) {
            m_len = inputlen + outputlen;
            m_brainArray = new double[m_len];
            memset(m_brainArray, 0, m_len*sizeof(double));

            // [output ... state input ...] = m_brainArray
            //  \_______________/\_______/
            //      outputlen     inputlen
            //             \_____________/
            //               m_inputlen
            m_input = &m_brainArray[outputlen - statelen];
            m_output = &m_brainArray[0];
        } else {
            m_brainArray = nullptr;
            m_input = new double[m_inputlen];
            m_output = new double[m_outl];
        }

        // Make layers, num_layers setup in constructor
        layers = new neuronutility::NeuronLayer[num_layers];
        // If no constr_param, make one
        if (constr_param == nullptr) {
            constr_param = new int[num_layers];
            for (int i = 0; i < num_layers; i++) {
                constr_param[i] = DEFAULT_NN;
            }
        }
        // Now follow details of contr_param
        // let constr_param[-1] = m_inputlen (state + input)
        int last_size = m_inputlen;
        for (int i = 0; i < num_layers - 1; i++) {
            // Store matrix as 1D array of size nxm
            double *layer_matrix = new double[last_size * constr_param[i]];
            double *layer_bias = new double[constr_param[i]];
            layers[i].layer = layer_matrix;
            layers[i].bias = layer_bias;
            layers[i].num_neurons = constr_param[i];
            layers[i].num_inputs = last_size;
            neuronutility::initializeNeuronLayer(&layers[i]);

            last_size = constr_param[i];
            max_n = (last_size > max_n) ? last_size : max_n;
        }
        // Last one has to be of size (m_outl)x(last_size) (requires minimum 1 layer)
        double *layer_matrix = new double[m_outl * last_size];
        double *layer_bias = new double[m_outl];
        layers[num_layers - 1].layer = layer_matrix;
        layers[num_layers - 1].bias = layer_bias;
        layers[num_layers - 1].num_neurons = m_outl;
        layers[num_layers - 1].num_inputs = last_size;
        neuronutility::initializeNeuronLayer(&layers[num_layers - 1]);
        delete[] constr_param;
        constr_param = nullptr;
    } else {
        // Already setup
        assert(m_inputlen == inputlen + statelen);
        assert(!m_sharedmem || (m_len == inputlen + outputlen));
        assert(m_statelen == statelen);
        assert(m_outl == outputlen);
        assert(max_n == m_outl);


    }
    if (m_sharedmem){
        bio.output_array = &m_brainArray[0];
        bio.input_array  = &m_brainArray[outputlen];
    } else {
        bio.input_array = m_input;
        bio.output_array = m_output;
    }
    // Should now have num_layers layers of appropriate sizes to do matrix multiplication.


    return bio;
}

void NeuralBrain::destroy() {
    if (m_sharedmem) {
        if (m_brainArray != nullptr) {
            delete m_brainArray;
            m_brainArray = nullptr;
        }
    } else {
        delete m_input; delete m_output;
    }

    if (layers != nullptr) {
        layers = nullptr;
        delete layers;
    }
    m_input = nullptr; m_output = nullptr;
    // Do not free m_output (belongs to m_brainArray)
}

void NeuralBrain::update(){
    // This should go through and update the neural net with appropriate
    // matrix multiplications and sigmoid functions
    neuronutility::computeNeurons(layers, num_layers, m_input, m_inputlen, m_output, m_outl);
    if (!m_sharedmem){
        memcpy(&m_input[m_inputlen-m_statelen],
               &m_output[m_statelen],
               m_statelen*sizeof(double));
    }
}

void NeuralBrain::reward(double val){}

NeuralBrain* NeuralBrain::mutateCopy(double mut){
    NeuralBrain* new_brain = new NeuralBrain;

    new_brain->layers = new neuronutility::NeuronLayer[num_layers];
    for (int l = 0; l < num_layers; l++){
        new_brain->layers[l] = neuronutility::mutateNeuronLayer(layers[l],mut,mut);
    }

    new_brain->m_sharedmem = m_sharedmem;
    new_brain->m_len = m_len;

    new_brain->m_inputlen = m_inputlen;
    new_brain->m_statelen = m_statelen;

    new_brain->m_outl = m_outl;
    new_brain->max_n = new_brain->m_outl;

    if (m_sharedmem) {
        new_brain->m_brainArray = new double[m_len];
        memset(new_brain->m_brainArray, 0, m_len * (sizeof(double)));
        new_brain->m_input = &(new_brain->m_brainArray[m_statelen]);
        new_brain->m_output = new_brain->m_brainArray;
    } else {
        new_brain->m_input = new double[m_inputlen];
        new_brain->m_output = new double[m_outl];

        memset(new_brain->m_input, 0, m_inputlen * (sizeof(double)));
        memset(new_brain->m_output, 0, m_outl * (sizeof(double)));
    }

    return new_brain;
}

std::string NeuralBrain::details(){
    std::ostringstream strs;
    strs << "{";

    strs << "\"statesize:\"" << m_statelen;

    // The shape of the brain layers
    strs << ", \"shape\":[";
    for (int i = 0; i<num_layers; i++){
        strs << layers[i].num_inputs << ", ";
    }
    strs << layers[num_layers-1].num_neurons;
    strs << "]";

    strs << "}";

    return strs.str();
}


