#include "Actor.h"
#include "StudentWorld.h"
#include <queue>


// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp


Actor::Actor(StudentWorld *p, int ID, int x, int y, Direction dir, double size, unsigned int depth): GraphObject(ID, x, y, dir, size, depth),m_world(p),isalive(true),m_ticks(0),state(0),annoy(0)
{

    setVisible(true);
}
player::player(StudentWorld *p, int x, int y): Actor(p, IID_PLAYER, x, y, right, 1.0, 0),water(5),sonar(1),gold(0)
{
    setHealth(10);
}

void player::doSomething()
{
    if (getHealth() == 0)
    {
        setDead();
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
        return;
    }
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch(ch)
        {
            case KEY_PRESS_ESCAPE:
                setDead();
                break;
            case KEY_PRESS_SPACE:
                if(getWater() > 0)
                {
                    fire();
                    deWater();
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                }
                break;
            case KEY_PRESS_LEFT:
                if (getDirection() != left)
                    setDirection(left);
                else
                {
                    if(getX() > 0)
                    {
                        if ((getWorld()->RunIntoActor(getX(), getY(), IID_BOULDER, getDirection()))== nullptr)
                        {
                            moveTo(getX()-1, getY());
                            getWorld()->removeDirt();
                        }
                        getWorld()->findPath(60,60);
                        getWorld()->findPath2(getX(), getY());
                    }
                    else
                        moveTo(getX(), getY());
                }
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() != right)
                    setDirection(right);
                else
                {
                    if (getX() < 60){
                        if ((getWorld()->RunIntoActor(getX(), getY(), IID_BOULDER, getDirection()))== nullptr)
                        {
                            moveTo(getX()+1, getY());
                            getWorld()->removeDirt();
                        }
                        getWorld()->findPath(60,60);
                        getWorld()->findPath2(getX(), getY());
                    }
                    else
                        moveTo(getX(), getY());
                }
                break;
            case KEY_PRESS_UP:
                if (getDirection() != up)
                    setDirection(up);
                else
                {
                    if (getY() < 60){
                        if ((getWorld()->RunIntoActor(getX(), getY(), IID_BOULDER, getDirection()))== nullptr)
                        {
                            moveTo(getX(), getY()+1);
                            getWorld()->removeDirt();
                        }
                        getWorld()->findPath(60,60);
                        getWorld()->findPath2(getX(), getY());
                    }
                    else
                        moveTo(getX(), getY());
                }
                break;
            case KEY_PRESS_DOWN:
                if (getDirection() != down)
                    setDirection(down);
                else
                {
                    if (getY() > 0){
                        if ((getWorld()->RunIntoActor(getX(), getY(), IID_BOULDER, getDirection()))== nullptr)
                        {
                            moveTo(getX(), getY()-1);
                            getWorld()->removeDirt();
                        }
                        getWorld()->findPath(60,60);
                        getWorld()->findPath2(getX(), getY());
                    }
                    else
                        moveTo(getX(), getY());
                }
                break;
            case KEY_PRESS_TAB:
                if (getGold() > 0)
                {
                    getWorld()->createGold(getX(),getY());
                    deGold();
                }
                
                break;
            case 'Z':
            case 'z':
                if (getSonar() > 0)
                {
                    getWorld()->displayActor();
                    deSonar();
                    getWorld()->playSound(SOUND_SONAR);
                }
                break;
        }
    }

}
void player::fire()
{
    getWorld()->createSquirt(this);
}

Protester::Protester(StudentWorld *p, int x, int y, int ID):Actor(p, ID, x, y, left, 1.0, 0),numOfSquaresToMove(0)
{
    setVisible(true);
}

void Protester::doSomething()
{}
bool Protester::huntPlayer()
{
    if (playerInsight() && (getX() == getWorld()->getPlayer()->getX() || getY() == getWorld()->getPlayer()->getY()))
    {
        if (getWorld()->getPlayer()->getX() < getX())
        {
            setDirection(left);
            moveTo(getX()-1, getY());
            clearTicks();
            return true;
        }
        if (getWorld()->getPlayer()->getX() > getX())
        {
            setDirection(right);
            moveTo(getX()+1, getY());
            clearTicks();
            return true;
        }
        if (getWorld()->getPlayer()->getY() < getY())
        {
            setDirection(down);
            moveTo(getX(), getY()-1);
            clearTicks();
            return true;
        }
        if (getWorld()->getPlayer()->getY() > getY())
        {
            setDirection(up);
            moveTo(getX(), getY()+1);
            clearTicks();
            return true;
        }
    }
    return false;
}

