//
//  main.cpp
//  HW-3
//
//  Created by wenhui kuang on 1/29/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//
#include "Map.h"
#include <iostream>
#include <string>
#include <stack>

using namespace std;

int precedence(char letter);

bool isInfix(string &infix);

bool matchLetterInMap(string &postfix, const Map & values);

int evaluate(string infix, const Map& values, string& postfix, int& result);

void convertToPostfix (string &infix, string& postfix);

int evaluate(string infix, const Map& values, string& postfix, int& result)
{
    stack<int> operands;
    int operand1, operand2;
    postfix = "";
    int temp = result;
    if (!isInfix(infix))
        return 1;
    convertToPostfix(infix, postfix);
    if (!matchLetterInMap(postfix, values))
        return 2;
    for (int i = 0; i < postfix.size(); i++)
    {
        if (isalpha(postfix[i]))
        {
            //string temp(&postfix[i]);
            values.get(postfix[i], result);
            operands.push(result);
        }
        else if (postfix[i] == '+')
        {
            operand2 = operands.top();
            operands.pop();
            operand1 = operands.top();
            operands.pop();
            result = operand1 + operand2;
            operands.push(result);
        }
        else if (postfix[i] == '-')
        {
            operand2 = operands.top();
            operands.pop();
            operand1 = operands.top();
            operands.pop();
            result = operand1 - operand2;
            operands.push(result);
        }
        else if (postfix[i] == '*')
        {
            operand2 = operands.top();
            operands.pop();
            operand1 = operands.top();
            operands.pop();
            result = operand1 * operand2;
            operands.push(result);
        }
        else if (postfix[i] == '/')
        {
            operand2 = operands.top();
            if (operand2 == 0)
            {
                result = temp;
                return 3;
            }
            operands.pop();
            operand1 = operands.top();
            operands.pop();
            result = operand1 / operand2;
            operands.push(result);
        }
    }
    return 0;
}

int precedence(char letter)
{
    if (letter == '/' || letter == '*')
        return 1;
    else
        return 0;
}

bool matchLetterInMap(string &postfix, const Map& values)
{
    for (int i = 0; i < postfix.size(); i++)
    {
        if (postfix[i] == '*' || postfix[i] == '-' || postfix[i] == '/' || postfix[i] == '+' )
            break;
        //string temp(&postfix[i]);
        if (!values.contains(postfix[i]))
                return false;
    }
    return true;
}

bool isInfix(string &infix)
{
    int letterCount = 0;
    int openParen = 0;
    int closeParen = 0;
    int operatorCount = 0;
    if (infix == "")
        return false;
    for (int i = 0; i < infix.size(); i++)
    {
        if (infix[i] == ' ')
            ;
        else if (infix[i] >= 'a' && infix[i] <= 'z')
            letterCount++;
        else if (infix[i] == '(')
            openParen++;
        else if (infix[i] == ')')
            closeParen++;
        else if (infix[i] == '*' || infix[i] == '-' || infix[i] == '/' || infix[i] == '+' )
            operatorCount++;
        else
            return false;
    }
    if (openParen == closeParen && letterCount - operatorCount == 1)
        return true;
    else
        return false;
}

void convertToPostfix (string &infix, string& postfix)
{
    stack<char> operator1;
    for (int i = 0; i < infix.size(); i++)
    {
        if (infix[i] >= 'a' && infix[i] <= 'z')
            postfix += infix[i];
        else if (infix[i] == '(')
            operator1.push(infix[i]);
        else if (infix[i] == ')')
        {
            while ( operator1.top() != '(')
            {
                postfix += operator1.top();
                operator1.pop();
            }
            operator1.pop();
        }
        else if (infix[i] == '*' || infix[i] == '-' || infix[i] == '/' || infix[i] == '+' )
        {
            while (!operator1.empty() && operator1.top() != '(' && (precedence(infix[i]) <= precedence(operator1.top())))
            {
                postfix += operator1.top();
                operator1.pop();
            }
            operator1.push(infix[i]);
        }
        else
            postfix += "";
    }
    while (!operator1.empty())
    {
        postfix += operator1.top();
        operator1.pop();
    }
}



