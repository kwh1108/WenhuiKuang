#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include <iostream>
#include "MultiMapTuple.h"
#include "BinaryFile.h"
using namespace std;

class DiskMultiMap
{
public:
    
    class Iterator
    {
    public:
        Iterator();
        Iterator(BinaryFile *bf, BinaryFile::Offset off);
        bool isValid() const;
        Iterator& operator++();
        MultiMapTuple operator*();
        
    private:
        BinaryFile *bf;
        BinaryFile::Offset off_set;
        bool Valid;
        void setValid(bool x){Valid = x;}
    };
    
    DiskMultiMap();
    ~DiskMultiMap();
    bool createNew(const std::string& filename, unsigned int numBuckets);
    bool openExisting(const std::string& filename);
    void close();
    bool insert(const std::string& key, const std::string& value, const std::string& context);
    Iterator search(const std::string& key);
    int erase(const std::string& key, const std::string& value, const std::string& context);
    void print(const string &key)
    {
        bucket b;
        unsigned int position = hashFuction(key) % head.numOfBuckets;
        m_bf.read(b,sizeof(head)+sizeof(bucket)*position);
        BinaryFile::Offset next = b.next;
        DiskNode node;
        while (next != 0)
        {
            m_bf.read(node, next);
            cout << node.m_tuple.key << endl;
            cout << node.m_tuple.value << endl;
            cout << node.m_tuple.context << endl;
            next = node.next;
        }
    }
private:
    struct header
    {
        unsigned int numOfBuckets;
        BinaryFile::Offset emptyList;
        int numOfEmptyNodes;
    };
    struct MapTuple
    {
        char key[121];
        char value[121];
        char context[121];
    };
    struct DiskNode {
        MapTuple m_tuple;
        BinaryFile::Offset next;
    };
    struct bucket
    {
        int position;
        BinaryFile::Offset next;
    };
    string filename;
    BinaryFile m_bf;
    header head;
    unsigned int hashFuction(const string &str)
    {
        unsigned int h = 2166136261U;
        for (int k = 0; k < str.size(); k++)
        {
            h += str[k];
            h *= 16777619;
        }
        return h;
    }
};

#endif // DISKMULTIMAP_H_