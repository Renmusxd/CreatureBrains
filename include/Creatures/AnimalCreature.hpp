//
//  Creature.hpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/18/15.
//
//

#ifndef Creature_hpp
#define Creature_hpp

#include <stdio.h>
#include "Creatures/Creature.hpp"
#include "Brains/Brain.hpp"
#include "World/World.hpp"

class World;

class AnimalCreature : public Creature {
public:
    ~AnimalCreature(){destroy();};
    void init(Brain* brain, double x, double y, double dir, int id, int fam, int gen);
    void destroy();
    /* Update the brain */
    void think(World* w);
    /* Update position and physical details */
    void update(World* w);
    /* Gets input and output state */
    std::string getState();
    std::string getBrain();
    void attack(int delta){m_energy-=delta;m_health -= 25;}
    bool dead(){return m_health<=0 || m_energy<=0;}
    void inflict(int dam){m_health-=dam;}
private:
    std::string inputString();
    std::string outputString();

    int m_health;
    bool m_sprinting = false;
};

#endif /* Creature_hpp */