//
//  main.cpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/18/15.
//
//

#include "Server/WorldServer.hpp"

int main(int argc, const char * argv[]) {
    WorldServer* ws = new WorldServer;
    // Read port from args
    ws->init(1708);
    // Maybe add a world?
    ws->newWorld(1500, 1500);
    ws->run();
    delete ws;
}
