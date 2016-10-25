//
//  main.cpp
//  test
//
//  Created by wenhui kuang on 1/18/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

bool combine(const Map& m1, const Map& m2, Map& result)
{
    bool differentValue = false;
    if (&result != &m1 && &result != &m2)
    {
        if (&m1 == &m2)
        {
            Map temp;
            result = temp;
            result = m1;
            return true;
        }
        else
        {
            Map temp;
            Map temp2(m1);
            result = temp;
            KeyType k,k1;
            ValueType v,v1;
            for (int i = 0; i < m2.size(); i++)
            {
                m2.get(i, k, v);
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
    }
    else if (&result != &m1)
    {
        if (m1.empty())
            return true;
        KeyType k,k1;
        ValueType v,v1;
        for (int i = 0; i < m1.size(); i++)
        {
            m1.get(i, k, v);
            bool found = false;
            for (int j = 0; j < m2.size() && !found; j++)
            {
                m2.get(j, k1, v1);
                if (k == k1 && v == v1)
                    found = true;
                if (k == k1 && v != v1)
                {
                    result.erase(k);
                    found = true;
                    differentValue = true;
                }
            }
            if (found == false)
                result.insert(k, v);
        }
        if (differentValue)
            return false;
        return true;
    }
    else if (&result != &m2)
    {
        if (m2.empty())
            return true;
        KeyType k,k1;
        ValueType v,v1;
        for (int i = 0; i < m2.size(); i++)
        {
            m2.get(i, k, v);
            bool found = false;
            for (int j = 0; j < m1.size() && !found; j++)
            {
                m1.get(j, k1, v1);
                if (k == k1 && v == v1)
                    found = true;
                if (k == k1 && v != v1)
                {
                    result.erase(k);
                    found = true;
                    differentValue = true;
                }
            }
            if (found == false)
                result.insert(k, v);
        }
        if (differentValue)
            return false;
        return true;
    }
    return true;
}
void subtract(const Map& m1, const Map& m2, Map& result)
{
    if (&result == &m1 && &m1 == &m2)
    {
        Map temp;
        result = temp;
        return;
    }
    if (&result != &m1 && &result != &m2)
    {
        if (&m1 == &m2)
        {
            Map temp;
            result = temp;
            result = m1;
        }
        else
        {
            if (m1.empty())
            {
                Map temp;
                result = temp;
                return;
            }
            KeyType k;
            ValueType v;
            Map temp;
            result = temp;
            for (int i = 0; i < m1.size(); i++)
            {
                m1.get(i, k, v);
                if (m2.contains(k))
                    break;
                else
                    result.insert(k, v);
            }
        }
    }
    else if (&result != &m1)
    {
        if (m1.empty())
        {
            Map temp;
            result = temp;
            return;
        }
        KeyType k;
        ValueType v;
        Map temp;
        result = temp;;
        for (int i = 0; i < m1.size(); i++)
        {
            m1.get(i, k, v);
            if (m2.contains(k))
                break;
            else
                temp.insert(k, v);
        }
        result = temp;
    }
    else if (&result != &m2)
    {
        if (m2.empty())
        {
            return;
        }
        KeyType k;
        ValueType v;
        for (int i = 0; i < m1.size(); i++)
        {
            m1.get(i, k, v);
            if (m2.contains(k))
            {
                result.erase(k);
                break;
            }
            else
                temp2.insert(k, v);
        }
        result = temp2;
    }
}


