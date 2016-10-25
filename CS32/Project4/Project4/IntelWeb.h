//
//  IntelWeb.h
//  Project4
//
//  Created by wenhui kuang on 3/8/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "DiskMultiMap.h"
#include "InteractionTuple.h"
#include <string>
#include <vector>
#include <map>

class IntelWeb
{
public:
    IntelWeb();
    ~IntelWeb();
    bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
    bool openExisting(const std::string& filePrefix);
    void close();
    bool ingest(const std::string& telemetryFile);
    unsigned int crawl(const std::vector<std::string>& indicators,
                       unsigned int minPrevalenceToBeGood,
                       std::vector<std::string>& badEntitiesFound,
                       std::vector<InteractionTuple>& badInteractions
                       );
    bool purge(const std::string& entity);
    
private:
    DiskMultiMap forward;
    DiskMultiMap backward;
    map<std::string, int> popularMap;
    void Prevalence(const std::string key)
    {
        int count = 0;
        DiskMultiMap::Iterator it = forward.search(key);
        while (it.isValid()) {
            count++;
            popularMap.insert(make_pair((*it).key, count));
            ++it;
        }
        it = backward.search(key);
        while (it.isValid()) {
            count++;
            popularMap.insert(make_pair((*it).key, count));
            ++it;
        }
    }
};

#endif // INTELWEB_H_