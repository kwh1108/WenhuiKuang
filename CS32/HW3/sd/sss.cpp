//
//  sss.cpp
//  sd
//
//  Created by wenhui kuang on 2/4/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//
#include <iostream>
using namespace std;

int main()
{
    string s = "kuang";
    string q = "wen";
    string j = "hui";
    char *w[3];
    w[0] = &s[0];
    w[1] = &q[0];
    w[2] = &j[0];
    cout << w[0][0];
}
