//
//  IntelWeb.cpp
//  Project4
//
//  Created by wenhui kuang on 3/8/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include "IntelWeb.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <queue>
#include <set>

IntelWeb::IntelWeb()
{}

IntelWeb::~IntelWeb()
{
    close();
}

void IntelWeb::close()
{
    forward.close();
    backward.close();
}

bool IntelWeb::createNew(const std::string &filePrefix, unsigned int maxDataItems)
{
    if(!forward.createNew(filePrefix + "DiskMap1", maxDataItems))
        return false;
    if(!backward.createNew(filePrefix + "DiskMap2", maxDataItems))
    {
        forward.close();
        return false;
    }
    return true;
}

bool IntelWeb::openExisting(const std::string &filePrefix)
{
    if(!forward.openExisting(filePrefix + "DiskMap1"))
        return false;
    if(!backward.openExisting(filePrefix + "DiskMap2"))
    {
        forward.close();
        return false;
    }
    return true;
}

bool IntelWeb::ingest(const std::string& telemetryFile)
{
    ifstream file(telemetryFile);
    string line;
    while(getline(file,line))
    {
        istringstream iss(line);
        string key,value,context;
        if(! (iss >> context >> value >> key))
        {
            cerr<<"Ignoring line due to bad input format" << line;
        }
        if(forward.insert(key, value, context) == false)
        {
            cerr<<"Error in inserting into file" << endl;
            return false;
        }
        if(backward.insert(value, key, context) == false)
        {
            cerr<<"Error in inserting into file" << endl;
            return false;
        }
    }
    return true;
}

unsigned int IntelWeb::crawl(const std::vector<std::string> &indicators, unsigned int minPrevalenceToBeGood, std::vector<std::string> &badEntitiesFound, std::vector<InteractionTuple> &badInteractions)
{
    queue<std::string> malicious;
    set<std::string> discovered;
    for (int i = 0; i < indicators.size();i++)
    {
        malicious.push(indicators[i]);
        Prevalence(indicators[i]);
    }
    
    while (!malicious.empty())
    {
        DiskMultiMap::Iterator a = forward.search(malicious.front());
        DiskMultiMap::Iterator b = backward.search(malicious.front());
        while (a.isValid() && popularMap[malicious.front()] < minPrevalenceToBeGood)
        {
            discovered.insert((*a).key);
            ++a;
        }
        while (b.isValid() && popularMap[malicious.front()] < minPrevalenceToBeGood)
        {
            discovered.insert((*b).key);
            ++b;
        }
        malicious.pop();
    }
    
    badEntitiesFound.clear();
    badInteractions.clear();
    
    set<std::string>::iterator it;
    for (it = discovered.begin(); it != discovered.end(); ++it)
    {
        badEntitiesFound.push_back(*it);
    }
    for (int i = 0; i < badEntitiesFound.size(); i++)
    {
        DiskMultiMap::Iterator p = forward.search(badEntitiesFound[i]);
        while (p.isValid()) {
            badInteractions.push_back(InteractionTuple((*p).key,(*p).value,(*p).context));
        }
        p = backward.search(badEntitiesFound[i]);
        while (p.isValid()) {
            badInteractions.push_back(InteractionTuple((*p).key,(*p).value,(*p).context));
        }
    }
    return static_cast<int>(badEntitiesFound.size());
    
}

bool IntelWeb::purge(const std::string& entity)
{
    DiskMultiMap::Iterator a = forward.search(entity);
    DiskMultiMap::Iterator b = backward.search(entity);
    bool hasToRemoved = a.isValid();
    while (a.isValid()) {
        MultiMapTuple tuple = *a;
        forward.erase(tuple.key, tuple.value, tuple.context);
        ++a;
    }
    if (!hasToRemoved)
        hasToRemoved = b.isValid();
    while (b.isValid()) {
        MultiMapTuple tuple = *a;
        backward.erase(tuple.key, tuple.value, tuple.context);
        ++b;
    }
    
    return hasToRemoved;
}