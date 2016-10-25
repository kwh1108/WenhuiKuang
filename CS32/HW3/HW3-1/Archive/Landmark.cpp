//
//  main.cpp
//  HW3
//
//  Created by wenhui kuang on 2/3/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//
#include <string>
#include <iostream>
using namespace std;

class Landmark
{
public:
    Landmark(string name)
    {
        m_name = name;
    }
    string name() const
    {
        return m_name;
    }
    virtual string icon() const = 0;
    virtual string color() const
    {
        return "yellow";
    }
    virtual ~Landmark()
    {
        cout << m_name << endl;
    }
private:
    string m_name;
};

class Hotel : public Landmark
{
public:
    Hotel(string n): Landmark(n)
    {}
    virtual string icon() const
    {
        return "bed icon";
    }
    virtual ~Hotel()
    {
        cout << "Destroying the Hotel ";
    }
};

class Restaurant : public Landmark
{
public:
    Restaurant(string n, int capacity):Landmark(n)
    {
        m_capacity = capacity;
    }
    virtual string icon() const
    {
        if (m_capacity < 40)
            return "small knife/fork";
        else
            return "large knife/fork";
    }
    virtual ~Restaurant()
    {
        cout << "Destroying the Restaurant ";
    }
private:
    int m_capacity;
};

class Hospital:public Landmark
{
public:
    Hospital(string n):Landmark(n)
    {}
    virtual string icon() const
    {
        return "H icon";
    }
    virtual string color() const
    {
        return "blue";
    }
    virtual ~Hospital()
    {
        cout << "Destroying the Hospital ";
    }
};
