//
// Created by Sumner Hearth on 9/26/16.
//

#ifndef AnimalWorld_hpp
#define AnimalWorld_hpp


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

class AnimalWorld : public World {
public:
    ~AnimalWorld(){destroy();};
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
    int eat(double x, double y, int maxamount);
    int getFood(double x, double y);
    int getTerrain(double x, double y);
    int mapFoodIndex(double x, double y);
    /* See in a direction */
    double see(double x, double y, double d, double len, double fov);
    Color seeColor(double x, double y, double d, double len, double fov);
    /* attack in a direction, return stolen energy */
    int attack(double x, double y, double d, double len, double angle, int maxsteal);
    /* Distance to closest creature in a given direction */
    double dist(double x, double y, double d, double len, double fov);
private:
    int m_time = 0;
    int m_xbound;
    int m_ybound;

    int m_currid = 0;
    int m_currfam = 0;

    /* Will likely store creatures in linked list since will not be
     * looking them up all that often, will be cycling through very
     * often though */
    CreatureNode* m_head = nullptr;
    int m_pop;

    std::mutex m_lock;
    std::mutex m_bufflock;

    Creature* m_recentc = nullptr;
    int m_recentcid = -1;

    int* m_terrain = nullptr;
    FoodSquare* m_foodsquares = nullptr;
    int m_xsquares, m_ysquares;
};


#endif //AnimalWorld_hpp
