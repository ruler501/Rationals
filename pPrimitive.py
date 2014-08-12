import glob
import os
import math
import sys

qsv = {1 : 2}
qs = []
for fn in glob.glob('*.csv'):
	nqs = []
	print("Loading data from "+fn)
	f = open(fn)
	for line in f:
		line = line.strip()
		if len(line.split('/')) != 2:
			continue
		q = int(line.split('/')[1])
		if q in qs:
			print("dupe")
			continue
		if q not in nqs:
			nqs.append(q)                                                      
		try:
			qsv[int(line.split('/')[1])] += 1
		except:
			qsv[int(line.split('/')[1])] = 1
	f.close()
	qs += nqs

maxn = 18
if len(sys.argv) > 1:
	maxn = int(sys.argv[1])
max = 3**maxn

qs.sort()

print("Writing data")
datafile = open('../data.csv', 'w')
datafile.write('Q,Rationals,Q mod 3,Maximum Expected\n')
for q in qs:
	if q > max:
		continue
	datafile.write(str(q)+','+str(qsv[q])+','+str(q % 3)+','+str(int(q**(math.log(2)/math.log(3))))+'\n')
datafile.close()

print("Writing stats")
statdata = open('../stats.csv', 'w')
statdata.write('N,Rationals,Denominators,Rationals per Denominator\n')
total = {}
for n in range(0,maxn):
	minQ = 3**n
	maxQ = 3**(n+1)
	total[n] = [0,0]
	
	closest = [0,0]
	for i in range(len(qs)):
	    if qs[closest[1]] < qs[i] and qs[i] < maxQ:
	        closest[1] = i
	    if qs[closest[0]] > qs[i] and qs[i] > minQ:
	        closest[0] = i
	
	for i in range(closest[0],closest[1]):
		total[n][0] += qsv[qs[i]]
		total[n][1] += 1
	try:
	    statdata.write(str(n)+','+str(total[n][0])+','+str(total[n][1])+','+str(total[n][0]/total[n][1])+'\n')
	except ZeroDivisionError:
		statdata.write(',,,\n')
a = None

statdata.write('\n')
	
statdata.write('N,Fraction Ratio,Denominator Ratio,Rational/Denominator Ratio\n')
for n in range(1,maxn):
	try:
		statdata.write(str(n)+','+str(float(total[n][0])/total[n-1][0])+','+str(float(total[n][1])/total[n-1][1])+','+str((total[n][0]/total[n][1])/(total[n-1][0]/total[n-1][1]))+'\n') 
	except ZeroDivisionError:
		statdata.write(',,,\n')
