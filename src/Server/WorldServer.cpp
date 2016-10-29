//
//  WorldServer.cpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/18/15.
//
//
#include <thread>
#include <chrono>
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <string>
#include <sstream>
#include <iostream>

#include "Server/WorldServer.hpp"
#include "Brains/NeuralBrain.hpp"

/* A class to control the world and handle client commands
 * commands should include:
 * NEW <X> <Y> - new world of size X, Y
 * CRT <Brain> <X> <Y> <D> - new creature with brain type at (X,Y,D), reponds id
 * GET - responds with creature information
 * GTC <ID> - reponds with detailed information about the creature ID
 */
void WorldServer::init(int port){
    struct sockaddr_in serv_addr;
    int yes = 1;
    sockfd = 0;
    //sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0){ perror("ERROR opening socket"); exit(1);}
    // Clear address area
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (ret < 0){perror("ERROR on setsockopt"); exit(1);}
    ret = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (ret < 0){perror("ERROR on binding"); exit(1);}
    ret = listen(sockfd,5);
    if (ret < 0){perror("ERROR on listen"); exit(1);}
    
    up = true;
}

/* Reads until delimiter or buffsize */
int readToEnd(int sockfd,char* buff, int buffsize, char delim){
    int bytesRead = 0;
    ssize_t result;
    while (bytesRead < buffsize){
        result = recv(sockfd, buff + bytesRead, (size_t)(buffsize - bytesRead), 0);
        if (result < 1 ){
            return 0;
        }
        for (int i = bytesRead; i<bytesRead+result; i++){
            if (buff[i]==delim){
                // Clear back of array
                // ERRORs will likely come from here...
                // memset(&buff[i+1], 0, (buffsize-bytesRead) - (i));
                // Return amount read
                return bytesRead + 1 + i;
            }
        }
        bytesRead += result;
    }
    return bytesRead;
}

void WorldServer::destroy(){
    printf("Trying to get lock to delete\n");
    m_lock.lock();
    
    if (m_world != nullptr){
        delete m_world;
    }
    if (up){
        close(sockfd);
        up = false;
    }
    
    m_lock.unlock();
}

void WorldServer::newWorld(int xbound, int ybound){
    m_lock.lock();
    if (m_world != nullptr){
        delete m_world;
    }
    m_world = new AnimalWorld;
    m_world->init(xbound,ybound,true);
    
    m_lock.unlock();
}

void WorldServer::run(){
    running = true;
    
    std::thread t1 = std::thread([this]{
        printf("[*] Starting world updater\n");
        this->updateWorld();
    });
    t1.detach();

    handleLoop();
    printf("[*] Closing simulation...\n");
}

void WorldServer::handleLoop(){
    while (running){
        struct sockaddr_in cli_addr;
        int clilen = sizeof(cli_addr);
        int newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, (unsigned int*) &clilen);
        if (newsockfd > 0){
            std::thread t = std::thread([this, newsockfd](){
                this->handleClient(newsockfd);
            });
            t.detach();
        } else {
            printf("[*] Socket closing... %i\n",newsockfd);
        }
    }
    printf("[*] Stopping server\n");
}

