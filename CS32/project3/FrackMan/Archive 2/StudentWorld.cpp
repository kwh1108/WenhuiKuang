#include "Actor.h"
#include "StudentWorld.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <iostream>
#include <queue>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

class Coord
{
public:
    Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
    int r() const { return m_r; }
    int c() const { return m_c; }
private:
    int m_r;
    int m_c;
};

int StudentWorld::init()
{
    numberToAdd = 0;
    m_player = new player(this, 30, 60);
 //   boulders *b = new boulders(this, 40,60);
  //  m_actor.push_back(b);
    int level = getLevel();
    int B = min(level / 2 + 2, 6);
    int G = max(5-level / 2, 2);
    int L = min(2+level, 20);
    setBarrel(L);
    barrel = L;
    for (int i = 0; i < 64; i++) //add dirts
    {
        for (int j = 0; j < 64; j++)
        {
            if ((i < 60 && i > 3) && (j > 29 && j < 34))
            {
                m_dirt[j][i] = nullptr;
                continue;
            }
            if (i > 59 )
            {
                m_dirt[j][i] = nullptr;
                
                continue;
            }
            m_dirt[j][i] = new Dirt(this, j , i);
            
        }
    }
    findPath(60,60);
    int probabilityOfHardcore = min(90, level* 10 + 30);
    int x = rand()%probabilityOfHardcore;
    if (x == 1)
    {
        Actor *m_p = new HardcoreProtester(this, 60, 60);
        m_actor.push_back(m_p);
    }
    else
    {
       Actor *m_p = new RegularProtester(this, 60, 60);
       m_actor.push_back(m_p);
    }
    numberToAdd++;
    display(B, G, L);
    return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move()
{
    addProtester();
    setText();
    if (m_player->isAlive())
    {
        m_player->doSomething();
    }
    if (m_actor.size() != 0)   //all actors dosomething
    {
        for (vector<Actor*>::iterator p = m_actor.begin(); p != m_actor.end(); p++)
        {
            if ((*p)->isAlive())
            {
                (*p)->doSomething();
                if (m_player->isAlive() == false) //check if player alive
                {
                    if (getLives() > 0){
                        decLives();
                        return GWSTATUS_PLAYER_DIED;
                    }
                }
            }
        }
    }
    addGoodies();
    RemoveDeadActors();
    if (barrel == 0)
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::deBarrel()
{
    if (barrel >0)
        barrel--;
}

void StudentWorld::createSquirt(player *p)
{
    GraphObject::Direction d = p->getDirection();
    int x = p->getX();
    int y = p->getY();
    
    switch(d)
    {
        case GraphObject::down:
            if (y >= 3)
                y -= 3;
            break;
        case GraphObject::left:
            if (x >= 3)
                x -= 3;
            break;
        case GraphObject::right:
            if (x <= 60)
                x += 3;
            break;
        case GraphObject::up:
            if (y <= 60)
                y += 3;
            break;
        case GraphObject::none:
            break;
    }
    Squirt *s = new Squirt(this, x , y, d);
    m_actor.push_back(s);
}

void StudentWorld::cleanUp()
{
    delete m_player;
    m_player = nullptr;
    
    if (m_actor.size() != 0) //go through the list to delete actors
    {
        for (vector<Actor*>::iterator p = m_actor.begin(); p != m_actor.end();)
        {
            delete *p;
            vector<Actor*>::iterator q = m_actor.erase(p);
            p = q;
        }
    }
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            if (m_dirt[i][j] != nullptr)
            {
                delete m_dirt[i][j];
                m_dirt[i][j] = nullptr;
            }
        }
    }

}

