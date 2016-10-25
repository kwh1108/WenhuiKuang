//
//  main.cpp
//  project4warmup
//
//  Created by wenhui kuang on 3/2/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include "DiskList.h"
#include <iostream>
using namespace std;

int main()
{
    DiskList x("mylist.dat");
    x.push_front("Lucy");
    x.push_front("Lucy");
    x.push_front("Ethel");
    x.push_front("Ethel");
    x.push_front("Lucy");
    x.push_front("Fred");
    x.push_front("Ethel");
    x.push_front("Ricky");
    x.push_front("Lucy");
    x.remove("Lucy");
    x.printAll();
}
