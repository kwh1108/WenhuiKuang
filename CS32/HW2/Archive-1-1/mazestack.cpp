//
//  main.cpp
//  HW2
//
//  Created by wenhui kuang on 1/29/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include <iostream>
#include <stack>
using namespace std;


class Coord
{
public:
    Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
    int r() const { return m_r; }
    int c() const { return m_c; }
private:
    int m_r;
    int m_c;
};


bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec)
{
    if (sr < 1 || sr > nRows - 2 || sc < 1 || sc > nCols - 2 || er < 1 || er > nRows - 2 || ec < 1 || ec > nCols - 2)
        return true;
    stack<Coord> coorkStack;
    Coord point(sr,sc);
    coorkStack.push(point);
    maze[sr][sc] = '#';
    
    while (!coorkStack.empty())
    {
        int r, c;
        Coord point2 = coorkStack.top();
        r = point2.r();
        c = point2.c();
        coorkStack.pop();
        if (r == er && c == ec)
            return true;
        if (maze[r-1][c] == '.')
        {
            coorkStack.push(Coord(r-1,c));
            maze[r-1][c] = '#';
        }
        if (maze[r][c+1] == '.')
        {
            coorkStack.push(Coord(r,c+1));
            maze[r][c+1] = '#';
        }
        if (maze[r+1][c] == '.')
        {
            coorkStack.push(Coord(r+1,c));
            maze[r+1][c] = '#';
        }
        if (maze[r][c-1] == '.')
        {
            coorkStack.push(Coord(r,c-1));
            maze[r][c-1] = '#';
        }
    }
    return false;

}

int main()
{
    string maze[10] = {
        "XXXXXXXXXX",
        "X........X",
        "XX.X.XXXXX",
        "X..X.X...X",
        "X..X...X.X",
        "XXXX.XXX.X",
        "X.X....XXX",
        "X..XX.XX.X",
        "X...X....X",
        "XXXXXXXXXX"
    };
    
    if (pathExists(maze, 10,10, 6,4, 1,1))
        cout << "Solvable!" << endl;
    else
        cout << "Out of luck!" << endl;
    return 0;
}
