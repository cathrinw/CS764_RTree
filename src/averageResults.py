import sys
import os

fname = sys.argv[1]
numLines = int(sys.argv[2])

f = open(fname)
lines = f.readlines()
f.close()
allLines = []
resLines = []
for i in range(0,numLines) : 
	allLines.append([])
	resLines.append([])

i = 0	
for line in lines : 
	allLines[i%numLines].append(line)
	i+=1
	

i = 0
for lineList in allLines : 
	#print lineList
	convertLists = map(lambda s: map(float, s), map(lambda s: s.split("\t"), map(lambda s: s.replace("\t\t", "\t"),map(str.strip , lineList))))
	#convertLists = [float(item.strip().replace("\t\t", "\t").split("\t")) for item in lineList]

	for lst in convertLists : 
		if len(resLines[i]) == 0 : 
			resLines[i].extend(lst)
		else :
			for j in range(0, len(lst)):
				resLines[i][j] += lst[j]
		
	for j in range(0, len(resLines[i])):
		resLines[i][j] /=  len(convertLists)
	resLines[i][0] = int(resLines[i][0])
		
	i += 1
	
for line in resLines : 
	s = ""
	for item in line : 
		s += str(item) + "\t"
	print s