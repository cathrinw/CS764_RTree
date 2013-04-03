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

#include "RTree.h"
#include <time.h>

////
// DEFINITIONS FOR RECT, ENTRY AND NODE
////

Rect::Rect(int *mins, int *maxs) {
	for (int i = 0 ; i < DIMENSIONS; ++i) {
		bboxMin[i] = mins[i];
		bboxMax[i] = maxs[i];
	}
}

bool 
Rect::overlaps(Rect &r){
	for (int i = 0 ; i < DIMENSIONS; ++i){
		if (bboxMin[i] > r.bboxMax[i] || r.bboxMin[i] > bboxMax[i])
			return false;
	}
	return true;
}

bool 
Rect::operator==(const Rect &r) const{
	for (int i = 0 ; i < DIMENSIONS; ++i) {
		if (bboxMin[i] != r.bboxMin[i])
			return false;
		if (bboxMax[i] != r.bboxMax[i])
			return false;
	}
	return true;
}

int 
Rect::getVolume(){
	int vol = 1;
	for (int i = 0 ; i < DIMENSIONS; ++i)
		vol *= abs(bboxMax[i] - bboxMin[i]);
	return vol;
}

int 
Rect::getIncVolume(Rect &r){
	int vol = 1;
	for (int i = 0 ; i < DIMENSIONS; ++i)
		vol *= abs(max(bboxMax[i], r.bboxMax[i]) - min(bboxMin[i], r.bboxMin[i]) );
	return vol;
}

void 
Rect::expand(Rect &r) {
	for (int i = 0 ; i < DIMENSIONS; ++i){
		bboxMin[i] = min(bboxMin[i], r.bboxMin[i]);
		bboxMax[i] = max(bboxMax[i], r.bboxMax[i]);
	}
}

/////////

Entry::Entry():nodeId(-1), child(NULL){
}

Entry::Entry(Rect &r){
	nodeId = -1;
	I = r;
	child = NULL;
}

Entry::~Entry(){
	if (child != NULL)
		delete child;
	child = NULL;
}

bool 
Entry::operator==(const Entry &e) const{
	return (e.I == I);
}

bool 
Entry::operator<(const Entry &e) const {
	return (this < &e);
}

string
Entry::toString(){
	char nid[10];
	sprintf(nid, "%i", nodeId);
	string s1 = "[";
	string s2 = "[";
	for (int i = 0 ; i < DIMENSIONS; ++i){
		char minC[10];
		char maxC[10];
		sprintf(minC, "%i,", I.bboxMin[i]);
		sprintf(maxC, "%i,", I.bboxMax[i]);
		s1 += minC;
		s2 += maxC;
	}
	s1+= "],"+s2+"]";
	s1 = nid + s1;
	return s1;	
}

///////

Node::Node(): numEntries(0){	
	entries = new Entry*[PAGESIZE];
}

Node::~Node(){
	for (int i = 0 ; i < numEntries; ++i)
		delete entries[i];
	numEntries = 0;
	delete []entries;
}

bool 
Node::addEntry(Entry *e){
	if (numEntries < PAGESIZE) {
		entries[numEntries++] = e;
		return true;
	}
	return false;	
}

bool 
Node::removeEntry(Entry &e){
	int j = -1;
	int orig = numEntries;
	bool isRemoved = false;
	for (int i = 0; i < orig; ++i){
		if (j >= 0 && j < numEntries)
			entries[j++] = entries[i];
		if (*(entries[i]) == e && !isRemoved) {
			if (entries[i]->child == NULL) {
				--numEntries;
				Entry *delEntry = entries[i];
				delete delEntry;
				entries[i] = NULL;
				j = i;
				isRemoved = true;
			}				
		}
	}
	return (j >= 0);
}

///
///// RTREE /////
///

RTree::RTree() {	
	root = new Node();
	currId = 0;
	numSplits = 0;
	split = &RTree::linearSplit;
}

RTree::~RTree(){
	delete root;
	currId = -1;
}

