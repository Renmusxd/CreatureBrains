//
//  AnimalWorld.cpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/18/15.
//
//

#include <thread>
#include <vector>
#include <iostream>

#include "Utility/TrigUtility.hpp"
#include "Brains/NeuralBrain.hpp"
#include "World/AnimalWorld.hpp"
#include "Creatures/AnimalCreature.hpp"

struct CreatureNode{
    Creature* c = nullptr ;
    CreatureNode* next = nullptr;
};
struct FoodSquare{
    int amount;
    int lastupdate;
    FoodSquare(){amount=FOODMAX; lastupdate=0;}
    FoodSquare(int a, int b){amount=a; lastupdate=b;}
};

void AnimalWorld::init(int xbound, int ybound, bool genterrain){
    m_xbound = xbound; m_ybound = ybound;
    m_currid = 0; m_time = 0;
    m_xsquares = (xbound/FOODSQUARE) + (((xbound % FOODSQUARE)==0)?0:1);
    m_ysquares = (ybound/FOODSQUARE) + (((ybound % FOODSQUARE)==0)?0:1);
    m_foodsquares = new FoodSquare[m_xsquares*m_ysquares];
    m_pop = 0;
    m_terrain = new int[m_xsquares*m_ysquares];
    if (genterrain){
        double xtarget = m_xbound/2.5;
        double ytarget = m_ybound/2.5;
        double width = 0.4;
        std::uniform_real_distribution<double> duni(-0.1,0.1);
        for (double y = 0; y<m_ybound; y+=FOODSQUARE){
            for (double x = 0; x<m_xbound; x+=FOODSQUARE){
                int i = mapFoodIndex(x,y);

                double dx1 = x - m_xbound/2.0;
                double dy1 = y - m_ybound/2.0;
                double eq1 = (dx1*dx1)/(xtarget*xtarget) + (dy1*dy1)/(ytarget*ytarget);

                m_terrain[i] = 1 - (int)(fabs(eq1-1) + width + duni(neuronutility::generator));
                m_terrain[i] += (rand()%10 - 8 > 0)?1:0;
            }
        }

    } else {
        for (int i = 0; i < m_xsquares*m_ysquares; i++){
            m_terrain[i] = 1;
        }
    }
}

void AnimalWorld::destroy(){
    m_lock.lock();
    
    // Free things
    CreatureNode* sel_node = m_head;
    while (sel_node != nullptr){
        Creature* c = sel_node->c;
        CreatureNode* n = sel_node;
        sel_node = sel_node->next;
        delete c;
        delete n;
    }
    m_head = nullptr;
    
    m_lock.unlock();
}

/* Gets ownership of c, may delete when dead */
void AnimalWorld::addCreature(Creature* c){
    CreatureNode* cn = new CreatureNode;
    cn->c = c;
    cn->next = m_head;
    m_head = cn;

    m_pop += 1;
}
int AnimalWorld::addCreature(Brain* b, double x, double y, double dir){
    Creature* c = new AnimalCreature();
    c->init(b, x, y, dir, m_currid, m_currfam++, 0);
    this->addCreature(c);
    return m_currid++;
}
int AnimalWorld::addCreature(Brain* b, double x, double y, double dir, int fam, int gen){
    Creature* c = new AnimalCreature();
    c->init(b, x, y, dir, m_currid, fam, gen);
    this->addCreature(c);
    return m_currid++;
}


int AnimalWorld::eat(double x, double y, int maxamount){
    int foodindx = mapFoodIndex(x,y);
    if (foodindx==-1){return 0;}
    if (getTerrain(x,y)<0){return 0;}
    int lastup = m_foodsquares[foodindx].lastupdate;
    int tot = FOODREGEN*(m_time - lastup) + m_foodsquares[foodindx].amount;
    if (tot>FOODMAX){tot = FOODMAX;}
    int change = maxamount;
    if (tot < maxamount){change = tot;}
    m_foodsquares[foodindx].amount = tot - change;
    m_foodsquares[foodindx].lastupdate = m_time;
    return change;
}

Color AnimalWorld::seeColor(double x, double y, double d, double len, double fov){
    double s = fabs(see(x,y,d,len,fov));
    return Color(s,s,s);
}

