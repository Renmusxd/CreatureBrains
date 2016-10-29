//
// Created by Sumner Hearth on 9/26/16.
//

#include <cmath>
#include "World/PetWorld.hpp"
#include "Creatures/PetCreature.hpp"

void PetWorld::init(int xbound, int ybound, bool _){
    m_xbound = xbound; m_ybound = ybound;

}

void PetWorld::destroy(){
    if (m_pet != nullptr){
        delete m_pet;
        m_pet = nullptr;
    }
}

void PetWorld::update(){
    m_lock.lock();
    if (m_pet != nullptr) {
        m_pet->think(this);
        m_pet->update(this);
    }
    m_lock.unlock();
}

/* Adds creature to world, world is now owner */
void PetWorld::addCreature(Creature* c){
    if (m_pet != nullptr){
        delete m_pet;
    }
    m_pet = c;
}

int PetWorld::addCreature(Brain* b, double x, double y, double dir){
    return addCreature(b,x,y,dir,m_currid,0);
}

int PetWorld::addCreature(Brain* b, double x, double y, double dir, int fam, int gen){
    Creature* c = new PetCreature();
    c->init(b, x, y, dir, m_currid, fam, gen);
    addCreature(c);
    return m_currid++;
}

/* 0+ for types of open space, -1 for obstacle */
int PetWorld::getTerrain(double x, double y){

}

/* See in a direction */
double PetWorld::see(double x, double y, double d, double len, double fov){
    Color c = seeColor(x,y,d,len,fov);
    return sqrt((c.r*c.r)+(c.g*c.g)+(c.b*c.b));
}

/* See in a direction */
Color PetWorld::seeColor(double x, double y, double d, double len, double fov){
    return Color(0,0,0);
}

/* Distance to closest creature in a given direction */
double PetWorld::dist(double x, double y, double d, double len, double fov){

}

/* Gets creature by id number */
Creature* PetWorld::getCreatureByID(int id){
    if (m_pet->id()==id){return m_pet;}
    return nullptr;
}

/* Makes string describing world state */
std::string PetWorld::getState(){

}

std::string PetWorld::getTerrain(){

}