vector<Entry*> 
RTree::search(Rect &r){
	pagesTouched = 0;
	Node *n;
	int numRes = 0;
	vector<Entry*> result;
	queue<Node*> worklist;
	worklist.push(root);
	while (!worklist.empty()) {
		n = worklist.front();
		worklist.pop();
		++pagesTouched;
		for (int i = 0 ; i< n->numEntries; ++i){
			if (n->entries[i]->I.overlaps(r)){
				if (n->entries[i]->child == NULL) {
					result.push_back(n->entries[i]);
					++numRes;
				} else {
					worklist.push(n->entries[i]->child);
				}
			} 				
		}
	}
	return result;
}

Node *
RTree::chooseLeaf(Entry *e, vector<Node*> &path, int depth) {
	Node *n = root;
	int d = 1;
	while (true) {
		path.push_back(n);
		if (n->numEntries == 0)
			return n;
		if (n->entries[0]->child == NULL || (depth > 0 && d == depth))
			return n;
		int smallestVol = n->entries[0]->I.getVolume();
		int smallest = abs(n->entries[0]->I.getIncVolume(e->I) - smallestVol);		
		Node *currN = n->entries[0]->child;
		for (int i = 1; i < n->numEntries; ++i){			
			int currVol = n->entries[i]->I.getVolume();
			int curr = abs(n->entries[i]->I.getIncVolume(e->I) - currVol);
			if ((curr < smallest) || (curr == smallest && currVol < smallestVol)) {
				smallestVol = currVol;
				smallest = curr;
				currN = n->entries[i]->child;
			}	
		}
		n = currN;
		++d;	
	}
}

void 
RTree::pickSeeds(vector<Entry*> &entries, Entry *&e1, Entry *&e2){
	if (entries.size() == 0)
		return;
	int maxVolume;
	e1 = NULL;
	e2 = NULL;
	int index1, index2;
	for (size_t i = 0 ; i < entries.size()-1; ++i){
		for (size_t j = i+1; j < entries.size(); ++j){
			int v1 = entries[i]->I.getVolume();
			int v2 = entries[j]->I.getVolume();
			int v3 = abs(entries[i]->I.getIncVolume(entries[j]->I) - v1 - v2);
			if (e1 == NULL || v3 > maxVolume){
				e1 = entries[i];
				e2 = entries[j];
				index1 = i;
				index2 = j;
				maxVolume = v3;
			}
		}
	}

	entries.erase(entries.begin()+index1);
	if (index2 > index1)
		--index2;
	entries.erase(entries.begin()+index2);
}

void 
RTree::pickNext(vector<Entry*> &entries ,vector<Entry*> &g1,Rect &r1,vector<Entry*> &g2,Rect &r2){
	int vol1 = r1.getVolume();
	int vol2 = r2.getVolume();
	int maxDiff;
	Entry *currEntry;
	Rect *curRect;
	int currPos;
	vector<Entry*> *curGroup;
	for (size_t i = 0; i < entries.size(); ++i){		
		int g1Poss = r1.getIncVolume(entries[i]->I);
		int g2Poss = r2.getIncVolume(entries[i]->I);
		int diff = abs(g1Poss - g2Poss);
		if (i == 0 || diff > maxDiff){
			maxDiff = diff;
			currPos = i;
			currEntry = entries[i];
			if (g1Poss < g2Poss || (g1Poss == g2Poss && vol1 < vol2)){
				curGroup = &g1;
				curRect = &r1;
			} else {
				curGroup = &g2;
				curRect = &r2;
			}
		}
	}
	curGroup->push_back(currEntry);
	curRect->expand(currEntry->I);
	entries.erase(entries.begin()+currPos);	
}

