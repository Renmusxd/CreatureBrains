//
// Created by Sumner Hearth on 9/26/16.
//

#include <cmath>
#include <sstream>
#include "Creatures/PetCreature.hpp"
#include "Utility/TrigUtility.hpp"

#define TOTALFOV (TWOPI/4.0)
#define VERTFOV (TOTALFOV/2.0)
#define HORZFOV (TOTALFOV)
#define SIGHTROWS 3
#define SIGHTCOLUMNS 3
#define NUMDISTSENSORS 1
#define NUMINTERNALSTATES 1

#define NUMACTIONS 2
#define STATE_LEN 5
#define THRESHOLD 0.5
#define WHEELMAX 1.0
#define TURN_SPEED 0.1
#define MOVE_SPEED 0.1

#define BOREDOM -0.01

void PetCreature::init(Brain* brain, double x, double y, double dir, int id, int fam, int gen){
    m_xpos = x; m_ypos = y; m_dir = dir;
    m_id = id; m_fam = fam; m_gen = gen;

    m_senseslen = NUMINTERNALSTATES +(3*SIGHTROWS*SIGHTCOLUMNS) + NUMDISTSENSORS;
    m_actionslen = NUMACTIONS + STATE_LEN;

    m_brain = brain;
    BrainIO bio = m_brain->init(m_senseslen, m_actionslen, STATE_LEN, false);
    m_senses = bio.input_array;
    m_actions = bio.output_array;
    memset(m_senses,0,m_senseslen*sizeof(double));
}

void PetCreature::destroy(){
    if (m_brain != nullptr){
        delete m_brain;
        m_brain = nullptr;
    }
}

/* Update the brain */
void PetCreature::think(World* w){
    PetCreatureSensors sensors(this,w);

    int indx = 0;
    m_senses[indx++] = sensors.getEnergy();
    m_senses[indx++] = sensors.getDistance(0);
    double v_angle = VERTFOV/SIGHTROWS;
    double h_angle = HORZFOV/SIGHTCOLUMNS;
    for (int r = 0; r<SIGHTROWS; r++){
        double vstart = (v_angle*r) - (VERTFOV/2.0);
        double vend = (v_angle*(r+1)) - (VERTFOV/2.0);
        double vcent = (vstart + vend)/2.0;
        for (int c = 0; c<SIGHTCOLUMNS; c++){
            double hstart = (h_angle*c) - (HORZFOV/2.0);
            double hend = (h_angle*(c+1)) - (HORZFOV/2.0);
            double hcent = (hstart + hend)/2.0;

            Color col = sensors.getSight(hcent,vcent,h_angle,v_angle);
            m_senses[indx++] = col.r;
            m_senses[indx++] = col.g;
            m_senses[indx++] = col.b;
        }
    }

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

/* Update position and physical details */
void PetCreature::update(World* w){
    // TODO get actions from m_actions
    PetCreatureActions pca = PetCreatureActions(this,w);
    PetCreatureSensors pcs = PetCreatureSensors(this,w);

    pca.move(m_actions[0]);
    pca.turn(m_actions[1]);

    pca.commit();
    reward(pcs.getReward());
}

void PetCreature::reward(double amount){

}

/* Gets input and output state */
std::string PetCreature::getState(){
    // TODO describe
    std::string senseString = inputString();
    std::string actionString = outputString();

    char sbuff[1028 + senseString.length() + actionString.length()];
    std::string format = "{\"id\":%i, \"fam\":%d, \"gen\":%d, \"app\":%f, \"x\":%f, \"y\":%f, \"d\":%f, "
            "\"braintype\":\"%s\", \"energy\":%i, \"health\":%i, "
            "\"inputs\":%s, \"outputs\":%s}\n";


}

std::string PetCreature::inputString(){
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

std::string PetCreature::outputString(){
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

std::string PetCreature::getBrain(){
    return m_brain->details();
}

/*
 * Some functions to facilitate sensor readings and machine output
 */

Color PetCreatureSensors::getSight(double theta, double phi, double tfov, double pfov){
    TrigUtility* tu = TrigUtility::getInstance();
    double sin_phi = tu->msin(phi);

    double dist;
    if (sin_phi == 0){
        dist = m_camerarange;
    } else {
        double cos_phi = tu->mcos(phi);
        dist = m_cameraheight * (cos_phi/sin_phi);
    }

    return m_world->seeColor(m_creature->x(),m_creature->y(),m_creature->d()+theta,dist,tfov);
}

double PetCreatureSensors::getDistance(double theta){
    return m_world->dist(m_creature->x(),m_creature->y(),
                  m_creature->d()+theta, m_distrange,0)/m_distrange;
}

double PetCreatureSensors::getReward(){
    double base = BOREDOM;


    return base;
}

void PetCreatureActions::commit() {
    if (m_leftwheel!=0 || m_rightwheel!=0) {
        if (m_leftwheel>WHEELMAX){m_leftwheel = WHEELMAX;}
        else if (m_leftwheel < -WHEELMAX){m_leftwheel = -WHEELMAX;}
        if (m_rightwheel>WHEELMAX){m_rightwheel = WHEELMAX;}
        else if (m_rightwheel < -WHEELMAX){m_rightwheel = -WHEELMAX;}

        double move = MOVE_SPEED*(m_leftwheel + m_rightwheel)/(2*WHEELMAX);
        double turn = TURN_SPEED*(m_rightwheel - m_leftwheel)/(2*WHEELMAX);

        double halfturn = m_creature->m_dir + turn/2.0;
        m_creature->m_xpos += move*TrigUtility::getInstance()->mcos(halfturn);
        m_creature->m_ypos += move*TrigUtility::getInstance()->msin(halfturn);
        m_creature->m_dir += turn;
    }
}
