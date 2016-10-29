//
//  NPU.hpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/19/15.
//
//

#ifndef NPU_hpp
#define NPU_hpp

#include <stdio.h>
#include "Brain.hpp"

class NPU : public Brain {
public:
    ~NPU(){destroy();};
    BrainIO init(int inputlen, int outputlen, int arg, bool opt);
    void destroy();
    void update();
    void reward(int val);
    std::string type(){return "NPU";};
    std::string details();
    NPU* mutateCopy(double mut);
private:
    double* m_brainArray;
    int m_len;
    double* m_output; // For simplicity
    int m_outl;
};

#endif /* NPU_hpp */
