#include<mpirxx.h>
#include<cmath>
#include<cstdlib>
#include<iostream>
#include<fstream>


mpq_class M1(0);
long long int last(0);

void convCTernary(long long int pq, unsigned char l){
    for(int j=0; j<l; j++)
        M1 += mpq_class(2)* mpq_class(int(pq >> j) & 1) * mpq_class(1,std::pow(3,j));
}

int main(int argc, char* argv[]){
    if(argc == 2){
		unsigned char minI = 1;
		unsigned char maxI = atoi(argv[1]);
		mpq_class M2;
		for (unsigned char i=minI; i < maxI; i++){
			mpq_class temp = mpq_class(1,pow(3,i));
			M2 = temp/(1-temp);
			long long int maxJ = 1<<(i-1);
			for(long long int j=0; j < maxJ; j++){
				convCTernary(j, i);
				std::cout << mpq_class(M1*M2) << std::endl;
			}
		}
    }
    if(argc == 4){
		unsigned char i = atoi(argv[1]);
		unsigned long long int minJ = atoi(argv[2]);
		unsigned long long int maxJ = atoi(argv[3]);
		mpq_class M2;
		mpq_class temp = mpq_class(1,pow(3,i));
		M2 = temp/(1-temp);
		for(long long int j=minJ; j < maxJ; j++){
			convCTernary(j, i);
			std::cout << M1*M2 << std::endl;
		}
	}

}
