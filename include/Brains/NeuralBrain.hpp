//
//  NeuralBrain.hpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/18/15.
//
//

#ifndef NeuralBrain_hpp
#define NeuralBrain_hpp

#include <stdio.h>
#include "Utility/NeuralUtility.hpp"
#include "Brain.hpp"

class NeuralBrain : public Brain {
public:
    NeuralBrain(){num_layers = 2; layers = nullptr;};
    NeuralBrain(int nl, int* nn){num_layers = (nl<=1)?1:nl; constr_param=nn; layers = nullptr;};
    ~NeuralBrain(){destroy();};
    
    BrainIO init(int inputlen, int outputlen, int statelen, bool sharedmem);
    void destroy();
    void update();
    void reward(double val);
    std::string type(){return "NNB";};
    std::string details();
    NeuralBrain* mutateCopy(double mut);
private:
    double* m_brainArray;
    double *m_input;
    int m_len;
    int m_inputlen;
    double* m_output; // For simplicity
    int m_outl;
    int m_statelen;
    
    // first layer is (n1)x(m_len)
    // second layer is (n2)x(n1)
    // ...
    // last layer is (m_outl)x(n[num_layers])
    neuronutility::NeuronLayer* layers;
    int num_layers = 2;
    int max_n = 0;
    int* constr_param = nullptr;

    bool m_sharedmem;
};

#endif /* NeuralBrain_hpp */