void 
RTree::linearPickSeeds(vector<Entry*> &entries, Entry *&e1, Entry *&e2){
	float normalized;
	int index1 = 0, index2 = 0;
	for (size_t i = 0 ; i < DIMENSIONS; ++i){
		int currHigh = entries[0]->I.bboxMin[i];
		int currLow = entries[0]->I.bboxMax[i];		
		int mmin = currHigh, mmax = currLow;
		int width = (entries[0]->I.bboxMax[i] - entries[0]->I.bboxMin[i]);
		int i1 = 0, i2 = 0;
		for (size_t j = 1 ; j < entries.size(); ++j){
			mmin = min(mmin, entries[j]->I.bboxMin[i]);
			mmax = max(mmax, entries[j]->I.bboxMax[i]);
			width += (entries[j]->I.bboxMax[i] - entries[j]->I.bboxMin[i]);
			
			if (max(currHigh, entries[j]->I.bboxMin[i]) > currHigh) {
				currHigh = entries[j]->I.bboxMin[i];
				i1 =j;
			}
			
			if (min(currLow, entries[j]->I.bboxMax[i]) < currLow ){
				currLow = entries[j]->I.bboxMax[i];
				i2 = j;
			}
		}
		if (i1 == i2) {
			i2 = (i1 == 0) ? 1 : 0;
			currLow = entries[i2]->I.bboxMax[i];
			for (size_t j = i2+1 ; j < entries.size(); ++j){
				if (min(currLow, entries[j]->I.bboxMax[i]) < currLow && j != (size_t)i1){
					currLow = entries[j]->I.bboxMax[i];
					i2 = j;
				}
			}
		}
		
		float norm = (currHigh-currLow)*1.0/abs(mmax-mmin); //width
		if (i == 0 || norm > normalized){
			normalized = norm;
			index1 = i1;
			index2 = i2;
		}
	}
	e1 = entries[index1];	
	e2 = entries[index2];
	assert(index1 != index2);
	if (index1 > index2) {
		entries.erase(entries.begin()+index1);
		entries.erase(entries.begin()+index2);
	} else {
		entries.erase(entries.begin()+index2);
		entries.erase(entries.begin()+index1);
	}
}

void 
RTree::linearPickNext(vector<Entry*> &entries ,vector<Entry*> &g1,Rect &r1,vector<Entry*> &g2,Rect &r2){
	srand (time(NULL));
	int rd = (int)rand() % entries.size();
	int rd2 = (int)rand();
	Rect *curRect = &r1;
	vector<Entry*> *curGroup = &g1;
	if (rd2 % 2 == 0) {
		curRect = &r2;
		curGroup = &g2;
	}
	
	curGroup->push_back(entries[rd]);
	curRect->expand(entries[rd]->I);
	entries.erase(entries.begin()+rd);
}

void 
RTree::exhaustiveSplit(vector<Entry*> &entries, vector<Entry*> &group1, vector<Entry*> &group2){
	assert(entries.size() == (size_t)PAGESIZE + 1);
	size_t s = entries.size() / 2;

	sort(entries.begin(), entries.begin());
	int totalVolume;
	bool started = false;
	int iters = 0;
	int maxIters = 10 * currId * PAGESIZE;
	for (size_t i = MINSIZE; i <= s; ++i){
		sort(entries.begin(), entries.begin());
		do {
			++iters;
			vector<Entry*> eg1(entries.begin(), entries.begin()+i);
			vector<Entry*> eg2(entries.begin() +i, entries.end());
		
			Rect v1 = eg1[0]->I;
			for (size_t j = 1; j < eg1.size(); ++j){
				v1.expand(eg1[j]->I);
			}
			Rect v2 = eg2[0]->I;
			for (size_t j = 1; j < eg2.size(); ++j){
				v2.expand(eg2[j]->I);
			}
			int totalVol = v1.getVolume() + v2.getVolume();
			if (!started || (totalVol < totalVolume)) {
				totalVolume = totalVol;
				group1.clear();
				group2.clear();
				group1 = vector<Entry*>(eg1);
				group2 = vector<Entry*>(eg2);				
			}
			started = true;
			if (iters > maxIters) 
				return;
		} while (next_permutation(entries.begin(), entries.end()));		
	}
}

