//
//  Disk.cpp
//  Project4
//
//  Created by wenhui kuang on 3/6/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//
#include "DiskMultiMap.h"
#include <cstring>
#include <iostream>
using namespace std;

DiskMultiMap::DiskMultiMap()
{}

DiskMultiMap::~DiskMultiMap()
{
    m_bf.close();
}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
    if(filename == this->filename)
    {
        if (!m_bf.createNew(filename))
            return false;
    }
    else if (m_bf.isOpen())
    {
        close();
    }
    if (!m_bf.createNew(filename))
        return false;
    this->filename = filename;
    head.numOfEmptyNodes = 0;
    // initialize all the offset at the bucket to zero indicating empty bucket.
    // head of hash table
    head.numOfBuckets = numBuckets;
    head.emptyList = 0;
    m_bf.write(head, 0);
    for (int i = 0; i < numBuckets; i++)
    {
        bucket b;
        b.next = 0;
        b.position = i;
        m_bf.write(b, sizeof(head)+sizeof(bucket)*i);
    }
    return true;
}

bool DiskMultiMap::openExisting(const std::string &filename)
{
    if(m_bf.isOpen())
        close();
    if (!m_bf.openExisting(filename))
        return false;
    return true;
}
bool DiskMultiMap::insert(const std::string &key, const std::string &value, const std::string &context)
{
    if (key.size() > 120 || value.size() > 120 || context.size() > 120)
        return false;
    int position = hashFuction(key) % head.numOfBuckets;
    header head;
    m_bf.read(head, 0);
    DiskNode newNode;
    bucket b;
    strcpy(newNode.m_tuple.key,key.c_str());
    strcpy(newNode.m_tuple.value,value.c_str());
    strcpy(newNode.m_tuple.context,context.c_str());
    if (head.numOfEmptyNodes == 0)
    {
        m_bf.read(b,sizeof(head)+sizeof(bucket)*position);
        if (b.next == 0)
            newNode.next = 0;
        else
            newNode.next = b.next;
        b.next = m_bf.fileLength();
        m_bf.write(newNode, m_bf.fileLength());
    }
    else
    {
        m_bf.read(newNode, head.emptyList);
        BinaryFile::Offset next = head.emptyList;
        head.emptyList = newNode.next;
        newNode.next = b.next;
        b.next = next;
        m_bf.write(newNode, next);
        head.numOfEmptyNodes--;
    }
    m_bf.write(head, 0);
    m_bf.write(b,sizeof(head)+sizeof(bucket)*position);
    return true;
}

int DiskMultiMap::erase(const std::string &key, const std::string &value, const std::string &context)
{
    int position = hashFuction(key) % head.numOfBuckets;
    int total = 0,firstToremove = 0,i = 0,j = 0;
    header head;
    bucket b;
    m_bf.read(head, 0);
    m_bf.read(b,sizeof(head)+sizeof(bucket)*position);
    if (b.next == 0)
        return 0;
    DiskNode curr,previous;
    BinaryFile::Offset prev = b.next;
    BinaryFile::Offset next = b.next;
    BinaryFile::Offset temp;
    while (next != 0)
    {
        if (firstToremove > 1 && i != j)
        {
            m_bf.read(previous, prev);
            prev = previous.next;
        }
        i = firstToremove;
        m_bf.read(curr, next);
        string k(curr.m_tuple.key);
        string v(curr.m_tuple.value);
        string c(curr.m_tuple.context);
        temp = curr.next;
        if (k == key && v == value && c == context)
        {
            total++;
            head.numOfEmptyNodes++;
            if (head.emptyList == 0)
                curr.next = 0;
            else
                curr.next = head.emptyList;
            head.emptyList = next;
            if (firstToremove == 0){
                b.next = temp;
            }
            else if (firstToremove >= 1){
                m_bf.read(previous,prev);
                previous.next = temp;
                m_bf.write(previous, prev);
            }
            firstToremove--;
        }
        m_bf.write(curr, next);
        next = temp;
        firstToremove++;
        j = firstToremove;
    }
    m_bf.write(head, 0);
    m_bf.write(b,sizeof(head)+sizeof(bucket)*position);
    return total;
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string &key)
{
    int position = hashFuction(key) % head.numOfBuckets;
    bucket b;
    m_bf.read(b,sizeof(head)+sizeof(bucket)*position);
    Iterator itr;
    if (b.next == 0)
        return itr;
    BinaryFile::Offset next = b.next;
    DiskNode node;
    m_bf.read(node,next);
    while (node.next != 0)
    {
        if (strcmp(node.m_tuple.key, key.c_str()) == 0)
        {
            Iterator itr(&m_bf,next);
            return itr;
        }
        m_bf.read(node,next);
    }
    return itr;
}

void DiskMultiMap::close()
{
    m_bf.close();
}

bool DiskMultiMap::Iterator::isValid() const
{
    return Valid;
}
DiskMultiMap::Iterator::Iterator():bf(nullptr),off_set(0),Valid(false)
{}

DiskMultiMap::Iterator::Iterator(BinaryFile *bf, BinaryFile::Offset off):Valid(true)
{
    this->bf = bf;
    off_set = off;
}

MultiMapTuple DiskMultiMap::Iterator::operator*()
{
    DiskNode node;
    bf->read(node, off_set);
    MultiMapTuple tuple;
    tuple.key = node.m_tuple.key;
    tuple.value = node.m_tuple.value;
    tuple.context = node.m_tuple.context;
    return tuple;
}

DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
{
    DiskNode node,node1;
    bf->read(node, off_set);
    off_set = node.next;
    bf->read(node1, off_set);
    while (off_set != 0)    
    {
        if (strcmp(node.m_tuple.key, node1.m_tuple.key) == 0)
            return *this;
        bf->read(node1,off_set);
        off_set = node1.next;
    }
    if (off_set == 0)
    {
        setValid(false);
        return *this;
    }
    return *this;
}