//
//  Map.h
//  222
//
//  Created by wenhui kuang on 1/16/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#ifndef MAP_INCLUDE
#define MAP_INCLUDE


#include <stdio.h>
#include <string>
using namespace std;

typedef int KeyType;
typedef int ValueType;

const int DEFAULT_MAX_ITEMS = 200;



class Map
{
public:
    Map();
    bool empty() const;
    int size() const;
    bool insert(const KeyType& key, const ValueType& value);
    bool update(const KeyType& key, const ValueType& value);
    bool insertOrUpdate(const KeyType& key, const ValueType& value);
    bool erase(const KeyType& key);
    bool contains(const KeyType& key) const;
    bool get(const KeyType& key, ValueType& value) const;
    bool get(int i, KeyType& key, ValueType& value) const;
    void swap(Map& other);
private:
    struct data
    {
        KeyType m_key;
        ValueType m_value;
    };
    data pairs[DEFAULT_MAX_ITEMS];
    int count;
};

#endif /* defined(__HW1__map__) */