void StudentWorld::setText()
{
    int score = getScore();
    int lev = getLevel();
    int life = getLives();
    int health = m_player->getHealth()*10;
    int water = m_player->getWater();
    int gold = m_player->getGold();
    int sonar = m_player->getSonar();
    ostringstream s;
    ostringstream ss;
    s.fill('0');
    ss.fill(' ');
    s << "scr: " << setw(6) << score;
    ss << "  lev: " << setw(2) << lev << "  Lives: " << setw(1) << life << "  Hlth: " << setw(3) << health << "%" << "  Wtr: "<< setw(2) << water << "  Gld: " << setw(2) << gold << "  Sonar: " << setw(2) << sonar << "  Oil Left: " << setw(2) << barrel;
    string text = s.str() + ss.str();
    setGameStatText(text);
    
}



void StudentWorld::removeDirt()
{
    int x = m_player->getX();
    int y = m_player->getY();
    bool found = false;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (m_dirt[x+j][y+i] != nullptr)
            {
                found = true;
                delete m_dirt[x+j][y+i];
                m_dirt[x+j][y+i] = nullptr;
            }
        }
    }
    if (found)
        playSound(SOUND_DIG);
    
}

void StudentWorld::display(int B, int G, int L)
{
    for (int i = 0; i < B; i++)   // add boulders
    {
        bool OkToPut = true;
        int x,y;
        if (rand()%2 == 1)
        {
            x = rand()%24;
            y = rand()%37 + 20;
        }
        else
        {
            x = rand()%23 + 34;
            y = rand()%37 + 20;
        }
        for (vector<Actor*>::iterator p = m_actor.begin(); p != m_actor.end(); p++)
        {
            
            if (sqrt(pow(x-(*p)->getX(),2) + pow(y-(*p)->getY(),2)) <= 6)
            {
                i--;
                OkToPut = false;
            }
        }
        if (OkToPut)
        {
            m_actor.push_back(new boulders(this, x, y));
            for (int i = x; i < x+4; i++)
            {
                for (int j = y; j < y+4; j++)
                {
                    delete m_dirt[i][j];
                    m_dirt[i][j] = nullptr;
                }
            }
        }
    }
 
    for (int i = 0; i < G; i++) // add golds
    {
        bool OkToPut = true;
        int x,y;
        if (rand()%2 == 1)
        {
            x = rand()%26;
            y = rand()%57;
        }
        else
        {
            x = rand()%23 + 34;
            y = rand()%57;
        }
        for (vector<Actor*>::iterator p = m_actor.begin(); p != m_actor.end(); p++)
        {
            if (sqrt(pow(x-(*p)->getX(),2) + pow(y-(*p)->getY(),2)) <= 6)
            {
                i--;
                OkToPut = false;
            }
        }
        if (OkToPut)
            m_actor.push_back(new Gold(this, x, y));
    }
    for (int i = 0; i < L; i++)
    {
        bool OkToPut = true;
        int x,y;
        if (rand()%2 == 1)
        {
            x = rand()%26;
            y = rand()%57;
        }
        else
        {
            x = rand()%23 + 34;
            y = rand()%57;
        }
        for (vector<Actor*>::iterator p = m_actor.begin(); p != m_actor.end(); p++)
        {
            
            if (sqrt(pow(x-(*p)->getX(),2) + pow(y-(*p)->getY(),2)) <= 6)
            {
                i--;
                OkToPut = false;
            }
        }
        if (OkToPut)
            m_actor.push_back(new Barrel(this, x, y));
    }

}

Actor* StudentWorld::findActor(int x, int y, int IID)
{ //find a specific type actor in specific location
    if (IID == IID_PLAYER && m_player != nullptr && m_player->getX() == x && m_player->getY() == y)
    {
        return m_player;
    }
    if (m_actor.size() == 0) return nullptr;
    for (vector<Actor*>::iterator p = m_actor.begin(); p != m_actor.end(); p++)
    {
        if (x == (*p)->getX() && y == (*p)->getY() && compareActor(*p, IID))
            return (*p);
    }
    return nullptr;
}