bool Protester::playerInsight()
{
    if (getWorld()->getPlayer()->getX() < getX())
    {
        if (getWorld()->BlockSightOfProtester(getX(), getY(), left) || getWorld()->BouldersInsight(getX(), getY(), left))
            return false;
    }
    if (getWorld()->getPlayer()->getX() > getX())
    {
        if (getWorld()->BlockSightOfProtester(getX(), getY(), right) || getWorld()->BouldersInsight(getX(), getY(), right))
            return false;
    }
    if (getWorld()->getPlayer()->getY() < getY())
    {
        if (getWorld()->BlockSightOfProtester(getX(), getY(), down) || getWorld()->BouldersInsight(getX(), getY(), down))
            return false;
    }
    if (getWorld()->getPlayer()->getY() > getY())
    {
        if (getWorld()->BlockSightOfProtester(getX(), getY(), up) || getWorld()->BouldersInsight(getX(), getY(), up))
            return false;
    }
    return true;
}
void Protester::wandering()
{
    Actor* boulder = getWorld()->RunIntoActor(getX(), getY(), IID_BOULDER, getDirection());
    if (getDirection() == left && !getWorld()->DirtBlocked(getX(), getY(), getDirection()) && canMoveTo() && getNumOfSquaresToMove() > 0 && boulder == nullptr)
    {
        moveTo(getX()-1, getY());
    }
    else if (getDirection() == right && canMoveTo() && !getWorld()->DirtBlocked(getX(), getY(), getDirection()) && getNumOfSquaresToMove() > 0)
    {
        moveTo(getX()+1, getY());
    }
    else if (getDirection() == up && canMoveTo() && !getWorld()->DirtBlocked(getX(), getY(), getDirection())&& getNumOfSquaresToMove() > 0)
    {
        moveTo(getX(), getY()+1);
    }
    else if (getDirection() == down && canMoveTo() && !getWorld()->DirtBlocked(getX(), getY(), getDirection())&& getNumOfSquaresToMove() > 0)
    {
        moveTo(getX(), getY()-1);
    }
    else
    {
        setNumOfSquaresToMove(0);
        chooseDirection();
    }

}
void Protester::chooseDirection()
{
    int x = rand()%4;
    if (x == 0 && getDirection() != left){
        
        setDirection(left);
        moveTo(getX(), getY());
    }
    if (x == 1 && getDirection() != right){
        setDirection(right);
        moveTo(getX(), getY());
    }
    if (x == 2 && getDirection() != up){
        setDirection(up);
        moveTo(getX(), getY());
    }
    if (x == 3 && getDirection() != down){
        setDirection(down);
        moveTo(getX(), getY());
    }
}
bool Protester::canMoveTo()
{
    if (getX() < 1 && getDirection() == left)
        return false;
    if (getX() > 59  && getDirection() == right)
        return false;
    if (getY() < 1 && getDirection() == down)
        return false;
    if (getY() > 59 && getDirection() == up)
        return false;
    return true;
}
void Protester::exit()
{
    Direction d = getWorld()->exit(getX(), getY());
    switch (d) {
        case right:
            setDirection(right);
            moveTo(getX()+1, getY());
            break;
        case left:
            setDirection(left);
            moveTo(getX()-1, getY());
            break;
        case up:
            setDirection(up);
            moveTo(getX(), getY()+1);
            break;
        case down:
            setDirection(down);
            moveTo(getX(), getY()-1);
            break;
        default:
            break;
    }
}

