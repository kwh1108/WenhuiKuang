//
//  aa.cpp
//  useful function
//
//  Created by wenhui kuang on 2/8/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

//Recursion:
// to determine p is prime number or not
#include <iostream>
using namespace std;
const int MAX_SIZE = 100;
bool isPrime(int p, int i=2)
{
    if (i==p) return 1;//or better  if (i*i>p) return 1;
    if (p%i == 0) return 0;
    return isPrime (p, i+1);
}

void reverse(char *s, int length)
{
    if (length > 1)
    {
        char temp = *s;
        char e = *(s+1);
        *s = *(s+length-1);
        *(s+length-1) = temp;
        reverse(s+1, length-2);
    }
}
// Mergesort
void merge(int a[], int m, int s, int e)
{
    int b[MAX_SIZE];
    int index = s;
    int first1 = s;
    int last1 = m;
    int first2 = m;
    int last2 = e;
    while (first1 < last1 && first2 < last2)
    {
        if (a[first1] < a[first2])
        {
            b[index] = a[first1];
            first1++;
        }
        else
        {
            b[index] = a[first2];
            first2++;
        }
        index++;
    }
    while (first1 < last1)
    {
        b[index] = a[first1];
        first1++;
        index++;
    }
    while (first2 < last2)
    {
        b[index] = a[first2];
        first2++;
        index++;
    }
    for(index = s; index < e; index++)
    {
        a[index] = b[index];
    }
    
}
void mergeSort(int a[], int s, int e)
{
    if (e-s >=2)
    {
        int m = (s+e)/2;
        mergeSort(a, s, m);
        mergeSort(a, m, e);
        merge(a,m,s,e);
    }
}

//recursion to reverse singly linked list
Node *reverse (Node *node)
{
    if (node == nullptr)
        return node;
    if (node->next == nullptr)
        return node;
    Node *rest = node->next;
    node->next = nullptr;
    Node *head = reverse(rest);
    rest->next = node;
    return head;
    
}

Node *merge(Node* node1, Node* node2)
{
    if (node1 == nullptr)
        return node2;
    if (node2 == nullptr)
        return node1;
    if (node1->value < node2->value){
        node1->next = merge(node1->next, node2);
        return node1;
    }
    else
    {
        node2->next = merge(node1, node2->next);
        return node2;
    }
}
int lps(string seq,int i, int j)
{
    if (i == j)
        return 1;
    if (seq[i] == seq[j] && j - i == 1)
        return 2;
    if (seq[i] == seq[j])
        return lps(seq, i+1, j-1)+2;
    return max(lps(seq, i, j-1), lps(seq, i+1, j));
}

int main()
{
    int a[MAX_SIZE];
    for (int i = 0; i < MAX_SIZE; i++)
    {
        a[i] = rand()%100;
    }
    
    mergeSort(a, 0, MAX_SIZE);
    return 0;
    
}

insertion sort:
best case: O(n) it is already sorted
worst case: O(n^2)

quickSort:
worst case: O(n^2)
best case: O(nlogn)
space complexity: O(logn)

Mergesort:
worst case: O(nlogn)
best case: O(nlogn)
space complexity O(n)

balance tree:
depth of every left and right subtree of every node differs by 1 or less.

perfect:
every

preorder: Node left right
inorder: left node right
postorder: left right node

isSymetric(Node *root)
{
    if (root == nullptr)
        return true;
    return isSym(Node *root->left, Node * root->right)
}

isSym(Node *left, Node *right)
{
    if (left == nullptr && right == nullptr)
        return true;
    if (left != nullptr || right != nullptr)
        return false;
    if (left->value != rigth->value)
        return false;
    return isSym(left->left, rigth->rigth) && isSym(left->right, rigth->left);
}

preorderI(Node *root)
{
    stack 
}

BST(Node* root, int min, int max)
{
    if (root == nullptr) return true;
    if (min > root->value || max < root ->value) return false;
    return BST(root->left, min, root->value) && BST(root->left, root->value, max);
}
