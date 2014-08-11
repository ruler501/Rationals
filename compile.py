import glob
import os
import math
import sys

#basedir = '/home/dsr0088/'
#basedir = 'D:\\C++\\Rationals\\'
qsv = {}
qs = []
#for d in filter(os.path.isdir, [os.path.join(basedir,f) for f in os.listdir(basedir)]):
#	os.chdir(d)
for fn in glob.glob('*.csv'):
	nqs = []
	f = open(fn)
	for line in f.readlines():
		line = line.strip()
		if len(line.split(',')) != 2 or line.split(',')[0] == 'q':
			continue
		q = int(line.split(',')[0])
		if q in qs:
			print("dupe")
			continue
		if q not in nqs:
			nqs.append(q)                                                      
		try:
			qsv[int(line.split(',')[0])] += int(line.split(',')[1])
		except:
			qsv[int(line.split(',')[0])] = int(line.split(',')[1])
	f.close()
	qs += nqs

maxn = 18
if len(sys.argv) > 1:
	maxn = int(sys.argv[1])
max = 3**maxn

qs.sort()

datafile = open('../data.csv', 'w')
datafile.write('Q,Rationals,Q mod 3,Maximum Expected\n')
for q in qs:
	if q > max:
		continue
	datafile.write(str(q)+','+str(qsv[q])+','+str(q % 3)+','+str(int(q**(math.log(2)/math.log(3))))+'\n')
datafile.close()

statdata = open('../stats.csv', 'w')
statdata.write('N,Rationals,Denominators,Rationals per Denominator\n')
total = {}
for n in range(0,maxn):
	min = 3**n
	max = 3**(n+1)
	total[n] = [0,0]
	
	for i in range(qs.index(min),qs.index(max)+1):
		total[n][0] += qsv[qs[i]]
		total[n][1] += 1
	statdata.write(str(n)+','+str(total[n][0])+','+str(total[n][1])+','+str(total[n][0]/total[n][1])+'\n')
a = None

statdata.write('\n')
	
statdata.write('N,Fraction Ratio,Denominator Ratio,Rational/Denominator Ratio\n')
for n in range(1,maxn):
	statdata.write(str(n)+','+str(float(total[n][0])/total[n-1][0])+','+str(float(total[n][1])/total[n-1][1])+','+str((total[n][0]/total[n][1])/(total[n-1][0]/total[n-1][1]))+'\n') 