RegularProtester::RegularProtester(StudentWorld *p, int x, int y):Protester(p, x, y,IID_PROTESTER)
{
    setHealth(5);
}
void RegularProtester::doSomething()
{
    int level = getWorld()->getLevel();
    int ticksToWaitBetweenMoves = max(0, 3-level/4);
    int N = max(50, 100-level * 10);
    if (getNumOfSquaresToMove() <= 0)
    {
        int x = rand()%53+8;
        setNumOfSquaresToMove(x);
    }
    if (getState() == 0)
    {
        if (getCurrentTicks() < ticksToWaitBetweenMoves){
            incrementTick();
            return;
        }
        Actor* target = getWorld()->RunIntoActor(getX(), getY(), IID_GOLD, getDirection());
        if (target != nullptr)
        {
            setState(2);
            getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
            getWorld()->increaseScore(25);
            target->setDead();
        }
        else
        {
            if (huntPlayer())
            {
                if (getWorld()->checkRadius(getX(), getY())){
                    getWorld()->playSound(SOUND_PROTESTER_YELL);
                    getWorld()->getPlayer()->annoyed();
                    setState(1);
                    return;
                }
                return;
            }
            if (getWorld()->checkRadius(getX(), getY())){
                getWorld()->playSound(SOUND_PROTESTER_YELL);
                getWorld()->getPlayer()->annoyed();
                setState(1);
                return;
            }
            deNumOfSquaresToMove();
            wandering();
            clearTicks();
        }
    }
    if (getState() == 1)
    {
        incrementTick();
        if (getCurrentTicks() < 60)
            return;
        setState(0);
        clearTicks();
    }
    if (getState() == 2)
    {
        if (getCurrentTicks() < ticksToWaitBetweenMoves){
            incrementTick();
            return;
        }
        exit();
        clearTicks();
        if (getX() == 60 && getY() == 60){
            setDead();
            getWorld()->deNumberToAdd();
        }
        
    }
    if (getState() == 3)
    {
        if (getHealth() < 0)
        {
            getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
            setState(2);
        }
        else
        {
            if (getCurrentTicks() < N){
                incrementTick();
                return;
            }
            clearTicks();
            setState(0);
        }
    }

}
HardcoreProtester::HardcoreProtester(StudentWorld *p, int x, int y):Protester(p, x, y,IID_HARD_CORE_PROTESTER)
{
    setHealth(14);
    setVisible(true);
}

void HardcoreProtester::doSomething()
{
    int level = getWorld()->getLevel();
    int ticksToWaitBetweenMoves = max(0, 3-level/4);
    int N = max(50, 100-level * 10);
    if (getNumOfSquaresToMove() <= 0)
    {
        int x = rand()%53+8;
        setNumOfSquaresToMove(x);
    }
    if (getState() == 0)
    {
        if (getCurrentTicks() < ticksToWaitBetweenMoves){
            incrementTick();
            return;
        }
        if (getWorld()->cellPhone(getX(), getY()))
            setState(5);
        Actor* target = getWorld()->RunIntoActor(getX(), getY(), IID_GOLD, getDirection());
        if (target != nullptr)
        {
            setState(4);
            getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
            getWorld()->increaseScore(50);
            target->setDead();
        }
        else
        {
            if (getWorld()->cellPhone(getX(), getY()))
                setState(5);
            if (huntPlayer())
            {
                if (getWorld()->checkRadius(getX(), getY())){
                    getWorld()->playSound(SOUND_PROTESTER_YELL);
                    getWorld()->getPlayer()->annoyed();
                    setState(1);
                }
                return;
            }
            if (getWorld()->checkRadius(getX(), getY())){
                getWorld()->playSound(SOUND_PROTESTER_YELL);
                getWorld()->getPlayer()->annoyed();
                setState(1);
            }
            deNumOfSquaresToMove();
            wandering();
            clearTicks();
        }
    }
    if (getState() == 1)
    {
        incrementTick();
        if (getCurrentTicks() < 60)
            return;
        setState(0);
        clearTicks();
    }
    if (getState() == 2)
    {
        if (getCurrentTicks() < ticksToWaitBetweenMoves){
            incrementTick();
            return;
        }
        exit();
        clearTicks();
        if (getX() == 60 && getY() == 60){
            getWorld()->deNumberToAdd();
            setDead();
        }
    }
    if (getState() == 3)
    {
        if (getHealth() < 0)
        {
            getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
            setState(2);
        }
        else
        {
            if (getCurrentTicks() < N){
                incrementTick();
                return;
            }
            setState(0);
        }
    }
    if (getState() == 4)
    {
        if (getCurrentTicks() < N){
            incrementTick();
            return;
        }
        setState(0);
        clearTicks();
    }
    if (getState() == 5)
    {
        if (getCurrentTicks() < ticksToWaitBetweenMoves){
            incrementTick();
            return;
        }
        if (getWorld()->checkRadius(getX(), getY())){
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->getPlayer()->annoyed();
            setState(1);
        }
        Actor* target = getWorld()->RunIntoActor(getX(), getY(), IID_GOLD, getDirection());
        if (target != nullptr)
        {
            setState(4);
            getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
            target->setDead();
        }
        searchFrackman();
        if (!getWorld()->cellPhone(getX(), getY()))
            setState(0);
        clearTicks();
    }
}
void HardcoreProtester::searchFrackman()
{
    Direction d = getWorld()->findFrackman(getX(), getY());
    switch (d) {
        case right:
            setDirection(right);
            moveTo(getX()+1, getY());
            break;
        case left:
            setDirection(left);
            moveTo(getX()-1, getY());
            break;
        case up:
            setDirection(up);
            moveTo(getX(), getY()+1);
            break;
        case down:
            setDirection(down);
            moveTo(getX(), getY()-1);
            break;
        default:
            break;
    }
}

