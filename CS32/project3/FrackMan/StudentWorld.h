#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "Actor.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir)
    : GameWorld(assetDir)
    {}
    
    virtual int init();
    
    virtual int move();
    
		  // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		  // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    //	decLives();
    //	return GWSTATUS_PLAYER_DIED;
    
    
    virtual void cleanUp();
    ~StudentWorld();
    void createSquirt(player *m_player);
    void setText();
    void removeDirt();
    void removeObj();
    bool isThereDirt(int x, int y);
    Actor* findActor(int x, int y, int IID);
    bool compareActor(Actor* actor, int type);
    void RemoveDeadActors();
    void displayActor();
    bool checkRadius(int x, int y);
    player* getPlayer();
    void deBarrel();
    void setBarrel(int b){barrel = b;}
    void addGoodies();
    bool putWaterpool(int x, int y);
    bool pickupAble(int x, int y);
    bool detected(int x, int y);
    void createGold(int x, int y);
    bool DirtBlocked(int x, int y, GraphObject::Direction d);
    Actor* RunIntoActor(int x, int y, int ID, GraphObject::Direction D);
    bool BlockSightOfProtester(int x, int y, GraphObject::Direction D);
    bool BouldersInsight(int x, int y, GraphObject::Direction d);
    void addProtester();
    void display(int B,int G,int L);
    bool IsThereBoulder(int x, int y);
    void setCounter(int x){counter = x;}
    void setNumberToAdd(int x){numberToAdd = x;}
    void findPath(int x, int y);
    GraphObject::Direction exit(int x, int y);
    void deNumberToAdd(){numberToAdd--;}
    bool cellPhone(int x, int y);
    GraphObject::Direction findFrackman(int x, int y);
    void findPath2(int x, int y);
private:
    vector<Actor*> m_actor;
    player *m_player;
    Dirt *m_dirt[64][64];
    int barrel;
    int numberToAdd;
    int counter;
    GraphObject::Direction maze[61][61];
    GraphObject::Direction search[61][61];
    
};

#endif // STUDENTWORLD_H_