void 
RTree::quadSplit(vector<Entry*> &entries, vector<Entry*> &group1, vector<Entry*> &group2){
	Entry *e1, *e2;
	pickSeeds(entries, e1, e2);
	assert(e1 != NULL && e2 != NULL);
	group1.push_back(e1);
	group2.push_back(e2);	
	Rect r1 = e1->I;
	Rect r2 = e2->I;
	while (!entries.empty()){
		if (group1.size() + entries.size() == (size_t)MINSIZE){
			for (size_t i = 0; i < entries.size(); ++i)
				group1.push_back(entries[i]);
			entries.clear();
			break;
		}
		if (group2.size() + entries.size() == (size_t)MINSIZE){
			for (size_t i = 0; i < entries.size(); ++i)
				group2.push_back(entries[i]);
			entries.clear();
			break;
		}
		pickNext(entries, group1, r1, group2, r2);
		
	}
}

void 
RTree::linearSplit(vector<Entry*> &entries, vector<Entry*> &group1, vector<Entry*> &group2){
	Entry *e1, *e2;
	linearPickSeeds(entries, e1, e2);
	assert(e1 != NULL && e2 != NULL);
	group1.push_back(e1);
	group2.push_back(e2);	
	Rect r1 = e1->I;
	Rect r2 = e2->I;
	while (!entries.empty()){
		if (group1.size() + entries.size() == (size_t)MINSIZE){
			for (size_t i = 0; i < entries.size(); ++i)
				group1.push_back(entries[i]);
			entries.clear();
			break;
		}
		if (group2.size() + entries.size() == (size_t)MINSIZE){
			for (size_t i = 0; i < entries.size(); ++i)
				group2.push_back(entries[i]);
			entries.clear();
			break;
		}
		linearPickNext(entries, group1, r1, group2, r2);
		
	}
}

void 
RTree::linearQSplit(vector<Entry*> &entries, vector<Entry*> &group1, vector<Entry*> &group2){
	Entry *e1, *e2;
	linearPickSeeds(entries, e1, e2);
	assert(e1 != NULL && e2 != NULL);
	group1.push_back(e1);
	group2.push_back(e2);	
	Rect r1 = e1->I;
	Rect r2 = e2->I;
	while (!entries.empty()){
		if (group1.size() + entries.size() == (size_t)MINSIZE){
			for (size_t i = 0; i < entries.size(); ++i)
				group1.push_back(entries[i]);
			entries.clear();
			break;
		}
		if (group2.size() + entries.size() == (size_t)MINSIZE){
			for (size_t i = 0; i < entries.size(); ++i)
				group2.push_back(entries[i]);
			entries.clear();
			break;
		}
		pickNext(entries, group1, r1, group2, r2);
		
	}
}

void 
RTree::nodeSplit(vector<Entry*> &entries, vector<Entry*> &group1, vector<Entry*> &group2){
	(this->*split)(entries, group1, group2);
	++numSplits;
}

pair<Node*, Node*> 
RTree::adjustTree(vector<Node*> &path, Node *n, Node *nn ){
	if (n == root)
		return make_pair(n, nn);
	Node *newNode = NULL;	
	Node *parent = path.back();
	path.pop_back();
	assert(n->numEntries > 0);
	Rect r = n->entries[0]->I;
	for (int i = 1 ; i < n->numEntries; ++i)
		r.expand(n->entries[i]->I);
	for (int i = 0; i < parent->numEntries; ++i)
		if (parent->entries[i]->child == n) {
			parent->entries[i]->I = r;
			break;
		}
		
	if (nn != NULL) {
		assert(nn->numEntries > 0);
		Entry *e = new Entry();
		e->child  = nn;
		e->I = nn->entries[0]->I;
		for (int i = 1 ; i < nn->numEntries ; ++i)
			e->I.expand(nn->entries[i]->I);
		if (!parent->addEntry(e)){
			vector<Entry*> entries(parent->entries, parent->entries+parent->numEntries);
			entries.push_back(e);
			vector<Entry*> g1, g2;
			size_t eSize = entries.size();
			nodeSplit(entries, g1, g2);
			assert(eSize == (g1.size() + g2.size()));
			for (size_t i = 0; i < g1.size() ; ++i)
				parent->entries[i] = g1[i];
			for (size_t i = g1.size(); i < (size_t)PAGESIZE; ++i)
				parent->entries[i] = NULL;
			parent->numEntries = g1.size();
			newNode = new Node();
			for (size_t i = 0; i < g2.size() ; ++i)
				newNode->entries[i] = g2[i];
			newNode->numEntries = g2.size();				
		} 
	}
	return adjustTree(path, parent, newNode);
}

