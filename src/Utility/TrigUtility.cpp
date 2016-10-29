//
//  TrigUtility.cpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/29/15.
//
//

#include "Utility/TrigUtility.hpp"

TrigUtility* TrigUtility::m_instance;

TrigUtility::TrigUtility(){
    m_cosarray = new double[TRIGENTRIES];
    m_sinarray = new double[TRIGENTRIES];
    for (int i = 0; i<TRIGENTRIES; i++){
        double angle = (TWOPI) * (i/(double)TRIGENTRIES);
        m_cosarray[i] = cos(angle);
        m_sinarray[i] = sin(angle);
    }
};

TrigUtility::~TrigUtility(){
    delete[] m_cosarray;
    delete[] m_sinarray;
};


TrigUtility* TrigUtility::getInstance(){
    if (TrigUtility::m_instance == nullptr){
        TrigUtility::m_instance = new TrigUtility();
    }
    return TrigUtility::m_instance;
}

double TrigUtility::mcos(double angle){
    int indx = angleIndex(angle);
    return m_cosarray[indx];
}

double TrigUtility::msin(double angle){
    int indx = angleIndex(angle);
    return m_sinarray[indx];
}

double TrigUtility::mtan(double angle){
    int indx = angleIndex(angle);
    return m_sinarray[indx]/m_cosarray[indx];
}

/* Changes the angle into an index from 0 to (TRIGENTRIES-1)*/
int TrigUtility::angleIndex(double angle){
    while (angle < 0){angle += TWOPI;}
    while (angle > TWOPI){ angle -= TWOPI;}
    int x = (int)(angle/(TWOPI) * (TRIGENTRIES-1))%TRIGENTRIES;
    return x;
}
