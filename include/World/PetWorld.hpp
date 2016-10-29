//
// Created by Sumner Hearth on 9/26/16.
//

#ifndef CREATUREBRAINS_PETWORLD_HPP
#define CREATUREBRAINS_PETWORLD_HPP

#include "World/World.hpp"

class PetCreature;

class PetWorld: public World{
    ~PetWorld(){destroy();};
    void init(int xbound, int ybound, bool genterrain);
    void destroy();
    /* Multithread think, then consectutive update */
    void update();
    /* Adds creature to world, world is now owner */
    void addCreature(Creature* c);
    int addCreature(Brain* b, double x, double y, double dir);
    int addCreature(Brain* b, double x, double y, double dir, int fam, int gen);
    /* Makes string describing world state */
    std::string getState();
    std::string getTerrain();
    /* Gets creature by id number, caches results */
    Creature* getCreatureByID(int id);
    int eat(double x, double y, int maxamount){return maxamount;};
    int getFood(double x, double y){return 0;};
    int getTerrain(double x, double y);
    /* See in a direction */
    double see(double x, double y, double d, double len, double fov);
    Color seeColor(double x, double y, double d, double len, double fov);
    /* attack in a direction, return stolen energy */
    int attack(double x, double y, double d, double len, double angle, int maxsteal){return 0;};
    /* Distance to closest creature in a given direction */
    double dist(double x, double y, double d, double len, double fov);
private:
    int m_xbound = 0;
    int m_ybound = 0;
    int m_currid = 0;
    std::mutex m_lock;
    Creature* m_pet;
};

#endif //CREATUREBRAINS_PETWORLD_HPP
