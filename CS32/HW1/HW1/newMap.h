//
//  newMap.h
//  333
//
//  Created by wenhui kuang on 1/16/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#ifndef NEWMAP_INCLUDE
#define NEWMAP_INCLUDE

#include <stdio.h>
#include <string>
using namespace std;

typedef string KeyType;
typedef double ValueType;

const int DEFAULT_MAX_ITEMS = 200;

class Map
{
public:
    Map(int size);
    Map();
    Map(const Map &other);
    Map& operator=(const Map& rhs);
    ~Map();
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
    data *pairs;
    int count;
    int capacity;
    int firstIndex;
};


#endif /* defined(___33__newMap__) */
