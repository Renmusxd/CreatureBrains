//
//  Brain.hpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/18/15.
//
//

#ifndef Brain_hpp
#define Brain_hpp

#include <stdio.h>
#include <string>

struct BrainIO{
    double* input_array;
    double* output_array;
};

class Brain{
public:
    virtual ~Brain() {};
    /* Makes a brain, owns returned array */
    virtual BrainIO init(int inputlen, int outputlen, int arg, bool opt) = 0;
    virtual void destroy() = 0;
    /* Updates output array based on input */
    virtual void update() = 0;
    virtual void reward(double val) = 0;
    virtual std::string type() = 0;
    virtual std::string details() = 0;
    virtual Brain* mutateCopy(double mut) = 0;
};

#endif /* Brain_hpp */