void WorldServer::handleClient(int clifd){
    printf("[%i] Connected to client\n",clifd);
    // Check running regularly
    bool connected = true;
    size_t bufflen = 256;
    int sbufflen = 256;
    char buff[bufflen];
    char sbuff[sbufflen];
    std::string mstr = std::string("Hello!\n");
    send(clifd,&mstr,mstr.length()+1,0);

    while (running && connected){
        memset(buff, 0, bufflen);
        std::stringstream ss;
        int bytesread = readToEnd(clifd, buff, (int) bufflen, '\n');
        if (bytesread==0){connected = false;}
        ss << buff;
        std::string cmd;
        ss >> cmd;

        if (strncmp(cmd.c_str(), "STATE", 6)==0){
            // Respond with state
            m_lock.lock();
            mstr = m_world->getState() + "\n";
            m_lock.unlock();
        }
        else if (strncmp(cmd.c_str(), "FOOD", 5)==0){
            m_lock.lock();
            mstr = m_world->getTerrain() + "\n";
            m_lock.unlock();
        }
        else if (strncmp(cmd.c_str(), "CRINFO", 7)==0){
            // Respond with info about critter
            if (bytesread >= 9){
                int id = 0;
                ss >> id;
                // TODO get critter info
                memset(sbuff,0,sizeof(sbuff));
                
                m_lock.lock();
                Creature* c = m_world->getCreatureByID(id);
                if (c==nullptr){
                    mstr = "No such Creature\n";
                } else {
                    mstr = c->getState();
                }
                m_lock.unlock();
            } else {
                mstr = std::string("CINFO i => i is an integer\n");
            }
        }
        else if (strncmp(cmd.c_str(), "ADDCR", 6)==0){
            // Respond with critter id
            std::string braintype;
            double xpos, ypos, dir;
            ss >> braintype >> xpos >> ypos >> dir;
            Brain* b = nullptr;
            if (strncmp(braintype.c_str(),"NNB",4)==0){
                printf("[%i] Adding neural brain\n",clifd);
                b = new NeuralBrain();
            } else if (strncmp(braintype.c_str(),"NPU",4)==0){
                printf("[%i] Adding NPU\n",clifd);
                //b = new NPU();
            } else {
                printf("[%i] Cannot make brain of type %s\n",clifd, braintype.c_str());
            }
            memset(sbuff,0,sizeof(sbuff));
            if (b != nullptr){
                
                m_lock.lock();
                int id = m_world->addCreature(b, xpos, ypos, dir);
                m_lock.unlock();
                
                snprintf(sbuff, sizeof(sbuff), "{\"id\":%i}\n", id);
                mstr = std::string(sbuff);
                printf("[%i] Adding creature %i with: %s %f %f %f\n",clifd, id,braintype.c_str(),xpos,ypos,dir);
            } else {
                printf("[%i] Failed to add creature\n",clifd);
                mstr = std::string("Usage: ADDCR b x y d\n");
            }
        }
        else if (strncmp(cmd.c_str(), "SETRATE", 8)==0){
            // Respond with true or false
            double rate = 0;
            ss >> rate;
            printf("[%i] Setting rate to %f\n",clifd, rate);
            
            if (rate==0){
                paused = true;
                mstr = std::string("{\"rate\":0}\n");
                m_sleep = 0.1;
            } else {
                paused = false;
                m_sleep = 1/rate;
                snprintf(sbuff, sizeof(sbuff), "{\"rate\":%f}\n", rate);
                mstr = std::string(sbuff);
            }
        }
        else if (strncmp(cmd.c_str(), "GETRATE", 8)==0){
            if (paused || m_sleep==0){
                mstr = std::string("{\"rate\":0}\n");
            } else {
                double rate = 1.0/m_sleep;
                snprintf(sbuff, sizeof(sbuff), "{\"rate\":%f}\n", rate);
                mstr = std::string(sbuff);
            }
        }
        else if (strncmp(cmd.c_str(), "EXIT", 6)==0){
            mstr = std::string("Goodbye!\n");
            connected = false;
        }
        else if (strncmp(cmd.c_str(), "SHUTDOWN", 9)==0){
            mstr = std::string("Shutting down...\n");
            printf("[%i] Shutting down\n",clifd);
            connected = false;
            running = false;
            shutdown(sockfd,0);
            close(sockfd);
        }
        else {
            memset(sbuff,0,sizeof(sbuff));
            snprintf(sbuff, sizeof(sbuff), "Invalid Command: %s\n", cmd.c_str());
            mstr = std::string(sbuff);
        }
        send(clifd,mstr.c_str(),mstr.length()+1,0);
    }
    printf("[%i] Closing connection...\n",clifd);
    close(clifd);
}

void WorldServer::updateWorld(){
    while (running){
        std::this_thread::sleep_for(std::chrono::milliseconds((long)(m_sleep*1000)));

        m_lock.lock();
        if (!paused && (m_world != nullptr)){
            m_world->update();
        }
        m_lock.unlock();
    }
    printf("[*] Stopping world updater\n");
}