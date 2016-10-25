//
//  main.cpp
//  111
//
//  Created by wenhui kuang on 1/16/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include "WeightMap.h"
#include <assert.h>
#include <iostream>
using namespace std;

int main()
{
    WeightMap m;
    ValueType weight,weight1,weight2;
    m.enroll("kuang", 150.4);
    m.enroll("lei", 99.7);
    m.enroll("kuang", 150.4);
    weight = m.weight("kuang");
    weight1 = m.weight("lei");
    weight2 = m.weight("lu");
    assert(m.size() == 2 && weight == 150.4 && weight1 == 99.7 && weight2 == -1);
    
    cout << "The first test passes" << endl;
    
    KeyType name = "kuang";
    m.adjustWeight(name, -160.4);
    m.adjustWeight("lei", 20);
    m.print();
    assert(m.weight("kuang") == 150.4 && m.weight("lei")== 119.7);
    
    WeightMap m1;
    m1.enroll("lu", 210.8);
    m1.enroll("fei", 120.9);
    m1.enroll("zhou", 390.4);
    m1.print();

    cout << "After the call of copy constructor which is generated by the compiler" << endl;
    
    WeightMap m3(m1);
    m3.print();
    assert(m3.weight("lu") == 210.8 && m3.weight("fei") == 120.9 && m3.weight("zhou") == 390.4);
    
    cout << "m1 and m2 have the same content which means the copy constructor do the right thing" << endl;
    WeightMap m2;
    m2 = m1;
    m2.print();
    assert(m2.weight("lu") == 210.8 && m2.weight("fei") == 120.9 && m2.weight("zhou") == 390.4);
    
    
    
    
    cout << "The assignment operator also does the right thing" << endl;
    return 0;
    
}