bool StudentWorld::compareActor(Actor* actor, int type)
{
    if (actor != nullptr)     //using dynamic cast to check desire type.
    {
        if (type == IID_WATER_SPURT)
        {
            Squirt* squirt = dynamic_cast<Squirt*>(actor);
            if (squirt != nullptr) return true;
        }
        else if (type == IID_PROTESTER)
        {
            Protester* protester = dynamic_cast<Protester*>(actor);
            if (protester != nullptr) return true;
        }
        else if (type == IID_BOULDER)
        {
            boulders* boulder = dynamic_cast<boulders*>(actor);
            if (boulder != nullptr) return true;
        }
        else if (type == IID_BARREL)
        {
            Barrel* barrel = dynamic_cast<Barrel*>(actor);
            if (barrel != nullptr) return true;
        }
        else if (type == IID_GOLD)
        {
            Gold* gold = dynamic_cast<Gold*>(actor);
            if (gold != nullptr) return true;
        }
        else if (type == IID_HARD_CORE_PROTESTER)
        {
            HardcoreProtester* hard = dynamic_cast<HardcoreProtester*>(actor);
            if (hard != nullptr) return true;
        }
        else if (type == IID_SONAR)
        {
            Sonar* sonar = dynamic_cast<Sonar*>(actor);
            if (sonar != nullptr) return true;
        }
        else if (type == IID_WATER_POOL)
        {
            WaterRefills* water = dynamic_cast<WaterRefills*>(actor);
            if (water != nullptr) return true;
        }
    }
    return false;
}

bool StudentWorld::isThereDirt(int x, int y)
{
    for (int i = x; i < x+4; i++)
    {
        if(m_dirt[i][y-1] != nullptr)
            return true;
    }
    return false;
}

void StudentWorld::RemoveDeadActors()
{
    if (m_actor.size() != 0)
    {
        for (vector<Actor*>::iterator p = m_actor.begin(); p != m_actor.end(); p++)
        {
            if ((*p)->isAlive() == false)
            {
                delete *p;
                vector<Actor*>::iterator q = m_actor.erase(p);
                p = q;
                p--;
            }
        }
    }
}

bool StudentWorld::detected(int x, int y)
{
    for (int i = 0; i < 24; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            if (m_player->getX()-12+i == x && m_player->getY()-12+j == y)
                return true;
        }
    }
    return false;
}

bool StudentWorld::checkRadius(int x, int y)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (m_player->getX()-4+i == x && m_player->getY()-4+j == y)
                return true;
        }
    }
    return false;
}

bool StudentWorld::pickupAble(int x, int y)
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (m_player->getX()-3+i == x && m_player->getY()-3+j == y)
                return true;
        }
    }
    return false;
}

void StudentWorld::displayActor()
{
    for (vector<Actor*>::iterator p = m_actor.begin(); p != m_actor.end(); p++)
    {
        if(detected((*p)->getX(), (*p)->getY())){
            (*p)->setVisible(true);
            return;
        }
    }
}

player* StudentWorld::getPlayer()
{
    return m_player;
}

void StudentWorld::addGoodies()
{
    int G = getLevel() * 25 + 300;
    if (rand()%G == 1)
    {
        if (rand()%5 == 0)
            m_actor.push_back(new Sonar(this, 0, 60));
        else
        {
            int x = rand()%60;
            int y = rand()%60;
            while (!putWaterpool(x, y) || IsThereBoulder(x, y))
            {
                x = rand()%60;
                y = rand()%60;
            }
            m_actor.push_back(new WaterRefills(this,x,y));
        }
    }
}

bool StudentWorld::putWaterpool(int x, int y)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (m_dirt[x+i][y+j] != nullptr)
                return false;
        }
    }
    return true;
}

