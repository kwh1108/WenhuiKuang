//
//  Circular array.h
//  circlular array
//
//  Created by wenhui kuang on 1/31/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#ifndef __circlular_array__Circular_array__
#define __circlular_array__Circular_array__

#include <iostream>
typedef int keytype;
class CircularArray
{
public:
    CircularArray(int s)
    {
        size = posHead = posTail = 0;
        this->capacity = s;
        head = new keytype[s];
        tail = head;
        first = head;
        last = &head[capacity - 1];
    }
    bool empty()
    {
        if (size == capacity)
            return false;
        else
            return true;
    }
    bool insert(keytype n)
    {
        if (size != capacity)
        {
            if (posTail == capacity - 1)
                posTail %= (capacity - 1);
            else
                posTail++;
            *tail = n;
            if (tail == last)
                tail = first;
            else
                tail++;
            size++;
            return true;
        }
        return false;
    }
    bool erase(keytype n)
    {
        if (size == 0 || !contains(n))
            return false;
        if (head == last)
            head = first;
        else
            head++;
        size--;
        return true;
        
    }
    bool contains(keytype n)
    {
        
        int *temp = head;
        for (int i = 0; i < size; i++)
        {
            if (*temp == n)
                return true;
            if (temp == last)
                temp = first;
            else
                temp++;
        }
        return false;
    }
    int getTail()
    {
        return *tail;
    }
    int getHead()
    {
        return *head;
    }
    ~CircularArray()
    {
        delete [] first;
    }
private:
    int size, capacity,posHead,posTail;
    keytype *head;
    keytype *tail;
    keytype *first;
    keytype *last;
};

#endif /* defined(__circlular_array__Circular_array__) */
