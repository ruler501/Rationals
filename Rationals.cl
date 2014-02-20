bool testCantor(unsigned long p, unsigned long q){
	while(q % 3 == 0){
		q /= 3;
		if (p/q == 1) return p==q;
		p -= q*((unsigned long)p/q);
	}
	unsigned long p_start = p;
	do{
		unsigned long p3 = p * 3;
		if(p3/q == 1) return false;
		p = p3 - q*((unsigned long)p3/q);
	} while(p != p_start);
	return true;
}

int coprime(unsigned long a, unsigned long b){
    long c;
    while (a != 0){
        c = a;
        a = b % a;
        b = c;
    }
    return (b==1)&2;
}

__kernel
void countRationals(__global unsigned long *input, __global unsigned long *output){
    int gid = get_global_id(0);
    unsigned long q = input[gid], p = 1;
    output[gid] = 0;
    for(p = 1; p <= q/3; p += 3){
        if(testCantor(p, q))
            for(unsigned long i = p; i <= q/3; i *= 3)
                    output[gid] += coprime(i,q);
    }
    for(p = 2; p <= q/3; p += 3){
        if(testCantor(p, q))
            for(unsigned long i = p; i <= q/3; i *= 3)
                    output[gid] += coprime(i,q);
    }
    gid = 32767 - get_global_id(0);
    q = input[gid];
    output[gid] = 0;
    for(p = 1; p <= q/3; p += 3){
        if(testCantor(p, q))
            for(unsigned long i = p; i <= q/3; i *= 3)
                    output[gid] += coprime(i,q);
    }
    for(p = 2; p <= q/3; p += 3){
        if(testCantor(p, q))
            for(unsigned long i = p; i <= q/3; i *= 3)
                    output[gid] += coprime(i,q);
    }
}
