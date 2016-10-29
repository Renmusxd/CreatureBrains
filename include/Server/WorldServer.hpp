//
//  WorldServer.hpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/18/15.
//
//

#ifndef WorldServer_hpp
#define WorldServer_hpp

#include <stdio.h>
#include <mutex>
#include <sys/socket.h>

#include "World/AnimalWorld.hpp"

class WorldServer {
public:
    ~WorldServer(){destroy();};
    void init(int port);
    void destroy();
    /* Run starts thread to handle server requests and runs simulations */
    void run();
    
    void newWorld(int xbound, int ybound);
    void handleLoop();
    void updateWorld();
private:
    World* m_world = nullptr;
    std::mutex m_lock;
    bool up;
    bool running;
    int sockfd;
    
    double m_sleep =0.01667;
    bool paused = false;
    
    void handleClient(int clifd);
};

#endif /* WorldServer_hpp */
