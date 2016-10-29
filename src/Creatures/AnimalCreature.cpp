//
//  AnimalCreature.cpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/18/15.
//
//
#include <string>
#include <sstream>
#include <iomanip>
#include "Utility/NeuralUtility.hpp"
#include "Creatures/AnimalCreature.hpp"
#include "Utility/TrigUtility.hpp"

#define THRESHOLD 0.5
#define AnimalCreature_SPEED 0.5
#define AnimalCreature_TURN 0.1

#define NUMSENSES 10
#define NUMACTIONS 7
#define STATE_LEN 5

#define EATAMOUNT 15
#define ENERGYDECAY 5
#define SWIMDECAY 50
#define RESTLIFE 1800
#define MAX_ENERGY (RESTLIFE*ENERGYDECAY)
#define MAX_HEALTH 100
#define MUTATIONRATE 0.02
#define ATTACKRANGE 15
#define ATTACKANGLE TWOPI/8.0
#define ATTACKSTEAL 1000.0
#define SIGHT 50
#define AnimalCreatureSIGHT 100
#define VIEWANGLE TWOPI/8.0
#define FOVANGLE TWOPI/16.0
// 2 degrees

#define SPRINTDECAY 2.0
#define SPRINTSPEED 1.5


void AnimalCreature::init(Brain* b, double x, double y, double dir,
                    int id, int fam, int gen){
    m_xpos = x; m_ypos = y; m_dir = dir;
    m_id = id; m_fam = fam; m_gen = gen;
    
    m_senseslen = NUMSENSES; // Senses for status, senses for sight
    m_actionslen = NUMACTIONS + STATE_LEN;
    
    m_brain = b;
    BrainIO bio = m_brain->init(m_senseslen, m_actionslen, STATE_LEN, false);
    m_senses = bio.input_array;
    m_actions = bio.output_array;
    memset(m_senses,0,m_senseslen*sizeof(double));

    m_energy = MAX_ENERGY/4;
    m_health = MAX_HEALTH;
}

void AnimalCreature::destroy(){
    // Don't free arrays
    if (m_brain != nullptr){
        delete m_brain;
        m_brain = nullptr;
    }
}

void AnimalCreature::think(World* w){
    // TODO Analyze surroundings, update senses
    TrigUtility* tu = TrigUtility::getInstance();

    double lsightx = SIGHT * tu->mcos(m_dir - VIEWANGLE);
    double lsighty = SIGHT * tu->msin(m_dir - VIEWANGLE);

    double csightx = SIGHT * tu->mcos(m_dir);
    double csighty = SIGHT * tu->msin(m_dir);

    double rsightx = SIGHT * tu->mcos(m_dir + VIEWANGLE);
    double rsighty = SIGHT * tu->msin(m_dir + VIEWANGLE);

    bool nonzero = false;

    for (int i = 0; i<m_senseslen; i++){
        nonzero = nonzero || (i>5 && m_senses[i]!=0.0);
    }

    m_senses[0] = m_health/(double)MAX_HEALTH;
    m_senses[1] = m_energy/(double)MAX_ENERGY;
    m_senses[2] = w->getFood(m_xpos,m_ypos)/(double)FOODMAX;
    m_senses[3] = w->getFood(m_xpos + lsightx, m_ypos + lsighty)/(double)FOODMAX;
    m_senses[4] = w->getFood(m_xpos + csightx, m_ypos + csighty)/(double)FOODMAX;
    m_senses[5] = w->getFood(m_xpos + rsightx, m_ypos + rsighty)/(double)FOODMAX;
    m_senses[6] = w->see(m_xpos,m_ypos,m_dir-VIEWANGLE,SIGHT,FOVANGLE);
    m_senses[7] = w->see(m_xpos,m_ypos,m_dir,SIGHT,FOVANGLE);
    m_senses[8] = w->see(m_xpos,m_ypos,m_dir+VIEWANGLE,SIGHT,FOVANGLE);
    for (int i = 0; i<m_senseslen; i++){
        if (isnan(m_senses[i])){
            m_senses[i]=0;
        }
    }
    m_brain->update();
    for (int i = 0; i<m_actionslen; i++){
        if (isnan(m_actions[i])){
            m_actions[i]=0;
        }
    }
}

