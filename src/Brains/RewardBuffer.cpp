//
//  RewardBuffer.cpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/21/15.
//
//
#include <cstdlib>
#include <string.h>
#include "Brains/RewardBuffer.hpp"
namespace neuronutility{
    
void SlowRewardBuffer::init(NeuronLayer* layers, int numlayers, int inputlen, int bufferlen){
    m_length = bufferlen;
    m_inputlen = inputlen;
    m_layers = layers;
    m_numlayers = numlayers;
    // [input1, input2, ...]
    m_information = new int*[m_length];
    for (int i = 0; i<m_length; i++){
        m_information[i] = new int[m_inputlen];
    }
}

void SlowRewardBuffer::destroy(){
    for (int i = 0; i<m_length; i++){
        delete [] m_information[i];
    }
    delete [] m_information;
    // Do not delete layers
}


void SlowRewardBuffer::log(int* inputarray){
    /* First use last one */
    if (m_size < m_length){m_size++;}
    
    /* Write to m_head */
    memcpy(&m_information[m_head], inputarray, m_inputlen*sizeof(double));
    // increase m_head
    m_head = (m_head + 1)%(m_length);
}

void SlowRewardBuffer::reward(int x, double factor){
    int upto = (x>m_size)? m_size : x;
    for (int i = 0; i<upto; i++){
        // Work backwards from m_head
        int loc = abs((m_head - i)%m_length);
        rewardAt(loc, (x-i)*factor);
    }
}

void SlowRewardBuffer::reward(double *rewards, int len){
    int upto = (len>m_size)? m_size : len;
    for (int i = 0; i<upto; i++){
        // Work backwards from m_head
        int loc = std::abs(((m_head - i)%m_length));
        rewardAt(loc, rewards[i]);
    }
}

void SlowRewardBuffer::rewardAt(int loc, double val) {
    /* Run through forwardfeed,
     * calculate alpha at each iteration
     * calculate delta for each iteration (backwards)
     */
    
}
    
}


