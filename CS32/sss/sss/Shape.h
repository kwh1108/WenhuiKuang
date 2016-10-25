//
//  Shape.h
//  sss
//
//  Created by wenhui kuang on 1/31/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#ifndef __sss__Shape__
#define __sss__Shape__

#include <iostream>
#include <stdio.h>
using namespace std;

class Shape
{
public:
    Shape(double x, double y)
    {
        cout << "Shape" << endl;
        m_y = y;
        m_x = x;
    }
    void move(double a, double b)
    {
        m_x += a;
        m_y += b;
    }
    void print()
    {
        cout << m_x << endl;
        cout << m_y << endl;
    }
    virtual~Shape()
    {
        cout << "hi" << endl;
    }
private:
    double m_x;
    double m_y;
};

#endif /* defined(__sss__Shape__) */