double AnimalWorld::see(double x, double y, double d, double len, double fov){
    TrigUtility* tu = TrigUtility::getInstance();
    double cos = tu->mcos(d);
    double sin = tu->msin(d);
    double fovcos = tu->mcos(fov); // cosine of max angular distance

    double mind2 = len*len + 1;
    Creature* seenc = nullptr;
    CreatureNode* sel_node = m_head;
    while (sel_node != nullptr){
        Creature* sel_c = sel_node->c;
        double dx = x - sel_c->x();
        double dy = y - sel_c->y();
        double d2 = dx*dx + dy*dy;
        if (d2 <= mind2){
            // Within sight range, check
            // cos(phi) = (a.b)/(norm(a)norm(b))
            double cos_d_angle = ((dx*cos)+(dy*sin))/sqrt(d2);
            if (cos_d_angle >= fovcos){
                seenc = sel_c;
                mind2 = d2;
            }
        }
        sel_node = sel_node->next;
    }
    // If found a creature then report appearance, else ground
    if (seenc != nullptr){return seenc->app();}
    else {return getFood(x+(len*cos),y+(len*sin))/(double)FOODMAX;}
}

int AnimalWorld::attack(double x, double y, double d, double len, double angle, int maxsteal){
    TrigUtility* tu = TrigUtility::getInstance();
    double cos = tu->mcos(d);
    double sin = tu->msin(d);
    double fovcos = tu->mcos(angle); // cosine of max angular distance

    double mind2 = len*len + 1;
    Creature* attackedc = nullptr;
    CreatureNode* sel_node = m_head;
    while (sel_node != nullptr){
        Creature* sel_c = sel_node->c;
        double dx = x - sel_c->x();
        double dy = y - sel_c->y();
        double d2 = dx*dx + dy*dy;
        if (d2 <= mind2){
            // Within sight range, check
            // cos(phi) = (a.b)/(norm(a)norm(b))
            double cos_d_angle = ((dx*cos)+(dy*sin))/sqrt(d2);
            if (cos_d_angle >= fovcos){
                attackedc = sel_c;
                mind2 = d2;
            }
        }
        sel_node = sel_node->next;
    }
    // If found a creature then report appearance, else ground
    if (attackedc != nullptr){
        int energy = attackedc->energy();
        int change = (energy<maxsteal)?energy:maxsteal;
        attackedc->attack(change);
        return change;
    } else {
        return 0;
    }
}

int AnimalWorld::getFood(double x, double y){
    if (getTerrain(x,y)<0){return -FOODMAX;}
    int foodindx = mapFoodIndex(x,y);
    if (foodindx == -1) { return -FOODMAX; }
    int lastup = m_foodsquares[foodindx].lastupdate;
    int tot = FOODREGEN * (m_time - lastup) + m_foodsquares[foodindx].amount;
    if (tot > FOODMAX) { tot = FOODMAX; }
    m_foodsquares[foodindx].amount = tot;
    m_foodsquares[foodindx].lastupdate = m_time;
    return m_foodsquares[foodindx].amount;
}

int AnimalWorld::getTerrain(double x, double y){
    if (x>=0 && x<m_xbound && y>=0 && y<m_ybound){
        if (m_terrain[mapFoodIndex(x,y)]){
            return 0;
        }
    }
    return -1;
}

void AnimalWorld::update(){
    m_lock.lock();
    
    CreatureNode* sel_node = m_head;
    if (sel_node != nullptr){
        /* MAKE THEM THINK */
        while (sel_node != nullptr){
                sel_node->c->think(this);
            sel_node = sel_node->next;
        }

        /* MAKE THEM ACT */
        sel_node = m_head;
        while (sel_node != nullptr){
            sel_node->c->update(this);
            sel_node = sel_node->next;
        }

        /* BRING OUT YOUR DEAD */
        while (m_head != nullptr && m_head->c->dead()){
            if (m_recentcid == m_head->c->id()){
                m_recentcid = -1;
            }
            CreatureNode* temp = m_head;
            m_head = m_head->next;
            delete temp->c;
            delete temp;
            m_pop -= 1;
        }
        if (m_head != nullptr) {
            CreatureNode *last_node = m_head;
            sel_node = m_head->next;
            while (sel_node != nullptr) {
                if (sel_node->c->dead()){
                    if (m_recentcid == sel_node->c->id()){
                        m_recentcid = -1;
                    }
                    CreatureNode* temp = sel_node;
                    last_node->next = sel_node->next;
                    sel_node = sel_node->next;
                    delete temp->c;
                    delete temp;
                    m_pop -= 1;
                } else {
                    last_node = sel_node;
                    sel_node = sel_node->next;
                }
            }
        }
    }
    m_time++;

    // Repopulate
    while (m_pop < MINPOP){
        std::uniform_int_distribution<int> xuni(0,m_xbound);
        std::uniform_int_distribution<int> yuni(0,m_ybound);
        std::uniform_real_distribution<double> duni(0,TWOPI);

        Brain* b = new NeuralBrain();
        double x = xuni(neuronutility::generator);
        double y = yuni(neuronutility::generator);
        double d = duni(neuronutility::generator);

        addCreature(b,x,y,d);
    }

    m_lock.unlock();

}