void AnimalCreature::update(World* w){
    if (m_health<MAX_HEALTH)m_health+=1;
    if (w->getTerrain(m_xpos,m_ypos)<0){
        m_energy -= SWIMDECAY * ((m_sprinting)?SPRINTDECAY:1);
    } else {
        m_energy -= ENERGYDECAY * ((m_sprinting)?SPRINTDECAY:1);
    }

    // Will read from actions
    double dir_mov = m_actions[0]; // forward/backward
    double dir_trn = m_actions[1]; // turn
    double act_eat = m_actions[2]; // eat
    double act_atk = m_actions[3]; // attack
    double act_mte = m_actions[4]; // reproduce
    double act_srt = m_actions[5]; // sprint
    double act_lok = m_actions[6]; // appearance to others
    // Perform actions

    // Eat
    if (act_eat > THRESHOLD){
        int change = EATAMOUNT;
        if (MAX_ENERGY - m_energy < EATAMOUNT){
            change = MAX_ENERGY - m_energy;
            if (change < 0){
                change = 0;
                m_energy = MAX_ENERGY;
            }
        }
        int ate = w->eat(m_xpos,m_ypos,change);
        m_energy += ate;
    }
    // Attack
    if (act_atk > THRESHOLD){
        w->attack(m_xpos,m_ypos,m_dir,ATTACKRANGE,ATTACKANGLE,ATTACKSTEAL);
    }

    // Mate (non-sexual for now)
    if (act_mte > THRESHOLD){
        if (m_energy > 3*MAX_ENERGY/4){
            m_energy -= MAX_ENERGY/2;
            Brain* newbrain = m_brain->mutateCopy(MUTATIONRATE);
            std::uniform_int_distribution<int> uni(-10,10);
            double x = m_xpos + uni(neuronutility::generator);
            double y = m_ypos + uni(neuronutility::generator);
            int id = w->addCreature(newbrain,
                           x, y,
                           m_dir + uni(neuronutility::generator),
                           m_fam, m_gen + 1);
        }
    }
    // Sprinting
    m_sprinting = (act_srt > THRESHOLD);
    m_appearance = act_lok;

    // Turn
    if (dir_trn > THRESHOLD){
        m_dir += AnimalCreature_TURN;
        m_dir = fmod(m_dir,TWOPI);
    } else if (dir_trn < -THRESHOLD){
        m_dir -= AnimalCreature_TURN;
        m_dir = fmod(m_dir+TWOPI,TWOPI);
    }
    // Move
    if (dir_mov > THRESHOLD){
        // Move forwards
        m_xpos += ((m_sprinting)?SPRINTSPEED:1) * AnimalCreature_SPEED*TrigUtility::getInstance()->mcos(m_dir);
        m_ypos += ((m_sprinting)?SPRINTSPEED:1) * AnimalCreature_SPEED*TrigUtility::getInstance()->msin(m_dir);
    } else if (dir_mov < -THRESHOLD){
        // Move backwards
        m_xpos -= ((m_sprinting)?SPRINTSPEED:1) * AnimalCreature_SPEED*TrigUtility::getInstance()->mcos(m_dir);
        m_ypos -= ((m_sprinting)?SPRINTSPEED:1) * AnimalCreature_SPEED*TrigUtility::getInstance()->msin(m_dir);
    }
}

std::string AnimalCreature::inputString(){
    std::ostringstream strs;
    strs << "[";
    if (m_senseslen >= 1) {
        strs << m_senses[0];
        if (m_senses[0] - (int)m_senses[0] == 0) {
            strs << ".0";
        }
    }
    for (int i = 1; i<m_senseslen; i++) {
        strs << ", " << m_senses[i];
        if (m_senses[i] - (int)m_senses[i] == 0) {
            strs << ".0";
        }
    }
    strs << "]";
    return strs.str();
}

std::string AnimalCreature::outputString(){
    std::ostringstream strs;
    strs << "[";
    if (m_actionslen >= 1) {
        strs << m_actions[0];
        if (m_actions[0] - (int)m_actions[0] == 0 ){
            strs << ".0";
        }
    }
    for (int i = 1; i<m_actionslen; i++) {
        strs << ", " << m_actions[i];
        if (m_actions[i] - (int)m_actions[i] == 0){
            strs << ".0";
        }
    }
    strs << "]";
    return strs.str();
}

std::string AnimalCreature::getState(){
    std::string senseString = inputString();
    std::string actionString = outputString();

    char sbuff[1028 + senseString.length() + actionString.length()];
    std::string format = "{\"id\":%i, \"fam\":%d, \"gen\":%d, \"app\":%f, \"x\":%f, \"y\":%f, \"d\":%f, "
            "\"braintype\":\"%s\", \"energy\":%i, \"health\":%i, "
            "\"inputs\":%s, \"outputs\":%s}\n";
    memset(sbuff,0,sizeof(sbuff));
    snprintf(sbuff, sizeof(sbuff), format.c_str(),
             m_id, m_fam, m_gen, m_appearance,
             m_xpos, m_ypos, m_dir,
             m_brain->type().c_str(), m_energy, m_health,
             inputString().c_str(), outputString().c_str());
    return std::string(sbuff);
}

std::string AnimalCreature::getBrain(){
    return m_brain->details();
}