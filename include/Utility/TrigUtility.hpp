//
//  TrigUtility.h
//  BrainSim
//
//  Created by Sumner Hearth on 11/29/15.
//
//

#ifndef TrigUtility_h
#define TrigUtility_h

#include <math.h>

#define TWOPI 2*M_PI
#define TRIGENTRIES 1000
#define FILL 0

class TrigUtility{
public:
    static TrigUtility* getInstance();
    double mcos(double angle);
    double msin(double angle);
    double mtan(double angle);
private:
    static TrigUtility* m_instance;
    double* m_cosarray;
    double* m_sinarray;
    /* Changes the angle into an index from 0 to (TRIGENTRIES-1)*/
    int angleIndex(double angle);
    TrigUtility();
    ~TrigUtility();
};

#endif /* TrigUtility_h */
