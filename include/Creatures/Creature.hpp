//
// Created by Sumner Hearth on 9/26/16.
//

#ifndef CREATUREBRAINS_CREATURE_H
#define CREATUREBRAINS_CREATURE_H

#include "World/World.hpp"
#include "Brains/Brain.hpp"

class World;

class Creature {
public:
    virtual ~Creature(){};
    virtual void init(Brain* brain, double x, double y, double dir, int id, int fam, int gen) = 0;
    virtual void destroy() = 0;
    /* Update the brain */
    virtual void think(World* w) = 0;
    /* Update position and physical details */
    virtual void update(World* w) = 0;
    /* Gets input and output state */
    virtual std::string getState() = 0;
    virtual std::string getBrain() = 0;
    virtual void attack(int delta) = 0;
    virtual bool dead() = 0;
    virtual void inflict(int dam) = 0;

    int id(){return m_id;};
    int fam(){return m_fam;};
    int energy(){return m_energy;}
    double x(){return m_xpos;};
    double y(){return m_ypos;};
    double d(){return m_dir;};
    double app(){return m_appearance;}
    void setx(double x){m_xpos = x;}
    void sety(double y){m_ypos = y;}

    Brain* m_brain;
    /* Can be any real input values */
    double* m_senses;
    /* Output is from 0 to 1 */
    double* m_actions;
    int m_senseslen;
    int m_actionslen;

    double m_xpos;
    double m_ypos;
    double m_dir;
    int m_id;
    int m_fam;
    int m_gen;
    int m_energy;
    double m_appearance;
};


#endif
