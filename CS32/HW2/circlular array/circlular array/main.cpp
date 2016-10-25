//
//  main.cpp
//  circlular array
//
//  Created by wenhui kuang on 1/31/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//
#include "Circular array.h"
#include <iostream>
using namespace std;
int main()
{
    CircularArray array(5);
    array.insert(5);
    array.insert(4);
    array.insert(3);
    array.insert(2);
    array.insert(1);
    cout << array.getTail() << endl;
    array.erase(5);
    array.erase(4);
    array.erase(3);
    array.erase(2);
    array.erase(1);
    cout << array.getHead() << endl;
    return 0;
}
