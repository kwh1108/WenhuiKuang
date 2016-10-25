//
//  Map.cpp
//  HW2
//
//  Created by wenhui kuang on 1/20/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include "Map.h"


Map::Map()
{
    head = nullptr;
    tail = nullptr;
    count = 0;
}
Map::Map(const Map &other)
{
    if (other.empty())
    {
        head = nullptr;
        tail = nullptr;
        count = 0;
    }
    else
    {
        KeyType k;
        ValueType v;
        count = 0;
        for (int i = 0; i < other.count; i++)
        {
            other.get(i, k, v);
            insert(k, v);
        }
    count = other.count;
    }
}

Map& Map::operator=(const Map& rhs)
{
    if (this != &rhs)
    {
        Map temp(rhs);
        swap(temp);
    }
    return *this;
}

Map::~Map()
{
    Node *NodeToRemove;
    while (head != nullptr)
    {
        NodeToRemove = head;
        head = head->next;
        delete NodeToRemove;
    }
    tail = nullptr;
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
/*    if (!contains(key))
    {
        Node *newNode = new Node;
        newNode->m_key = key;
        newNode->m_value = value;
        newNode->next = nullptr;
        newNode->prev = tail;
        if (head == nullptr)
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
        return true;
    }
    return false;
*/
    if (empty())
    {
        Node *newNode = new Node;
        head = newNode;
        tail = head;
        head->m_key = key;
        head->m_value = value;
        head->next = nullptr;
        head->prev = nullptr;
        count++;
        return true;
    }
    else if (!contains(key))
    {
        Node *newNode = new Node;
        newNode->m_key = key;
        newNode->m_value = value;
        newNode->prev = tail;
        tail->next = newNode;
        tail = newNode;
        tail->next = nullptr;
        count++;
        return true;
    }
    return false;
    
}

bool Map::update(const KeyType &key, const ValueType &value)
{
    if (empty() || !contains(key))
        return false;
    else
    {
        Node *curr = head;
        while (curr->m_key != key)
            curr = curr->next;
        curr->m_value = value;
    }
    return true;
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
    if (!empty() && contains(key))
    {
        if (count == 1)
        {
            delete head;
            head = nullptr;
            tail = nullptr;
            count--;
            return true;
        }
        Node *curr = head;
        while (curr->m_key != key)
            curr = curr->next;
        if (curr == head)
        {
            head = head->next;
            delete curr;
            head->prev = nullptr;
            
        }
        else if (curr == tail)
        {
            tail = tail->prev;
            delete curr;
            tail->next = nullptr;
        }
        else
        {
            curr->prev->next = curr->next;
            curr->next->prev = curr->prev;
            delete curr;
        }
        count--;
        return true;
    }
    return false;
}

bool Map::contains(const KeyType& key) const
{
    for (Node *curr = head; curr != nullptr; curr = curr->next)
    {
        if (curr->m_key == key)
            return true;
    }
    return false;
}

bool Map::get(const KeyType &key, ValueType &value) const
{
    if (empty() || !contains(key))
        return false;
    else
    {
        Node *curr = head;
        while (curr->m_key != key)
            curr = curr->next;
        value = curr->m_value;
        return true;
    }
}

bool Map::get(int i, KeyType& key, ValueType& value) const
{
    if (i >= 0 && i < size() && !empty())
    {
        Node *curr = head;
        while (i > 0)
        {
            curr = curr->next;
            i--;
        }
        key = curr->m_key;
        value = curr->m_value;
        return true;
    }
    return false;
}
void Map::swap(Map& other)
{
    Node *temp1 = head;
    Node *temp2 = tail;
    int temp_size = count;
    head = other.head;
    other.head = temp1;
    tail = other.tail;
    other.tail = temp2;
    count = other.count;
    other.count = temp_size;
}

bool combine(const Map& m1, const Map& m2, Map& result)
{
    if (m1.empty())
    {
        result = m2;
        return true;
    }
    if (m2.empty())
    {
        result = m1;
        return true;
    }
    bool differentValue = false;
    Map temp1(m1);   //create two tempory Maps to hold m1 and m2 to prevent alasing
    Map temp2(m2);
    Map temp3;
    result = temp3;
    KeyType k,k1;
    ValueType v,v1;
    for (int i = 0; i < temp1.size(); i++)
    {
        temp1.get(i, k, v);
        bool found = false;
        for (int j = 0; j < temp2.size() && !found; j++)
        {
            temp2.get(j, k1, v1);
            if (k == k1 && v == v1)
            {
                temp2.erase(k);
                result.insert(k,v);
                found = true;
            }
            if (k == k1 && v != v1)
            {
                temp2.erase(k);
                found = true;
                differentValue = true;
            }
        }
        if (found == false)
            result.insert(k, v);
    }
    for (int i = 0; i < temp2.size(); i++)
    {
        temp2.get(i, k, v);
        result.insert(k, v);
    }
    if (differentValue)
        return false;
    return true;
}
void subtract(const Map& m1, const Map& m2, Map& result)
{
    if (m1.empty())
    {
        result = m1;
        return;
    }
    Map temp1(m1);
    Map temp2(m2);
    KeyType k;
    ValueType v;
    Map temp;
    result = temp;
    for (int i = 0; i < temp1.size(); i++)
    {
        temp1.get(i, k, v);
        if (temp2.contains(k))
            break;
        else
            result.insert(k, v);
    }

}