void 
RTree::insertRecord(Entry *e, int depth) {
	// need the didLevelUp variable, because during delete the tree can grow a level
	// and items in the toBalance queue will not know about it otherwise. 
	didLevelUp = false;
	vector<Node*> path;
	if (e->nodeId < 0 && depth < 0 && e->child == NULL)  
		e->nodeId = currId++;
		
	Node *leaf = chooseLeaf(e, path, depth);
	Node *n = path.back();
	Node *newLeaf = NULL;
	path.pop_back();
	assert(leaf == n);
	if (!leaf->addEntry(e)) {
		vector<Entry*> entries(leaf->entries, leaf->entries+leaf->numEntries);		
		entries.push_back(e);		
		vector<Entry*> g1, g2;
		nodeSplit(entries, g1, g2);
		for (size_t i = 0 ; i < g1.size(); ++i)
			leaf->entries[i] = g1[i];
		for (size_t i = g1.size(); i < (size_t)PAGESIZE; ++i)
			leaf->entries[i] = NULL;
		leaf->numEntries = (int)g1.size();
		newLeaf = new Node();
		for (size_t i = 0 ; i < g2.size(); ++i)
			newLeaf->entries[i] = g2[i];
		newLeaf->numEntries = (int)g2.size();
	}
	pair<Node*, Node*> rr = adjustTree(path, leaf, newLeaf);
	if (rr.second != NULL) {
		root = new Node();
		Entry *e1 = new Entry(rr.first->entries[0]->I);
		Entry *e2 = new Entry(rr.second->entries[0]->I);
		for (int i = 1; i < rr.first->numEntries; ++i) {
			e1->I.expand(rr.first->entries[i]->I);
		}
		for (int i = 1 ; i < rr.second->numEntries; ++i) {
			e2->I.expand(rr.second->entries[i]->I);	
		}
		e1->child = rr.first;
		e2->child = rr.second;
		root->entries[0] = e1;
		root->entries[1] = e2;
		root->numEntries = 2;
		didLevelUp = true;
	}
}

void
RTree::insertRecord(Rect &r, int depth){
	Entry *e = new Entry(r);
	insertRecord(e,depth);
}

vector<Node*> 
RTree::findLeaf(vector<Node*> &path, Node *n, Rect &r){
	vector<Node*> p(path);
	p.push_back(n);
	if (n->numEntries > 0 && n->entries[0]->child == NULL){
		for (int i = 0 ; i < n->numEntries; ++i){
			if (n->entries[i]->I == r)
				return p;			
		}
		vector<Node*> v;
		return v;
	}
	
	for (int i = 0 ; i < n->numEntries ; ++i){
		if (n->entries[i]->I.overlaps(r)){
			vector<Node *>res = findLeaf(p, n->entries[i]->child, r);
			if (!res.empty())
				return res;
		}
	}
	vector<Node*> v;
	return v;
}

void
RTree::condenseTree(vector<Node*> &path, Node *leaf){
	Node *n = leaf;
	int depth = (int)path.size();
	int d = depth;
	assert(path.back() == n);
	path.pop_back();
	vector<pair<Node*,int> > q;
	while (1){
		if (n == root)
			break;
		Node *parent = path.back();
		path.pop_back();
		Entry *en = NULL;
		for (int i = 0; i < parent->numEntries; ++i){
			if (parent->entries[i]->child == n)
				en = parent->entries[i];
		}
		assert(en != NULL);
		
		if (n->numEntries < MINSIZE){
			en->child = NULL;			
			parent->removeEntry(*en);				
			q.push_back(make_pair(n,d));
		} else {
			en->I = n->entries[0]->I;
			for (int i = 1 ; i < n->numEntries; ++i)
				en->I.expand(n->entries[i]->I);
		}
		n = parent;
		--d;
	}
	int addToDepth = 0;
	for (size_t i = 0; i < q.size(); ++i){
		if (q[i].second == depth) {
			for (int j = 0 ; j < q[i].first->numEntries; ++j){
				insertRecord(q[i].first->entries[j]);
				if (didLevelUp)
					++addToDepth;
			}
			q[i].first->numEntries = 0;
			delete q[i].first;
		} else {
			for (int j = 0 ; j < q[i].first->numEntries; ++j) {
				insertRecord(q[i].first->entries[j], q[i].second+addToDepth);
				if (didLevelUp)
					++addToDepth;
			}
			q[i].first->numEntries = 0;
			delete q[i].first;
		}
	}
}

