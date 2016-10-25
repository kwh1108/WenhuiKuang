//
//  main.cpp
//  HW2
//
//  Created by wenhui kuang on 1/20/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include "Map.h"
#include <iostream>
#include <assert.h>
#include <string.h>
using namespace std;

int main(int argc, const char * argv[]) {
    
    KeyType k;
    ValueType v;
    Map hello; //Initiate a new map without specification
    assert(hello.empty()); //Check that hello has no nodes
    assert(hello.size()==0); //Same as above (test multiple functions)
    assert(hello.erase("joke")==false); //Check that there's nothing to delete
    assert(hello.contains("troll")==false); //Check that there's nothing contained
    assert(hello.insert("Head", 0));
    assert(!hello.empty()); //Check that hello is no longer empty
    assert(hello.size()==1); //Check that the single node has been inserted
    assert(hello.contains("Head")); //Check that inserted nodes are contained
    assert(!hello.insert("Head", 1)); //Check that duplicate keys cannot be inserted
    assert(hello.size()==1); //Check that the size has not changed
    hello.get("Head", v);
    assert(v==0); //Check that the node with key "Head" is holding the right value
    assert(!hello.erase("blah")); //Check that a node that does not exist cannot be erased
    assert(hello.update("Head", 9)); //Check that the node with key "Head" is found and updated
    hello.get("Head", v);
    assert(v==9); //Check that that node is holding the updated value
    assert(hello.insert("Between", 8)); //Check that another node can be inserted
    hello.get("Between", v);
    assert(v==8); //Check that that node is holding the right value
    hello.insertOrUpdate("Tail", 11);
    hello.insertOrUpdate("Between", 10);
    assert(hello.size()==3); //Check that the right number of nodes exists
    for(int i=0; i<hello.size(); i++) {
        hello.get(i, k, v);
        cerr << k << "->" << v << endl;
    }
    //Should print out keys and values such that Head->9; Between->10; Tail->11
    assert(hello.contains("Head") && hello.contains("Between") && hello.contains("Tail")); //Make sure the right nodes are contained
    assert(!hello.contains("head")); //Check that other nodes are not considered contained
    cout << "Passed basic tests involving one map." << endl; //Testing copy constructor and equals operand
    Map helloCopy1(hello); //test that the copy constructor makes an exact copy of the map
    assert(helloCopy1.size()==3); //Check that the right number of nodes exists
    for(int i=0; i<helloCopy1.size(); i++) {
        helloCopy1.get(i, k, v);
        cerr << k << "->" << v << endl;
    }

    //Should print out keys and values such that Head->9; Between->10; Tail->11
    Map helloCopy2=hello; //test that the copy constructor makes an exact copy of the map
    assert(helloCopy2.size()==3); //Check that the right number of nodes exists
    for(int i=0; i<helloCopy2.size(); i++) {
        helloCopy2.get(i, k, v);
        cerr << k << "->" << v << endl;
    }
    helloCopy2.get(0, k, v);
    helloCopy2.erase(k); //Check that the map can be emptied by erase
    cerr << k << "->" << v << " removed from helloCopy2" << endl;
    Map blank; //Create a blank map
    helloCopy1=blank;
    assert(helloCopy1.empty()); //Check that equals operand works for empty maps
    Map anotherBlank(blank);
    assert(anotherBlank.empty()); //Chcek that copy constructor works for empty maps
    cout << "Passed basic tests involving copy constructor and equals operand." << endl;
    Map oneNode;
    oneNode.insert("blah", 1);
    Map copyOneNode(oneNode); //Test the copy constructor with only one node
    assert(copyOneNode.size()==1); //Check that the size is correct
    assert(copyOneNode.contains("blah")); //Check that the key is contained in the new map
    Map goodbye;
    hello.swap(goodbye); //Swap hello with an empty map
    assert(hello.empty()); //Make sure the amount of elements has switched
    assert(goodbye.size()==3);
    for(int i=0; i<goodbye.size(); i++) {
        goodbye.get(i, k, v);
        cerr << k << "->" << v << endl;
    }
    goodbye.swap(hello); //Swap hello and goodbye back to normal
    assert(goodbye.empty());
    assert(hello.size()==3);
    for(int i=0; i<hello.size(); i++) {
        hello.get(i, k, v);
        cerr << k << "->" << v << endl; //Verify that hello and the original goodbye are the same
    }
    cout << "Passed swap tests." << endl;
    assert(hello.erase("Between")); //Check that the middle node is removed and size is updated
    assert(hello.size()==2);
    assert(hello.erase("Tail")); //Check that the tail node can be removed
    assert(hello.size()==1);
    assert(hello.erase("Head"));
    assert(hello.empty()); //Check that the head node can be removed (when it's the last one)
    assert(hello.insert("new head", 5));
    assert(hello.insert("new tail", 6));
    assert(hello.erase("new head")); //Check that the head node can be removed (when it's NOT the last node)
    assert(hello.size()==1);
    cout << "Passed erase tests." << endl;
    Map firstMap;
    firstMap.insert("one", 1); //Should be included in combine
    firstMap.insert("two", 2); //Should be included in combine
    firstMap.insert("three", 3); //Should be included in combine, since the values in first/secondMap equal
    firstMap.insert("four", 4); //Should NOT be included in combine, since the values in first/secondMap differ
    Map secondMap;
    secondMap.insert("three", 3);
    secondMap.insert("four", 10); //Inconsistent value! Should not be in combined Map
    secondMap.insert("five", 5);
    Map answer;
    assert(combine(firstMap, secondMap, answer)==false);
    assert(answer.size()==4); //Check that the right number of nodes exist
    for(int i=0; i<answer.size(); i++)
    {
        answer.get(i, k, v);
        cerr << k << "->" << v << endl;
    }
    assert(combine(firstMap, secondMap, secondMap)==false); //Check that combine works in face of aliasing
    assert(secondMap.size()==4);
    for(int i=0; i<secondMap.size(); i++)
    {
        secondMap.get(i, k, v);
        cerr << k << "->" << v << endl;
    }
    Map simpleOne; simpleOne.insert("blah", 1);
    Map simpleTwo; simpleTwo.insert("bleh", 2); Map simpleThree;
    assert(combine(simpleOne, simpleTwo, simpleThree)); //Check that combine returns true for unique keys
    assert(simpleThree.size()==2);
    assert(simpleThree.contains("blah") && simpleThree.contains("bleh")); //Check for the correct pairs
    Map random;
    assert(random.insert("eek", 0) && random.insert("aah", 1)); assert(random.size()==2);
    Map blankMap;
    Map blankResult;
    combine(random, blankMap, blankResult); //test subtract with an empty Map as m2
    assert(blankResult.size()==2 && blankResult.contains("eek") && blankResult.contains("aah"));
    combine(blankMap, random, blankResult); //test subtract with an empty Map as m1
    assert(blankResult.size()==2 && blankResult.contains("eek") && blankResult.contains("aah"));
    cout << "Passed combine tests." << endl;
    Map firstMapCopy;
    firstMapCopy.insert("one", 1); //Should be included in combine
    firstMapCopy.insert("two", 2); //Should be included in combine
    firstMapCopy.insert("three", 3); //Should be included in combine, since the values in first/secondMap equal
    firstMapCopy.insert("four", 4); //Should NOT be included in combine, since the values in first/secondMap differ
    Map secondMapCopy;
    secondMapCopy.insert("three", 3);
    secondMapCopy.insert("four", 10); //Inconsistent value! Should not be in combined Map
    secondMapCopy.insert("five", 5);
    Map anotherResult;
    subtract(firstMapCopy, secondMapCopy, anotherResult);
    assert(anotherResult.size()==2);
    assert(anotherResult.contains("one") && anotherResult.contains("two")); //Check for the correct pairs
    for(int i=0; i<anotherResult.size(); i++) {
        anotherResult.get(i, k, v); cerr << k << "->" << v << endl;
    }
    subtract(firstMapCopy, secondMapCopy, firstMapCopy); //Check that subtract works in the face of aliasing
    assert(firstMapCopy.size()==2);
    assert(firstMapCopy.contains("one") && firstMapCopy.contains("two")); //Check for the correct pairs
    for(int i=0; i<firstMapCopy.size(); i++) {
        firstMapCopy.get(i, k, v); cerr << k << "->" << v << endl;
    }
    Map random2;
    assert(random2.insert("eek", 0) && random2.insert("aah", 1));
    assert(random2.size()==2);
    Map blankMap2;
    Map blankResult2;
    subtract(random2, blankMap2, blankResult2); //test subtract with an empty Map as m2
    assert(blankResult2.size()==2 && blankResult2.contains("eek") && blankResult2.contains("aah"));
    subtract(blankMap2, random2, blankResult2); //test subtract with an empty Map as m1
    assert(blankResult2.empty());
    cout << "Passed subtract tests." << endl; cout << "Passed all of my tests." << endl;
    return 0;
}