bool StudentWorld::IsThereBoulder(int x, int y)
{
    for (int i = x-3; i < x + 4; i++)
    {
        for (int j = y - 3; j < y + 4; j++)
        {
            Actor* target = findActor(i, j, IID_BOULDER);
            if (target != nullptr)
                return true;
        }
    }
    return false;
}
/*
Actor* StudentWorld::findProtester(int x, int y, int ID)
{
    if (m_actor.size() == 0) return false;
    for (vector<Actor*>::iterator p = m_actor.begin(); p != m_actor.end(); p++)
    {
        if (x == (*p)->getX() && y == (*p)->getY() && compareActor(*p, ID))
        {
            return true;
        }
    }
    return false;
}
*/

void StudentWorld::createGold(int x, int y)
{
    Gold *g = new Gold(this,x,y);
    g->setState(2);
    m_actor.push_back(g);
}

bool StudentWorld::DirtBlocked(int x, int y, GraphObject::Direction d)
{
    if (d == GraphObject::right)
    {
        for (int i = y; i < y + 4; i++)
        {
            if (m_dirt[x+4][i] != nullptr)
                return true;
        }
    }
    if (d == GraphObject::left)
    {
        for (int i = y; i < y + 4; i++)
        {
            if (m_dirt[x-1][i] != nullptr)
                return true;
        }
    }
    if (d == GraphObject::up)
    {
        for (int i = x; i < x + 4; i++)
        {
            if (m_dirt[i][y+4] != nullptr)
                return true;
        }
    }
    if (d == GraphObject::down)
    {
        for (int i = x; i < x + 4; i++)
        {
            if (m_dirt[i][y-1] != nullptr)
                return true;
        }
    }
    return false;
}

Actor* StudentWorld::RunIntoActor(int x, int y, int ID, GraphObject::Direction D)
{
    if (D == GraphObject::right)
    {
        
        for (int i = y-3; i < y+3; i++)
        {
            for (int j = x; j < x + 4; j++)
            {
                Actor* character = findActor(j, i, ID);
                if (character != nullptr)
                    return character;
            }
        }
    }
    if (D == GraphObject::down)
    {
        for (int i = x-3; i < x+3; i++)
        {
            for (int j = y; j > y - 4; j--)
            {
                Actor* character = findActor(i, j, ID);
                if (character != nullptr)
                    return character;
            }
        }
    }
    if (D == GraphObject::left)
    {
        for (int i = y-3; i < y+3; i++)
        {
            for (int j = x; j > x - 4; j--)
            {
                Actor* character = findActor(j, i, ID);
                if (character != nullptr)
                    return character;
            }
        }
    }
    if (D == GraphObject::up)
    {
        for (int i = x-3; i < x+3; i++)
        {
            for (int j = y; j < y + 4; j++)
            {
                Actor* character = findActor(i, j, ID);
                if (character != nullptr)
                    return character;
            }
        }
    }
    return nullptr;
}

