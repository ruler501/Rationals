import math
import fractions

def rationalBasePrint(p, q, base):
    if p > q:
        p %= q
    if q == 1:
        return False
    numericConstants = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    result = "0."
    power = math.floor(math.log(p/q, base))
    if base > len(numericConstants):
        return False
    while q % base == 0:
        q /= base
        result += numericConstants[int(p/q)]
        p %= q
    result += '{'
    p_start = p
    for i in range(15):
        pb = p * base
        result += numericConstants[int(pb/q)]
        p = pb % q
        if p == p_start:
            break
    result += '}'
    print(result)
    return result

def basePrint(num, base):
    numericConstants = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    if base > len(numericConstants):
        return False
    power = math.floor(math.log(num, base))
    num /= base**power
    result = numericConstants[int(num)]+'.'
    for i in range(15):
        result += numericConstants[int(num)]
        num -= int(num)
        num *= base
    print(result+" "+str(base)+"^"+str(power))
    return result+" "+str(base)+"^"+str(power)

ourResult=[]
total = 0
count = 0

p = q = 1
b = 2
#while p != 0:
#    p = int(input("p "))
#    q = int(input("q "))
#    b = int(input("b "))
#    rationalBasePrint(p, q, b)


##for n in range(27):
##        rationalBasePrint(1, 3*n +1, 3)

#print('')

#ourResult.sort()           
#for i in ourResult:
#   print(i[1])

#rationalBasePrint(1, 16, 3)

#def count(n):
#	return (n**2-n)/2

base = 10
total = count = 0
totals=[0]*10
for q in range(0,10000):
	for p in range(0,q):
		if fractions.gcd(p,q) != 1:
			count += 1
			for i in range(2,10):
				if q % i == 0:
					totals[i] += 1
for i in range(2,100):
	print(i,i*totals[i]/count)