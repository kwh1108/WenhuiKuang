//
//  main.cpp
//  HW4-1
//
//  Created by wenhui kuang on 2/26/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

void listAll(const Class* c, string path)  // two-parameter overload
{
    cout << path + c->name() << endl;
    if ((c->subclasses()).empty())
        return;
    for (int i = 0; i < (c->subclasses()).size(); i++)
    {
        listAll((c->subclasses())[i], path + c->name() + "=>");
    }
    
}
