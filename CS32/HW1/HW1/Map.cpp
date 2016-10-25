//
//  Map.cpp
//  222
//
//  Created by wenhui kuang on 1/16/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include "Map.h"


Map::Map()
{
    count = 0;
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
    if (size() >= DEFAULT_MAX_ITEMS)
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
    int biggerSize, tempSize;
    if (size() > other.size())
        biggerSize = size();
    else
        biggerSize = other.size();
    for (int i = 0; i < biggerSize; i++)
    {
        KeyType tempKey;
        ValueType tempValue;
        tempKey = pairs[i].m_key;
        tempValue = pairs[i].m_value;
        pairs[i].m_key = other.pairs[i].m_key;
        pairs[i].m_value = other.pairs[i].m_value;
        other.pairs[i].m_key = tempKey;
        other.pairs[i].m_value = tempValue;
        
    }
    tempSize = other.size();
    other.count = size();
    count = tempSize;
}


