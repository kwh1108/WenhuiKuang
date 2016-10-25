//
//  DiskList.cpp
//  project4warmup
//
//  Created by wenhui kuang on 3/2/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include "BinaryFile.h"
#include <cstring>
#include <iostream>
using namespace std;

struct DiskNode {
    char data[256];
    BinaryFile::Offset next;
};

class DiskList
{
public:
    DiskList(const string& filename);
    bool push_front(const char* data);
    bool remove(const char* data);
    void printAll();
private:
    BinaryFile bf;
    string filename;
};

DiskList::DiskList(const string& filename):filename(filename)
{
    if(!bf.createNew(filename))
    {
        cout << "Error! Unable to create " << filename << " ." << endl;
        exit(1);
    }
    BinaryFile::Offset next = sizeof(BinaryFile::Offset);
    bf.write(next, 0);
    bf.close();
    
}

bool DiskList::push_front(const char *data)
{
    bf.openExisting(filename);
    if (strlen(data) >= 256)
    {
        bf.close();
        return false;
    }
    if (bf.fileLength() == 4)
    {
        DiskNode nextNode;
        strcpy(nextNode.data, data);
        nextNode.next = 0;
        bf.write(nextNode, sizeof(BinaryFile::Offset));
    }
    else
    {
        BinaryFile::Offset next = bf.fileLength();
        bf.write(next, 0);
        next = bf.fileLength() - sizeof(DiskNode);
        DiskNode nextNode;
        nextNode.next = next;
        strcpy(nextNode.data, data);
        bf.write(nextNode, bf.fileLength());
    }
    bf.close();
    return true;
}

bool DiskList::remove(const char* data)
{
    bf.openExisting(filename);
    if (bf.fileLength() == 4)
    {
        bf.close();
        return false;
    }
    char* temp = "@#$%";
    bool found = false;
    int count = 0;
    BinaryFile::Offset next;
    bf.read(next, 0);
    DiskNode d;
    bf.read(d, next);
    while(d.next != 0)
    {
        if (strlen(d.data) == strlen(data))
        {
            for (int i = 0; i < strlen(data); i++)
            {
                if (d.data[i] != data[i])
                {
                    found = false;
                    break;
                }
                else
                {
                    found = true;
                    continue;
                }
            }
        }
        if (found)
        {
            count++;
            strcpy(d.data, temp);
            bf.write(d, next);
        }
        found = false;
        next = d.next;
        bf.read(d, d.next);
    }
    if (strlen(d.data) == strlen(data))
    {
        for (int i = 0; i < strlen(data); i++)
        {
            if (d.data[i] != data[i])
            {
                found = false;
                break;
            }
            else
            {
                found = true;
                continue;
            }
        }
    }
    if (found)
    {
        count++;
        strcpy(d.data, temp);
        bf.write(d, next);
    }
    found = false;
    bf.read(d, d.next);
    if (count > 0)
    {
        bf.close();
        return true;
    }
    bf.close();
    return false;
}

void DiskList::printAll()
{
    bool found = false;
    char* temp = "@#$%";
    bf.openExisting(filename);
    if (bf.fileLength() == 4)
    {
        cout << "Empty list." << endl;
    }
    else
    {
        BinaryFile::Offset next;
        bf.read(next, 0);
        DiskNode d;
        bf.read(d, next);
        while(d.next != 0)
        {
            for (int i = 0; i < strlen(d.data); i++)
            {
                if (d.data[i] != temp[i])
                {
                    found = false;
                    break;
                }
                else
                    found = true;

            }
            if (!found)
                cout << d.data << endl;
            bf.read(d, d.next);
        }
        for (int i = 0; i < strlen(d.data); i++)
        {
            if (d.data[i] != temp[i])
            {
                found = false;
                break;
            }
            else
                found = true;
            
        }
        if (!found)
        cout << d.data << endl;
    }
}