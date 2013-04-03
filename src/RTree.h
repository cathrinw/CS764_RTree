/**************************
 * Copyright 2013 Cathrin Weiss (cweiss@cs.wisc.edu)
 * 
 * These structures are described in the Guttman 1984 paper. Function names 
 * are used literally according to paper descriptions
 *
 * This RTree implmentation is free software: you can redistribute it and/or modify it under the terms of the 
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along with this software. 
 * If not, see http://www.gnu.org/licenses/.
 **************************/
#ifndef __RTREE_H_
#define __RTREE_H_

#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// Here you could change the DIMENSIONS if you were so inclined
#define DIMENSIONS 2

// PAGESIZE and MINSIZE need to be defined in the main experiment code.
extern int PAGESIZE ;
extern int MINSIZE ;

/***
* Note: This version supports int rectangle values only. 
*/

struct Rect {
	int bboxMin[DIMENSIONS];
	int bboxMax[DIMENSIONS];
	
	Rect(){}
	Rect(int *mins, int *maxs);	
	~Rect(){}
	bool operator==(const Rect &r) const;
	int getVolume();	
	int getIncVolume(Rect &r);	
	void expand(Rect &r);	
	bool overlaps(Rect &r);
};


/// Declarations for Node and Entry

struct Node;

struct Entry {
	int nodeId;
	Rect I;
	Node *child;
	
	Entry();
	Entry(Rect &r);
	~Entry();	
	bool operator==(const Entry &e) const;
	bool operator<(const Entry &e) const;
	string toString();
};

struct Node {
	Entry **entries;	
	int numEntries;

	Node();
	~Node();
	bool addEntry(Entry *e);
	bool removeEntry(Entry &e);	
};

//// RTREE

struct RTree {
	Node *root;
	int currId;
	int pagesTouched;
	long numSplits;
	bool didLevelUp;
	
	void (RTree::*split)(vector<Entry*>&,vector<Entry*>&,vector<Entry*>&);
	
	RTree();	
	~RTree();
	
	vector<Entry*> search(Rect &r);	
	Node *chooseLeaf(Entry *e, vector<Node*>&, int depth=-1);
	void pickSeeds(vector<Entry*>&, Entry*&, Entry*&);
	void pickNext(vector<Entry*>&,vector<Entry*>&,Rect &,vector<Entry*>&,Rect &);	
	
	void linearPickSeeds(vector<Entry*>&, Entry*&, Entry*&);
	void linearPickNext(vector<Entry*>&,vector<Entry*>&,Rect &,vector<Entry*>&,Rect &);	
	
	void exhaustiveSplit(vector<Entry*>&,vector<Entry*>&,vector<Entry*>&);	
	void quadSplit(vector<Entry*>&,vector<Entry*>&,vector<Entry*>&);	
	void linearSplit(vector<Entry*>&,vector<Entry*>&,vector<Entry*>&);	
	void linearQSplit(vector<Entry*>&,vector<Entry*>&,vector<Entry*>&);
	
	void nodeSplit(vector<Entry*>&,vector<Entry*>&,vector<Entry*>&);	
	
	pair<Node*, Node*> adjustTree(vector<Node*> &path, Node *argN, Node *argNN = NULL);	
	void insertRecord(Entry *e, int depth=-1);	
	void insertRecord(Rect &r, int depth=-1);
	
	vector<Node*> findLeaf(vector<Node*>&, Node*, Rect&);
	void condenseTree(vector<Node*> &path, Node *leaf);
	bool deleteRecord(Rect &r);
	
	void printTree();
	long getFileSize();
	long getIndexSize();
	 	
};

#endif