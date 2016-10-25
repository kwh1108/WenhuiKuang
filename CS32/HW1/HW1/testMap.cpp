//
//  main.cpp
//  HW1
//
//  Created by wenhui kuang on 1/15/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include "Map.h"
#include <assert.h>
#include <string>
#include <iostream>
using namespace std;

int main()
{
    Map gpas;
    gpas.insert("Fred", 2.956);
    gpas.insert("Ethel", 3.538);
    ValueType v;
    KeyType k1;
    assert(gpas.get(1,k1,v)  &&  (k1 == "Fred"  ||  k1 == "Ethel"));
    KeyType k2;
    assert(gpas.get(1,k2,v)  &&  k2 == k1);
    
    
    Map gpas1;
    gpas1.insert("Fred", 2.956);
    assert(!gpas1.contains(""));
    gpas1.insert("Ethel", 3.538);
    gpas1.insert("", 4.000);
    gpas1.insert("Lucy", 2.956);
    assert(gpas1.contains(""));
    gpas1.erase("Fred");
    assert(gpas1.size() == 3  &&  gpas1.contains("Lucy")  &&  gpas1.contains("Ethel")  &&
           gpas1.contains(""));
    
    cout << "The first test passes" << endl;
    
    Map m1;
    m1.insert("Fred", 2.956);
    Map m2;
    m2.insert("Ethel", 3.538);
    m2.insert("Lucy", 2.956);
    m1.swap(m2);
    assert(m1.size() == 2  &&  m1.contains("Ethel")  &&  m1.contains("Lucy")  &&
           m2.size() == 1  &&  m2.contains("Fred"));
    
    cout << "The second test passes" << endl;
    
    Map m;  // maps strings to doubles
    assert(m.empty());
    ValueType V = -1234.5;
    assert( !m.get("abc", V)  &&  V == -1234.5); // v unchanged by get failure
    m.insert("xyz", 9876.5);
    m.insert("cde",444.3);
    m.update("cde", 333.3);
 //   assert(m.size() == 2);
    KeyType k = "hello";
    assert(m.get(0, k, V)  &&  k == "xyz"  &&  V == 9876.5);
  //  assert(m.get(1, k, V)  &&  k == "cde"  &&  V == 333.3);
    
    cout << "Passed all the default tests" << endl;
    
    
    //Customized test cases below
    
    Map test;
    
    assert(test.empty());
    assert(test.size() == 0);
    
    KeyType name = "apple";
    ValueType value = 3.0;
    
    test.insert(name, value);
    assert(test.size() == 1);
    assert(!test.empty());
    
    
    
    return 0;
}