Squirt::Squirt(StudentWorld *p, int x, int y, Direction dir):Actor(p, IID_WATER_SPURT, x, y, dir, 1.0, 0),distance(0)
{
    setVisible(true);
}
void Squirt::doSomething()
{
    Actor* target1 = getWorld()->RunIntoActor(getX(), getY(), IID_PROTESTER, getDirection());
    Actor* target2 = getWorld()->RunIntoActor(getX(), getY(), IID_HARD_CORE_PROTESTER, getDirection());
    Actor* target3 = getWorld()->RunIntoActor(getX(), getY(), IID_BOULDER, getDirection());
    Direction d = getWorld()->getPlayer()->getDirection();
    if (getX() < 0|| getX() > 60 || getY() < 0 || getY() > 60 )
        setDead();
    else
    {
        distance++;
        if (d == GraphObject::right && distance < 7)
        {
            if (!getWorld()->DirtBlocked(getX(), getY(), getDirection())
                && target1 == nullptr && target2 == nullptr && target3 == nullptr)
                    moveTo(getX()+1, getY());
            else if (target1 != nullptr)
            {
                target1->annoyed();
                target1->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
                target1->setState(3);
                setDead();
                if (target1->getHealth() < 0)
                    getWorld()->increaseScore(100);
            }
            else if (target2 != nullptr)
            {
                target2->annoyed();
                target2->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
                target2->setState(3);
                setDead();
                if (target2->getHealth() < 0)
                    getWorld()->increaseScore(250);
            }
            else
                setDead();
        }
        else if (d == GraphObject::left && distance < 7)
        {
            if (!getWorld()->DirtBlocked(getX(), getY(), getDirection())
                && target1 == nullptr && target2 == nullptr && target3 == nullptr)
                moveTo(getX()-1, getY());
            else if (target1 != nullptr)
            {
                target1->annoyed();
                target1->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
                target1->setState(3);
                setDead();
                if (target1->getHealth() < 0)
                    getWorld()->increaseScore(100);
            }
            else if (target2 != nullptr)
            {
                target2->annoyed();
                target2->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
                target2->setState(3);
                setDead();
                if (target2->getHealth() < 0)
                    getWorld()->increaseScore(250);
            }
            else
                setDead();
        }
        else if (d == GraphObject::up && distance < 7)
        {
            if (!getWorld()->DirtBlocked(getX(), getY(), getDirection())
                && target1 == nullptr && target2 == nullptr && target3 == nullptr)
                moveTo(getX(), getY()+1);
            else if (target1 != nullptr)
            {
                target1->annoyed();
                target1->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
                target1->setState(3);
                setDead();
                if (target1->getHealth() < 0)
                    getWorld()->increaseScore(100);
            }
            else if (target2 != nullptr)
            {
                target2->annoyed();
                target2->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
                target2->setState(3);
                setDead();
                if (target2->getHealth() < 0)
                    getWorld()->increaseScore(250);
            }
            else
                setDead();
        }
        else if (d == GraphObject::down && distance < 7)
        {
            if (!getWorld()->DirtBlocked(getX(), getY(), getDirection())
                && target1 == nullptr && target2 == nullptr && target3 == nullptr)
                moveTo(getX(), getY()-1);
            else if (target1 != nullptr)
            {
                target1->annoyed();
                target1->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
                target1->setState(3);
                setDead();
                if (target1->getHealth() < 0)
                    getWorld()->increaseScore(100);
            }
            else if (target2 != nullptr)
            {
                target2->annoyed();
                target2->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
                target2->setState(3);
                setDead();
                if (target2->getHealth() < 0)
                    getWorld()->increaseScore(250);
            }
            else
                setDead();
        }
        else
            setDead();
    }

}

Dirt::Dirt(StudentWorld *p, int x, int y): Actor(p, IID_DIRT, x, y, right, 0.25, 3)
{}