bool StudentWorld::BlockSightOfProtester(int x, int y,GraphObject::Direction d)
{
    if (d == GraphObject::right)
    {
        for (int i = x+4; i < m_player->getX(); i++)
        {
            for (int j = y; j < y + 4; j++)
            {
             //   Actor* target = findActor(i, j, IID_BOULDER);
                if (m_dirt[i][j] != nullptr)
                    return true;
            }
        }
    }
    if (d == GraphObject::left)
    {
        for (int i = x; i > m_player->getX(); i--)
        {
            for (int j = y; j < y + 4; j++)
            {
              //  Actor* target = findActor(i, j, IID_BOULDER);
                if (m_dirt[i][j] != nullptr)
                    return true;
            }
        }
    }
    if (d == GraphObject::up)
    {
        for (int j = y; j < m_player->getY(); j++)
        {
            for (int i = x; i < x + 4; i++)
            {
            //    Actor* target = findActor(i, j, IID_BOULDER);
                if (m_dirt[i][j] != nullptr)
                    return true;
            }
        }
    }
    if (d == GraphObject::down)
    {
        for (int j = y; j > m_player->getY(); j--)
        {
            for (int i = x; i < x + 4; i++)
            {
           //     Actor* target = findActor(i, j, IID_BOULDER);
                if (m_dirt[i][j] != nullptr)
                    return true;
            }
        }
    }
    return false;
}
bool StudentWorld::BouldersInsight(int x, int y, GraphObject::Direction d)
{
    if (d == GraphObject::right)
    {
        for (int i = x+4; i < m_player->getX(); i++)
        {
            for (int j = y-3; j < y + 4; j++)
            {
                Actor* target = findActor(i, j, IID_BOULDER);
                if (target != nullptr)
                    return true;
            }
        }
    }
    if (d == GraphObject::left)
    {
        for (int i = x; i > m_player->getX(); i--)
        {
            for (int j = y-3; j < y + 4; j++)
            {
                Actor* target = findActor(i, j, IID_BOULDER);
                if (target != nullptr)
                    return true;
            }
        }
    }
    if (d == GraphObject::up)
    {
        for (int j = y; j < m_player->getY(); j++)
        {
            for (int i = x-3; i < x + 4; i++)
            {
                Actor* target = findActor(i, j, IID_BOULDER);
                if (target != nullptr)
                    return true;
            }
        }
    }
    if (d == GraphObject::down)
    {
        for (int j = y; j > m_player->getY(); j--)
        {
            for (int i = x-3; i < x + 4; i++)
            {
                Actor* target = findActor(i, j, IID_BOULDER);
                if (target != nullptr)
                    return true;
            }
        }
    }
    return false;
}

void StudentWorld::addProtester()
{
    int level = getLevel();
    int num = level * 1.5;
    int P = min(15, 2 + num);
    int T = max(25, 200-level);
    int probabilityOfHardcore = min(90, level* 10 + 30);
    counter++;
    while (numberToAdd < P && counter >= T)
    {
        int x = rand()%probabilityOfHardcore;
        if (x == 1)
        {
            Actor *m_p = new HardcoreProtester(this, 60, 60);
            m_actor.push_back(m_p);
        }
        else
        {
            Actor *m_p = new RegularProtester(this, 60, 60);
            m_actor.push_back(m_p);
        }
        numberToAdd++;
        setCounter(0);
    }
    

}

void StudentWorld::findPath(int x, int y)
{
    queue<Coord> coordQueue;
    coordQueue.push(Coord(x, y));
    char maze1[61][61];
    for (int i = 0; i < 61; i++)
    {
        for (int j = 0; j < 61; j++)
        {
            maze1[i][j] = '.';
        }
    }
    for (int i = 0; i < 61; i++)
    {
        for (int j = 0; j < 61; j++)
        {
            maze[i][j] = GraphObject::none;
        }
    }
    maze1[60][60] = '#';
    while (!coordQueue.empty())
    {
        Coord p = coordQueue.front();
        int x = p.r();
        int y = p.c();
        coordQueue.pop();
        if (x > 0 && maze1[x-1][y] == '.' && !DirtBlocked(x, y,GraphObject::left))
        {
            Actor* actor = RunIntoActor(x, y, IID_BOULDER, GraphObject::left);
            if (actor == nullptr)
            {
                coordQueue.push(Coord(x-1,y));
                maze1[x-1][y] = '#';
                maze[x-1][y] = GraphObject::right;
            }
        }
        if (y < 60 && maze1[x][y+1] == '.' && !DirtBlocked(x, y,GraphObject::up))
        {
            Actor* actor = RunIntoActor(x, y, IID_BOULDER, GraphObject::up);
            if (actor == nullptr)
            {
                coordQueue.push(Coord(x,y+1));
                maze1[x][y+1] = '#';
                maze[x][y+1] = GraphObject::down;
            }
        }
        if (x < 60 && maze1[x+1][y] == '.' && !DirtBlocked(x, y,GraphObject::right))
        {
            Actor* actor = RunIntoActor(x, y, IID_BOULDER, GraphObject::right);
            if (actor == nullptr)
            {
                coordQueue.push(Coord(x+1,y));
                maze1[x+1][y] = '#';
                maze[x+1][y] = GraphObject::left;
            }
        }
        if (y > 0 &&  maze1[x][y-1] == '.' && !DirtBlocked(x, y,GraphObject::down))
        {
            Actor* actor = RunIntoActor(x, y, IID_BOULDER, GraphObject::down);
            if (actor == nullptr)
            {
                coordQueue.push(Coord(x,y-1));
                maze1[x][y-1] = '#';
                maze[x][y-1] = GraphObject::up;
            }
        }

    }
}
bool StudentWorld::cellPhone(int x, int y)
{
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            if (m_player->getX()-16+i == x && m_player->getY()-16+j == y)
                return true;
        }
    }
    return false;
}

