//
//  NPU.cpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/19/15.
//
//
#include <string.h>
#include "Brains/NPU.hpp"

BrainIO NPU::init(int inputlen, int outputlen, int arg, bool opt){
    // Make one array that holds both senses and output,
    // return pointer to top for input, pointer to remaining for out
    // This allows speedup (less memory copying).
    BrainIO bio;
    
    m_brainArray = new double[inputlen + outputlen];
    m_len = inputlen + outputlen; memset(m_brainArray, 0, m_len);
    bio.input_array  = &m_brainArray[0];
    bio.output_array = &m_brainArray[inputlen];
    m_output = bio.output_array;
    m_outl = outputlen;
    
    // Make layers
    
    return bio;
}

void NPU::destroy(){
    delete m_brainArray;
    m_brainArray = nullptr;
    // Do not free m_output (belongs to m_brainArray)
}

void NPU::update(){
    // Read m_brainArray
    
    // Update m_output
    
}

NPU* NPU::mutateCopy(double mut){
    return nullptr;
}

void NPU::reward(int val){
    
}

std::string NPU::details(){return " ";}