std::string AnimalWorld::getState(){

    m_lock.lock();
    std::string format = "{\"width\":%d, \"height\":%d, \"creatures\":%s}";
    std::string cformat = "{\"id\":%d, \"fam\":%d, \"app\":%f, \"x\":%f, \"y\":%f, \"d\":%f}";
    std::string cr_string = "[";
    char crbuff[128];
    CreatureNode* sel = m_head;
    if (sel!=nullptr){
        snprintf(crbuff, sizeof(crbuff),
                 cformat.c_str(),
                 sel->c->id(),sel->c->fam(), sel->c->app(),
                 sel->c->x(), sel->c->y(),sel->c->d());
        cr_string += std::string(crbuff);
        sel = sel->next;
    }
    while (sel!=nullptr){
        snprintf(crbuff, sizeof(crbuff),
                 (","+cformat).c_str(),
                 sel->c->id(),sel->c->fam(), sel->c->app(),
                 sel->c->x(), sel->c->y(),sel->c->d());
        cr_string += std::string(crbuff);
        sel = sel->next;
    }
    cr_string += "]";
    char buff[cr_string.length() + format.length() + 4];
    snprintf(buff, sizeof(buff), format.c_str(), m_xbound, m_ybound, cr_string.c_str());
    m_lock.unlock();

    return std::string(buff);
}

std::string AnimalWorld::getTerrain(){
    std::string format = "{\"max\":%d, \"nx\":%d, \"ny\":%d, \"food\":%s}";

    char crbuff[128];
    std::string cr_string = "[";

    m_lock.lock();
    for (int i = 0; i<m_xsquares*m_ysquares; i++){
        if (m_terrain[i]>0) {
            FoodSquare fq = m_foodsquares[i];
            fq.amount = fq.amount + FOODREGEN * (m_time - fq.lastupdate);
            if (fq.amount > FOODMAX) { fq.amount = FOODMAX; }
            fq.lastupdate = m_time;

            snprintf(crbuff, sizeof(crbuff), "%d,", fq.amount);
        } else {
            snprintf(crbuff, sizeof(crbuff), "%d,", -1);
        }
        cr_string += std::string(crbuff);
    }
    m_lock.unlock();
    cr_string[cr_string.length()-1] = ']';

    char buff[cr_string.length() + format.length() + 4];
    snprintf(buff, sizeof(buff), format.c_str(), FOODMAX,
             m_xsquares, m_ysquares, cr_string.c_str());
    return std::string(buff);
}

Creature* AnimalWorld::getCreatureByID(int id){
    Creature* selc;

    m_lock.lock();
    if (m_recentcid==id){
        selc = m_recentc;
    } else {
        CreatureNode* sel = m_head;
        while (sel!=nullptr && sel->c->id() != id){
            sel = sel->next;
        }
        if (sel==nullptr){
            selc = nullptr;
        } else {
            m_recentcid = id;
            m_recentc = sel->c;
            selc = sel->c;
        }
    }
    m_lock.unlock();

    return selc;
}

int AnimalWorld::mapFoodIndex(double x, double y){
    if (x<0 || x>m_xbound || y<0 || y>m_ybound){
        return -1;
    }
    return (m_ysquares*(int)(y/(double)FOODSQUARE)) + (int)(x/(double)FOODSQUARE);
}

double AnimalWorld::dist(double x, double y, double d, double len, double fov){
    TrigUtility* tu = TrigUtility::getInstance();
    double cos = tu->mcos(d);
    double sin = tu->msin(d);
    double fovcos = tu->mcos(fov); // cosine of max angular distance

    double mind2 = len*len + 1;
    Creature* seenc = nullptr;
    CreatureNode* sel_node = m_head;
    while (sel_node != nullptr){
        Creature* sel_c = sel_node->c;
        double dx = x - sel_c->x();
        double dy = y - sel_c->y();
        double d2 = dx*dx + dy*dy;
        if (d2 <= mind2){
            // Within sight range, check
            // cos(phi) = (a.b)/(norm(a)norm(b))
            double cos_d_angle = ((dx*cos)+(dy*sin))/sqrt(d2);
            if (cos_d_angle >= fovcos){
                seenc = sel_c;
                mind2 = d2;
            }
        }
        sel_node = sel_node->next;
    }
    // If found a creature then report appearance, else ground
    if (seenc != nullptr){
        return sqrt(mind2);
    } else {
        return 0;
    }
}