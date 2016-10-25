//
//  main.cpp
//  sss
//
//  Created by wenhui kuang on 1/24/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include "circle.h"
#include "Rect.h"
#include "Shape.h"
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

void f(Shape &s);

int main()
{
    ostringstream s;
    s.fill('0');
    int score = 100;
    int lev = 10;
    int life = 3;
    int health = 100;
    ostringstream ss;
    ss.fill(' ');
    s << "scr: " << setw(7) << score;
    ss << "  lev: " << setw(2) << lev << "  Lives: " << life << "  Hlth: " << setw(3) << health << "%" << endl;
    cout << s.str() << ss.str();
}

void f(Shape &s)
{
    s.print();
}