bool 
RTree::deleteRecord(Rect &r) {
	vector<Node*> path;
	vector<Node*> result = findLeaf(path, root, r);
	if (result.empty()) {
		return false;
	}
	Node *leaf = result.back();
	Entry *e = NULL;
	for (int i = 0; i < leaf->numEntries; ++i)
		if (leaf->entries[i]->I == r)
			e = leaf->entries[i];
	if (e != NULL){
		leaf->removeEntry(*e);
	}
	condenseTree(result, leaf);
	if (root->numEntries == 1 && root->entries[0]->child != NULL) {
		Node *c = root->entries[0]->child;
		root->entries[0]->child = NULL;
		delete root;
		root = c;		
	}
	return true;
}

void
RTree::printTree(){
	queue<pair<Node*, int> > worklist;
	worklist.push(make_pair(root, 1));
	int cur = 0;
	string spacer = " ";
	while (!worklist.empty()) {
		pair<Node*, int> cp = worklist.front();
		worklist.pop();
		if (cp.second > cur) {
			cout << "\nLEVEL " << cp.second << endl;
			spacer += "    ";
			cur = cp.second;			
		}
		cout << "\n" << spacer <<  "num entries: " << cp.first->numEntries << "  ";		 
		for (int i = 0 ; i < cp.first->numEntries; ++i){
			string eStr = cp.first->entries[i]->toString();
			cout << eStr << " ; " ;
			if (cp.first->entries[i]->child != NULL) {
				worklist.push(make_pair(cp.first->entries[i]->child, cp.second+1));
				assert(cp.first->entries[i]->nodeId < 0);
			}
		}
	}
	cout << "\n---------" << endl << endl;
}

long 
RTree::getFileSize() {
	long numPages = 0;
	queue<pair<Node*, int> > worklist;
	worklist.push(make_pair(root, 1));
	int cur = 0;
	while (!worklist.empty()) {
		pair<Node*, int> cp = worklist.front();
		worklist.pop();
		if (cp.second > cur) {
			cur = cp.second;			
		}		 
		for (int i = 0 ; i < cp.first->numEntries; ++i){
			if (cp.first->entries[i]->child != NULL) {
				worklist.push(make_pair(cp.first->entries[i]->child, cp.second+1));
			} else {
				++numPages;
			}
		}
	}
	return numPages;
	
}

// we need to simulate different page sizes. The OS could *maybe* do it for us, but it seems like a hassle.
// Compute the size of the tree with respect to data size and current PAGESIZE instead.
long 
RTree::getIndexSize() {
	long numPages = sizeof(int) + (PAGESIZE * sizeof(Entry*));
	queue<pair<Node*, int> > worklist;
	worklist.push(make_pair(root, 1));
	int deepestLeaf = -1;
	while (!worklist.empty()) {
		pair<Node*, int> cp = worklist.front();
		worklist.pop();
		numPages += sizeof(int) + (PAGESIZE * sizeof(Entry*));
		for (int i = 0 ; i < cp.first->numEntries; ++i){
			if (cp.first->entries[i]->child != NULL) {				
				worklist.push(make_pair(cp.first->entries[i]->child, cp.second+1));				
			} else {
				if (deepestLeaf < 0)
					deepestLeaf = cp.second;
				assert(deepestLeaf == cp.second);
			}
			numPages += sizeof(Entry);
		}
	}
	return numPages;
	
}
				