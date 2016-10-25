//
//  circle.h
//  sss
//
//  Created by wenhui kuang on 2/1/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#ifndef __sss__circle__
#define __sss__circle__

#include "Shape.h"
#include <iostream>
#include <stdio.h>
using namespace std;

class circle : public Shape
{
public:
    circle(double x, double y, double r):Shape(x,y)
    {
        cout << "circle" << endl;
        radius = r;
    }
    virtual double getradius()
    { return radius;}
    void print()
    {
        cout << radius << endl;
        
    }
    ~circle()
    {
        cout << "what's up" << endl;
    }
private:
    double radius;

};


#endif /* defined(__sss__circle__) */
