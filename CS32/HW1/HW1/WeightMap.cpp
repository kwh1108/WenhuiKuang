//
//  WeightMap.cpp
//  111
//
//  Created by wenhui kuang on 1/16/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include "Map.h"
#include "WeightMap.h"
#include <iostream>
#include <string>
using namespace std;

WeightMap::WeightMap()
{
    Map M;
}

bool WeightMap::enroll(string name, double startWeight)
{
    if (startWeight >= 0 && M.insert(name, startWeight))
        return true;
    return false;
}

double WeightMap::weight(string name) const
{
    double weight = -1;
    if (M.get(name, weight))
        return weight;
    return weight;
}

bool WeightMap::adjustWeight(string name, double amt)
{
    if (M.contains(name) && (weight(name) + amt >= 0))
    {
        M.update(name, weight(name) + amt);
        return true;
    }
        
    return false;
}

int WeightMap::size() const
{
    return M.size();
}

void WeightMap::print() const
{
    string name;
    double weight;
    for (int i = 0; i < size(); i++)
    {
        M.get(i, name, weight);
        cout << name << " " << weight << endl;
    }
}
WeightMap::~WeightMap()
{
    
}

