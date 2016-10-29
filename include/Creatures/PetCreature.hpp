//
// Created by Sumner Hearth on 9/26/16.
//

#ifndef PetCreature_hpp
#define PetCreature_hpp

#include "Creatures/Creature.hpp"

class PetCreature : public Creature {
    ~PetCreature(){destroy();};
    void init(Brain* brain, double x, double y, double dir, int id, int fam, int gen);
    void destroy();
    /* Update the brain */
    void think(World* w);
    /* Update position and physical details */
    void update(World* w);
    /* Gets input and output state */
    std::string getState();
    std::string inputString();
    std::string outputString();
    std::string getBrain();

    void reward(double amount);

    void attack(int delta){}
    bool dead(){return false;}
    void inflict(int dam){}
};

class PetCreatureSensors{
public:
    PetCreatureSensors(PetCreature* c, World* w){m_creature = c; m_world = w;};
    ~PetCreatureSensors(){};
    /* 0.0 -> 1.0 */
    double getEnergy(){return m_creature->m_energy;}
    /* Angle=0 is straight ahead */
    Color getSight(double theta, double phi, double tfov, double pfov);
    double getDistance(double theta);
    double getReward();
private:
    double m_cameraheight = 1;
    double m_distrange = 130;
    double m_camerarange = 1000;

    PetCreature* m_creature;
    World* m_world;
};

class PetCreatureActions{
public:
    PetCreatureActions(PetCreature* c, World* w){m_creature = c; m_world = w;};
    ~PetCreatureActions(){};
    void leftWheel(double v){m_leftwheel += v;};
    void rightWheel(double v){m_rightwheel += v;};
    void move(double v){m_leftwheel += v; m_rightwheel += v;}
    void turn(double v){m_leftwheel -= v; m_rightwheel += v;}

    void commit();
private:
    Creature* m_creature;
    World* m_world;

    double m_leftwheel = 0;
    double m_rightwheel = 0;
};

#endif //PetCreature_hpp