GraphObject::Direction StudentWorld::exit(int x, int y)
{
    return maze[x][y];
}

GraphObject::Direction StudentWorld::findFrackman(int x, int y)
{
    return search[x][y];
}

void StudentWorld::findPath2(int x, int y)
{
    queue<Coord> coordQueue;
    coordQueue.push(Coord(x, y));
    char maze1[61][61];
    for (int i = 0; i < 61; i++)
    {
        for (int j = 0; j < 61; j++)
        {
            maze1[i][j] = '.';
        }
    }
    for (int i = 0; i < 61; i++)
    {
        for (int j = 0; j < 61; j++)
        {
            search[i][j] = GraphObject::none;
        }
    }
    maze1[60][60] = '#';
    while (!coordQueue.empty())
    {
        Coord p = coordQueue.front();
        int x = p.r();
        int y = p.c();
        coordQueue.pop();
        if (x > 0 && maze1[x-1][y] == '.' && !DirtBlocked(x, y,GraphObject::left))
        {
            Actor* actor = RunIntoActor(x, y, IID_BOULDER, GraphObject::left);
            if (actor == nullptr)
            {
                coordQueue.push(Coord(x-1,y));
                maze1[x-1][y] = '#';
                search[x-1][y] = GraphObject::right;
            }
        }
        if (y < 60 && maze1[x][y+1] == '.' && !DirtBlocked(x, y,GraphObject::up))
        {
            Actor* actor = RunIntoActor(x, y, IID_BOULDER, GraphObject::up);
            if (actor == nullptr)
            {
                coordQueue.push(Coord(x,y+1));
                maze1[x][y+1] = '#';
                search[x][y+1] = GraphObject::down;
            }
        }
        if (x < 60 && maze1[x+1][y] == '.' && !DirtBlocked(x, y,GraphObject::right))
        {
            Actor* actor = RunIntoActor(x, y, IID_BOULDER, GraphObject::right);
            if (actor == nullptr)
            {
                coordQueue.push(Coord(x+1,y));
                maze1[x+1][y] = '#';
                search[x+1][y] = GraphObject::left;
            }
        }
        if (y > 0 &&  maze1[x][y-1] == '.' && !DirtBlocked(x, y,GraphObject::down))
        {
            Actor* actor = RunIntoActor(x, y, IID_BOULDER, GraphObject::down);
            if (actor == nullptr)
            {
                coordQueue.push(Coord(x,y-1));
                maze1[x][y-1] = '#';
                search[x][y-1] = GraphObject::up;
            }
        }
        
    }
}


StudentWorld::~StudentWorld()
{
    delete m_player;
    m_player = nullptr;
    
    if (m_actor.size() != 0) //go through the list to delete actors
    {
        for (vector<Actor*>::iterator p = m_actor.begin(); p != m_actor.end();)
        {
            delete *p;
            vector<Actor*>::iterator q = m_actor.erase(p);
            p = q;
        }
    }
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            if (m_dirt[i][j] != nullptr)
            {
                delete m_dirt[i][j];
                m_dirt[i][j] = nullptr;
            }
        }
    }
}



// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp







