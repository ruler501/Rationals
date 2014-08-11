#include<mpirxx.h>
#include<cmath>
#include<cstdlib>
#include<iostream>
#include<fstream>

void convCTernary(long long int const &pq, unsigned char const &l, mpq_class &M1){
    for(int j=0; j < l; j++)
        M1 += mpq_class(int(pq >> (j) & 1)) * mpq_class(std::pow(3,j));
}

void printRationals(unsigned char const &n, unsigned long long int const &minJ, unsigned long long int const &maxJ){;
	mpq_class M2(1, pow(3,n) - 1);
	bool skip = false;
	for(unsigned long long int j=minJ; j < maxJ; j++){
		skip = false;
		for(int i = 1; i<=n/2 && !skip; i++){
			if(n%i == 0){
				if(j  % (((1<<n)-1)/((1<<i)-1)) == 0) skip = true;
			}
		}
		if(skip) continue;
		mpq_class M1(0);
		convCTernary(j, n, M1);
		std::cout << mpq_class(mpq_class(2)*M1*M2) << std::endl;
	}
}

int main(int argc, char* argv[]){
    if(argc == 2){
		for(int i = 2; i<atoi(argv[1]); i++) printRationals(i, 1, 1<<i);
    }
    if(argc == 4){
		unsigned char i = atoi(argv[1]);
		unsigned long long int minJ = atoi(argv[2]);
		unsigned long long int maxJ = atoi(argv[3]);
		printRationals(i, minJ, maxJ);
	}

}
