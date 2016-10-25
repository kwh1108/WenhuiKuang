//
//  newMap.cpp
//  333
//
//  Created by wenhui kuang on 1/16/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include "newMap.h"
#include <iostream>
#include <cstdlib>
#include <string.h>
using namespace std;

Map::Map(int size):pairs(new data[size]), capacity(size), count(0),firstIndex(0)
{
    if (size < 0)
    {
        cout << "The size of the array cannot be negative" << endl;
        exit(1);
    }
}
Map::Map():pairs(new data[DEFAULT_MAX_ITEMS]), capacity(DEFAULT_MAX_ITEMS), count(0),firstIndex(0)
{
    
}

Map::Map(const Map &other)
{
    KeyType k;
    ValueType v;
    capacity = other.capacity;
    pairs = new data[capacity];
    firstIndex = 0;
    count = firstIndex;
    for (int i = 0; i < other.size(); i++)
    {
        other.get(i, k, v);
        insert(k,v);
    }
    count = other.count;
}

Map& Map::operator=(const Map &rhs)
{
    if (this != &rhs)
    {
        Map temp(rhs);
        delete [] pairs;
        KeyType k;
        ValueType v;
        capacity = rhs.capacity;
        pairs = new data[capacity];
        count = firstIndex;
        for (int i = 0; i < rhs.size(); i++)
        {
            rhs.get(i, k, v);
            insert(k,v);
        }
        count = rhs.count;
    }
    return *this;
}
Map::~Map()
{
    delete [] pairs;
}

bool Map::empty() const
{
    if (count == 0)
        return true;
    else
        return false;
}

int Map::size() const
{
    return count;
}

bool Map::insert(const KeyType &key, const ValueType &value)
{
    if (size() >= capacity)
        return false;
    else
    {
        if (contains(key))
            return false;
        pairs[size()].m_key = key;
        pairs[size()].m_value = value;
        count++;
        return true;
    }
}

bool Map::update(const KeyType &key, const ValueType &value)
{
    if (!empty())
    {
        for (int i = 0; i < count; i++)
        {
            if (pairs[i].m_key == key)
            {
                pairs[i].m_value = value;
                return true;
            }
        }
    }
    return false;
}

bool Map::insertOrUpdate(const KeyType &key, const ValueType &value)
{
    if (update(key, value) || insert(key, value))
        return true;
    else
        return false;
}

bool Map::erase(const KeyType &key)
{
    if (!empty())
    {
        bool found = false;
        int itemToRemove = 0;
        for (int i = 0; i < size() && !found ; i++)
        {
            if (pairs[i].m_key == key)
            {
                itemToRemove = i;
                found = true;
            }
        }
        for (int j = itemToRemove; j < size() - 1; j++)
        {
            pairs[j].m_key = pairs[j+1].m_key;
            pairs[j].m_value = pairs[j+1].m_value;
        }
        count--;
        return true;
        
    }
    return false;
}

bool Map::contains(const KeyType& key) const
{
    for (int i = 0; i < size(); i++)
    {
        if (pairs[i].m_key == key)
        {
            return true;
        }
    }
    return false;
}

bool Map::get(const KeyType &key, ValueType &value) const
{
    if (!empty())
    {
        // bool found = false;
        for (int i = 0; i < count; i++)
        {
            if (pairs[i].m_key == key)
            {
                value = pairs[i].m_value;
                return true;
            }
        }
    }
    return false;
}

bool Map::get(int i, KeyType& key, ValueType& value) const
{
    if (i >= 0 && i < size())
    {
        key = pairs[i].m_key;
        value = pairs[i].m_value;
        return true;
    }
    return false;
}
void Map::swap(Map& other)
{
    Map temp;
    temp = other;
    other = *this;
    *this = temp;    
}


