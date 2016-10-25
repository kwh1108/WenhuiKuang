//
//  Rect.h
//  sss
//
//  Created by wenhui kuang on 1/31/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#ifndef __sss__Rect__
#define __sss__Rect__

#include "Shape.h"
#include <iostream>
#include <stdio.h>
using namespace std;

class Rect : public Shape
{
public:
    Rect(double x, double y, double l, double w):Shape(x,y)
    {
        cout << "Rect" << endl;
        length = l;
        width = w;
    }
    virtual double getLength()
    { return length;}
    ~Rect()
    {
        cout << "hello" << endl;
    }
private:
    double length;
    double width;
};

#endif /* defined(__sss__Rect__) */
