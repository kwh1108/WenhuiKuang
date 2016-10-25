#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld *p, int ID, int x, int y, Direction dir, double size, unsigned int depth);
    virtual void doSomething()=0;
    StudentWorld *getWorld() const {return m_world;}
    int getHealth() const {return m_health;}
    virtual ~Actor(){};
    void annoyed(){m_health -= 2;}
    void setHealth(int health){m_health = health;}
    bool isAlive() const {return isalive;}
    void setDead(){isalive = false;}
    virtual void incrementTick(){m_ticks++;}
    int getCurrentTicks(){return m_ticks;}
    int getState(){return state;}
    void setState(int s){state = s;}
    void clearTicks(){m_ticks = 0;}
    void setAnnoyed(int x){annoy = x;}
    int getAnnoyed(){return annoy;}
private:
    int m_ticks;
    int m_health;
    StudentWorld *m_world;
    bool isalive;
    int state;
    int annoy;
};

class player : public Actor
{
public:
    player(StudentWorld *p, int x, int y);
    virtual void doSomething();
    void fire();
    int getWater(){return water;}
    int getSonar(){return sonar;}
    int getGold(){return gold;}
    void deWater(){water -= 1;}
    void addWater(){water += 5;}
    void addGold(){gold++;}
    void deGold(){gold -= 1;}
    void addSonar(){sonar += 2;}
    void deSonar(){sonar -= 1;}
    virtual ~player(){};
private:
    int water,sonar,gold;
};

class Protester: public Actor
{
public:
    Protester(StudentWorld *p, int x, int y,int ID);
    virtual void doSomething();
    void setNumOfSquaresToMove(int n){numOfSquaresToMove = n;}
    int getNumOfSquaresToMove(){return numOfSquaresToMove;}
    void deNumOfSquaresToMove(){numOfSquaresToMove -= 1;}
    virtual ~Protester(){};
    bool canMoveTo();
    void chooseDirection();
    void wandering();
    bool huntPlayer();
    bool playerInsight();
    void exit();
private:
    int numOfSquaresToMove;
};
class HardcoreProtester: public Protester
{
public:
    HardcoreProtester(StudentWorld *p, int x, int y);
    virtual void doSomething();
    void searchFrackman();
    virtual ~HardcoreProtester(){};
};

class RegularProtester: public Protester
{
public:
    RegularProtester(StudentWorld *p, int x, int y);
    virtual void doSomething();
    virtual ~RegularProtester(){};
};
class Dirt : public Actor
{
public:
    Dirt(StudentWorld *p, int x, int y);
    virtual void doSomething();
    virtual ~Dirt(){};
};
class Barrel : public Actor
{
public:
    Barrel(StudentWorld *p, int x, int y);
    virtual void doSomething();
    virtual ~Barrel(){};
};
class boulders : public Actor
{
public:
    boulders(StudentWorld *p, int x, int y);
    virtual void doSomething();
    virtual ~boulders(){};
};
class Gold : public Actor
{
public:
    Gold(StudentWorld *p, int x, int y);
    virtual void doSomething();
    virtual ~Gold(){};
};
class Squirt : public Actor
{
public:
    Squirt(StudentWorld *p, int x, int y, Direction dir);
    virtual void doSomething();
    virtual ~Squirt(){};
private:
    int distance;
};
class Sonar : public Actor
{
public:
    Sonar(StudentWorld *p, int x, int y);
    virtual void doSomething();
    virtual ~Sonar(){};
};
class WaterRefills : public Actor
{
public:
    WaterRefills(StudentWorld *p, int x, int y);
    virtual void doSomething();
    virtual ~WaterRefills(){};
};
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_