void Dirt::doSomething()
{}

boulders::boulders(StudentWorld *p, int x ,int y):Actor(p, IID_BOULDER, x, y, down, 1.0, 1)
{
    setVisible(true);
}
void boulders::doSomething()
{
    
    if (getState() == 0)
    {
        if (!getWorld()->isThereDirt(getX(), getY()))
        {
            incrementTick();
            if (getCurrentTicks() < 30)
                return;
            setState(1);
            getWorld()->playSound(SOUND_FALLING_ROCK);
        }
    }
    else
    {
        if(!getWorld()->isThereDirt(getX(), getY()) && getY() > -1)
            moveTo(getX(), getY()-1);
        else
            setDead();

        Actor* target = getWorld()->RunIntoActor(getX(), getY(), IID_PLAYER, getDirection());
        if (target != nullptr)
        {
            target->setDead();
            target->setHealth(0);
            target->getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
        }
        Actor* target1 = getWorld()->RunIntoActor(getX(), getY(), IID_PROTESTER, getDirection());
        if (target1 != nullptr)
        {
            if (getAnnoyed() == 0 && target1->getHealth() > 0)
            {
                target1->setHealth(0);
                target1->getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
                target1->setState(2);
                getWorld()->increaseScore(500);
            }
    
        }
        Actor* target2 = getWorld()->RunIntoActor(getX(), getY(), IID_HARD_CORE_PROTESTER, getDirection());
        if (target2 != nullptr)
        {
            if (getAnnoyed() == 0 && target2->getHealth() > 0)
            {
                target2->setHealth(0);
                target2->getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
                target2->setState(2);
                getWorld()->increaseScore(500);
            }
        }
    }
}


Gold::Gold(StudentWorld *p, int x, int y):Actor(p, IID_GOLD, x, y, right, 1.0, 2)
{
    setVisible(false);
}

void Gold::doSomething()
{
    if (getState() == 0)
    {
        if (getWorld()->checkRadius(getX(), getY()))
        {
            setVisible(true);
            setState(1);
        }
    }
    if (getState() == 1)
    {
        incrementTick();
        if (getCurrentTicks() < 2)
            return;
        if (getWorld()->pickupAble(getX(), getY()))
        {
            getWorld()->playSound(SOUND_GOT_GOODIE);
            setDead();
            getWorld()->increaseScore(10);
            getWorld()->getPlayer()->addGold();
            clearTicks();
        }
    }
    if (getState() == 2)
    {
        setVisible(true);
        incrementTick();
        if (getCurrentTicks() < 100)
            return;
        setDead();
    }

}

Barrel::Barrel(StudentWorld *p, int x, int y):Actor(p, IID_BARREL,x, y,right,1.0,2)
{
    setVisible(false);
}

void Barrel::doSomething()
{
    if (getWorld()->checkRadius(getX(), getY()))
    {
        setVisible(true);
        setState(1);
    }
    
    if (getWorld()->pickupAble(getX(), getY()))
    {
        getWorld()->playSound(SOUND_FOUND_OIL);
        setDead();
        getWorld()->increaseScore(1000);
        getWorld()->deBarrel();
    }

}

WaterRefills::WaterRefills(StudentWorld *p, int x, int y):Actor(p, IID_WATER_POOL, x, y, right, 1.0, 2)
{
    
}
void WaterRefills::doSomething()
{
    if (getWorld()->pickupAble(getX(), getY()))
    {
        getWorld()->playSound(SOUND_GOT_GOODIE);
        setDead();
        getWorld()->increaseScore(100);
        getWorld()->getPlayer()->addWater();
    }
    int level = getWorld()->getLevel();
    int T = max(100, 300 - 10*level);
    incrementTick();
    if (getCurrentTicks() < T)
        return;
    setDead();
}

Sonar::Sonar(StudentWorld *p, int x, int y):Actor(p, IID_SONAR, x, y, right, 1.0, 2)
{

}
void Sonar::doSomething()
{
    if (getWorld()->pickupAble(getX(), getY()))
    {
        getWorld()->playSound(SOUND_GOT_GOODIE);
        setDead();
        getWorld()->increaseScore(75);
        getWorld()->getPlayer()->addSonar();
    }
    int level = getWorld()->getLevel();
    int T = max(100, 300 - 10*level);
    incrementTick();
    if (getCurrentTicks() < T)
        return;
    setDead();
}



