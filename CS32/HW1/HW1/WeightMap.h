//
//  WeightMap.h
//  111
//
//  Created by wenhui kuang on 1/16/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#ifndef WEIGHTMAP_INCLUDE
#define WEIGHTMAP_INCLUDE

#include <iostream>
#include "Map.h"
using namespace std;

class WeightMap
{
public:
    WeightMap();   // Create an empty weight map.
  //  WeightMap (const WeightMap &other);
  //  WeightMap& operator=(const WeightMap &other);
    bool enroll(std::string name, double startWeight);
    double weight(std::string name) const;
    bool adjustWeight(std::string name, double amt);
    int size() const;  // Return the number of people in the WeightMap.
    void print() const;
    ~WeightMap();
private:
    Map M;
};

#endif /* defined(___11__WeightMap__) */
