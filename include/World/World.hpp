//
//  World.hpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/18/15.
//
//

#ifndef World_hpp
#define World_hpp

#include <stdio.h>
#include <mutex>
#include <string>

#include "Brains/Brain.hpp"
#include "Creatures/Creature.hpp"

#define MINPOP 50

#define FOODSQUARE 25
#define FOODMAX 512
#define FOODREGEN 1

struct CreatureNode;
struct FoodSquare;
class Creature;

struct Color{
    double r, g, b;
    Color(double red, double green, double blue){
        r = red; g = green; b = blue;
    }
};

class World{
public:
    virtual ~World(){};
    virtual void init(int xbound, int ybound, bool genterrain) = 0;
    virtual void destroy() = 0;
    /* Can multithread think, then consectutive update */
    virtual void update() = 0;
    /* Adds creature to world, world is now owner */
    virtual void addCreature(Creature* c) = 0;
    virtual int addCreature(Brain* b, double x, double y, double dir) = 0;
    virtual int addCreature(Brain* b, double x, double y, double dir, int fam, int gen) = 0;
    /* Makes string describing world state */
    virtual std::string getState() = 0;
    virtual std::string getTerrain() = 0;
    /* Gets creature by id number, caches results */
    virtual Creature* getCreatureByID(int id) = 0;
    virtual int eat(double x, double y, int maxamount) = 0;
    virtual int getFood(double x, double y) = 0;
    virtual int getTerrain(double x, double y) = 0;
    /* See in a direction */
    virtual double see(double x, double y, double d, double len, double fov) = 0;
    virtual Color seeColor(double x, double y, double d, double len, double fov) = 0;
    /* attack in a direction, return stolen energy */
    virtual int attack(double x, double y, double d, double len, double angle, int maxsteal) = 0;
    /* get distance of closest obstacle in direction */
    virtual double dist(double x, double y, double d, double len, double fov) = 0;
};

#endif /* World_hpp */
