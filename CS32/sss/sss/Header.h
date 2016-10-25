//
//  Header.h
//  sss
//
//  Created by wenhui kuang on 1/28/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#ifndef sss_Header_h
#define sss_Header_h
#include "Map.h"
int global = 10;

class haha
{
public:
    haha()
    {
        
    };
    void add(int n)
    {
        m.insert(n, n);
    };
    int get()
    {
        return global;
    };
private:
    Map m;

};
#endif
