/**************************
 * Copyright 2013 Cathrin Weiss (cweiss@cs.wisc.edu)
 * 
 * This tool is responsible for running the experiments as described in the Guttman paper
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

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <iostream>
#include <time.h>
#include <sys/resource.h>
#include <fstream>
#include "RTree.h"

using namespace std;

unsigned int numLines = 0;

struct Rec
{
  Rec()  {}

  Rec(int a_minX, int a_minY, int a_maxX, int a_maxY)
  {
    min[0] = a_minX;
    min[1] = a_minY;

    max[0] = a_maxX;
    max[1] = a_maxY;
  }


  int min[2];
  int max[2];
};



int PAGESIZE = 8;
int MINSIZE = 2;



void runDelete(int i , RTree &tree, int stop = -1) {
	struct rusage usage;
	struct timeval start, end;
	string fname = "randomDataDelete.bin";
	if (stop > 0)
		fname = "randomDataDyn.bin";
	ifstream f(fname.c_str());
	if (!f) {
		cout << "File " << fname << " does not exist. It must exist in the current working directory for this test to work." << endl;
		return;
	}
	int xmin, ymin, xmax, ymax;
	int k = 0, j = 0;
	double timespent = 0;
	double timespent_total = 0;
	while (1) {		
		f >> xmin;
		if (f.eof() )
			break;
		f >> ymin;
		f >> xmax;
		f >> ymax;		
			
		++k;
		if (stop > 0 && k > stop)
			break;
		Rec r(xmin, ymin, xmax, ymax);
		Rect rect(r.min, r.max);		
		bool b = false;
		if ( k % 10 == 0) {
			getrusage(RUSAGE_SELF, &usage);
			start = usage.ru_stime;
			b = tree.deleteRecord(rect); 
			getrusage(RUSAGE_SELF, &usage);
			end = usage.ru_stime;
			timespent += ((end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec);
			timespent_total += ((end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec);
			++j;
		} else {
			getrusage(RUSAGE_SELF, &usage);
			start = usage.ru_stime;
			b = tree.deleteRecord(rect); 	
			getrusage(RUSAGE_SELF, &usage);
			end = usage.ru_stime;
			timespent_total += ((end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec);
		}
		if (!b)
			cout << "Failed at: " << k << " , " << xmin << " " << ymin << " " << xmax << " " << ymax << endl;
		assert(b);
	}
	f.close(); 
	timespent = (timespent * 1.0)/(j );
	timespent_total = (timespent_total * 1.0)/k;
	cout << "	"  << timespent << endl;
}


void runSearch(RTree &tree, string fn = "searches.bin") {
	struct rusage usage;
	struct timeval start, end;
	
	ifstream f(fn.c_str());
	if (!f) {
		cout << "File " << fn << " does not exist. It must exist in the current working directory for this test to work." << endl;
		return;
	}
	int xmin, ymin, xmax, ymax;
	double timespent = 0;
	int j = 0;
	int pTouched = 0;
	int qualRecs = 0;
	while (1) {		
		f >> xmin;
		if (f.eof() )
			break;
		f >> ymin;
		f >> xmax;
		f >> ymax;		

		Rec r(xmin, ymin, xmax, ymax);
		Rect rect(r.min, r.max);		
		getrusage(RUSAGE_SELF, &usage);
		start = usage.ru_stime;
		vector<Entry*> v = tree.search(rect); 
		qualRecs += (int)v.size();
		pTouched += tree.pagesTouched;
		getrusage(RUSAGE_SELF, &usage);
		end = usage.ru_stime;
		timespent += ((end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec);
		++j;		
	} 
	f.close();
	timespent = (timespent * 1.0)/(j );
	cout << (pTouched*1.0/qualRecs) << "	"<< ((timespent*1000.0)/qualRecs) <<"	";	
}

void runInsert(int i ) {
	struct rusage usage;
	struct timeval start, end;

	RTree tree;
	if (i == 1){
		tree.split = &RTree::quadSplit;
	}
	else if (i == 2) {
		tree.split = &RTree::exhaustiveSplit;
	}
	
	ifstream f("randomData.bin");
	if (!f) {
		cout << "File randomData.bin does not exist. It must exist in the current working directory for this test to work." << endl;
		return;
	}
	int xmin, ymin, xmax, ymax;
	int k = 0, j = 0;
	double timespent = 0;
	int np = (numLines *1.0 / 100)*90;
	long numSplits = 0;
	while (1) {		
		f >> xmin;
		if (f.eof() )
			break;
		f >> ymin;
		f >> xmax;
		f >> ymax;		
			
		++k;
		Rec r(xmin, ymin, xmax, ymax);
		Rect rect(r.min, r.max);	
		if (k > np) {
			int ns = tree.numSplits;
			getrusage(RUSAGE_SELF, &usage);
			start = usage.ru_stime;
			tree.insertRecord(rect); 
			getrusage(RUSAGE_SELF, &usage);
			end = usage.ru_stime;
			timespent += ((end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec);
			++j;
			numSplits += (tree.numSplits - ns);
		} else {
			tree.insertRecord(rect);
		}
	} 
	f.close();
	timespent = (timespent * 1.0)/(j );

	long bpp = 64;
	long nodeSize = sizeof(Node) + (PAGESIZE * sizeof(Entry));
	while ( (bpp - nodeSize) < 0)
		bpp *= 2;

	cout << bpp << "	" << j << "	" << (tree.getIndexSize()/1024.0) << "	" << timespent <<"	" << numSplits << "	";
	runSearch(tree);
	runDelete(i, tree);
}

void runNumInsert(int i ) {
	struct rusage usage;
	struct timeval start, end;
		
	for (int c = 1000; c < 6000; c+=2000) {
		RTree tree;
		if (i == 1){
			tree.split = &RTree::quadSplit;
		}
		ifstream f("randomDataDyn.bin");
		if (!f) {
			cout << "File randomDataDyn.bin does not exist. It must exist in the current working directory for this test to work." << endl;
			return;
		}
		int xmin, ymin, xmax, ymax;
		int k = 0, j = 0;
		double timespent = 0;
		int np = (c*1.0/100)*90.0;
		long numSplits = 0;		
		while (1) {		
			f >> xmin;
			if (f.eof() )
				break;
			f >> ymin;
			f >> xmax;
			f >> ymax;		

			++k;
			Rec r(xmin, ymin, xmax, ymax);
			Rect rect(r.min, r.max);		
			int numS = tree.numSplits;
			getrusage(RUSAGE_SELF, &usage);
			start = usage.ru_stime;
			tree.insertRecord(rect); 
			getrusage(RUSAGE_SELF, &usage);
			end = usage.ru_stime;
			if (k > np) {
				timespent += ((end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec);
				numSplits += (tree.numSplits - numS);				
				++j;
			}
			if (k == c)
				break;			
		} 
		f.close();
		cout << k << "	" << (tree.getIndexSize()/1024.0) << "	" << (timespent * 1.0)/(j ) << "	" << numSplits << "	";
		if (k > 4000)
			runSearch(tree, "dynSearches.bin");
		else if (k > 2000)
			runSearch(tree, "dynSearches3k.bin");
		else 
			runSearch(tree);
		runDelete(i,tree, c);
	}

}


int main(int argc, char **argv) {
	int i = 0;
	if (argc > 1) {
		i = atoi(argv[1]);
		PAGESIZE = atoi(argv[2]);
		if (argc == 4)
			MINSIZE = PAGESIZE/2;
		if (i == 1 && argc > 4)
			MINSIZE = PAGESIZE/3;
	}
	ifstream f("randomData.bin");
	if (!f) {
		cout << "File randomData.bin does not exist. It must exist in the current working directory for this test to work." << endl;
		return 0;
	}
	string line;
	while (1) {
		getline(f, line);
		if (f.eof())
			break;
		++numLines;
	}
	f.close();	
	if (argc > 4)	
		runNumInsert(i);		
	else 
		runInsert(i);			
	return 0;
	
}