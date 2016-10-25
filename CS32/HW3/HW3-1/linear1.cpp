//
//  linear1.cpp
//  HW3-1
//
//  Created by wenhui kuang on 2/8/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include <iostream>
using namespace std;

bool anyTrue(const double a[], int n)
{
    if (n <= 0)
        return false;
    if (somePredicate(a[0]))
        return true;
    return anyTrue(a + 1, n - 1);
}
int countTrue(const double a[], int n)
{
    int count = 0;
    if (n <= 0)
        return 0;
    if (somePredicate(a[0]))
        count++;
    return count + countTrue(a + 1, n - 1);
}

int firstTrue(const double a[], int n)
{
    if (n <= 0)
        return -1;
    if (somePredicate(a[0]))
        return 0;
    int index = firstTrue(a + 1, n - 1);
    if (index == -1)
        return -1;
    else
        return ++index;
}
int indexOfMin(const double a[], int n)
{
    if (n <= 0)
        return -1;
    if (n == 1)
        return 0;
    int min = indexOfMin(a + 1, n - 1);
    if (a[0] <= a[min+1])
        return 0;
    else
        return min + 1;   // this is to remember the index of the bigger element.
}

bool includes(const double a1[], int n1, const double a2[], int n2)
{
    /*
     int i = 0, j = 0;
     while(n1 > 0 && n2 > 0)
     {
     if (a1[i] == a2[j])
     {
     n1--;
     n2--;
     i++;
     j++;
     }
     else
     {
     n1--;
     i++;
     }
     }
     if (n2 <= 0)
     return true;
     return false;
     */
    if (n2 <= 0)
        return true;
    if (n1 <= 0)
        return false;
    if (*a1 == *a2)
        return includes(a1 + 1, n1 - 1, a2 + 1, n2 - 1);
    else
        return includes(a1 + 1, n1 - 1, a2, n2);
    
}


