import math
import string
import sys
import random
import time
from fractions import Fraction

def convTernary(i, l):
	temp = i - convTernary.last
	tempb = i
	for z in range(l):
		convTernary.M1 += Fraction(2-tempb%2)*Fraction(temp % 2)*Fraction(3)**Fraction(-z)
		temp = temp >> 1
		tempb = tempb >> 1
	return convTernary.M1

def main(argv=None):
	if argv == None:
		argv = sys.argv
	M2 = [0, ]
	for i in range(1,int(sys.argv[1])):
		M2.append(Fraction(Fraction(3)**(-i)/(Fraction(1) - Fraction(3)**(-i))))
	convTernary.M1 = 0
	convTernary.last = 0
	for i in range(1,int(sys.argv[1])):
		for j in range(1,2**i):
			temp = j
			convTernary(j,i)
			#M1 = Fraction(0)
			#for z in range(i):
			#	M1 += Fraction(Fraction(2)*Fraction(temp % 2)*Fraction(3)**Fraction(-z))
			#	temp = temp >> 1
			#print("j: "+bin(j)[2:].zfill(i)[::-1])
			temp = Fraction(convTernary.M1*M2[i])
			print(str(temp.numerator)+','+str(temp.denominator))
		
if __name__=="__main__":
	main()
			