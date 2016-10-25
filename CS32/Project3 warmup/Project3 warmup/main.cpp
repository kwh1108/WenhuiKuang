//
//  main.cpp
//  Project3 warmup
//
//  Created by wenhui kuang on 2/10/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>

using namespace std;

void removeOdds(list<int>& li)
{
    if (li.begin() == li.end())
        return;
    list<int>::iterator p = li.begin();
    while (p != li.end())
    {
        if (*p % 2 != 0)
        {
            list<int>::iterator q = li.erase(p);
            p = q;
        }
        else
            p++;
    }
}

void removeOdds(vector<int>& v)
{
    if (v.begin() == v.end())
        return;
    vector<int>::iterator p = v.begin();
    while (p != v.end())
    {
        if (*p % 2 != 0)
        {
            vector<int>::iterator q = v.erase(p);
            p = q;
        }
        else
            p++;
    }
}

vector<int> destroyedOnes;

class Movie
{
public:
    Movie(int r) : m_rating(r) {}
    ~Movie() { destroyedOnes.push_back(m_rating); }
    int rating() const { return m_rating; }
private:
    int m_rating;
};

// Remove the movies in li with a rating below 50 and destroy them.
// It is acceptable if the order of the remaining movies is not
// the same as in the original list.
void removeBad(list<Movie*>& li)
{
    if (li.begin() == li.end())
        return;
    list<Movie*>::iterator p = li.begin();
    while(p != li.end())
    {
        Movie* mp = *p;
        if(mp->rating() < 50)
        {
            delete mp;
            list<Movie*>::iterator q = li.erase(p);
            p = q;
        }
        else
            p++;
    }
}

void removeBad(vector<Movie*>& v)
{
    if (v.begin() == v.end())
        return;
    vector<Movie*>::iterator p = v.begin();
    while(p != v.end())
    {
        if((*p)->rating() < 50)
        {
            delete *p;
            vector<Movie*>::iterator q = v.erase(p);
            p = q;
        }
        else
            p++;
    }

}


void test()
{
    int a[8] = { 85, 80, 30, 70, 20, 15, 90, 10 };
    vector<Movie*> x;
    for (int k = 0; k < 8; k++)
        x.push_back(new Movie(a[k]));
    assert(x.size() == 8 && x.front()->rating() == 85 && x.back()->rating() == 10);
    removeBad(x);
    assert(x.size() == 4 && destroyedOnes.size() == 4);
    vector<int> v;
    for (int k = 0; k < 4; k++)
        v.push_back(x[k]->rating());
    sort(v.begin(), v.end());
    int expect[4] = { 70, 80, 85, 90 };
    for (int k = 0; k < 4; k++)
        assert(v[k] == expect[k]);
    sort(destroyedOnes.begin(), destroyedOnes.end());
    int expectGone[4] = { 10, 15, 20, 30 };
    for (int k = 0; k < 4; k++)
        assert(destroyedOnes[k] == expectGone[k]);
}

int main()
{
    test();
    cout << "Passed" << endl;
}