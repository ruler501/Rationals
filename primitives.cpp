#include<mpirxx.h>
#include<cmath>
#include<cstdlib>
#include<iostream>
#include<fstream>
#include<sstream>
#include<thread>
#include<future>
#include<mutex>

std::fstream outfile;

int THREADS = std::thread::hardware_concurrency() != 0 ? std::thread::hardware_concurrency() : 8;
std::mutex outMutex;


inline void convCTernary(long long int const &pq, unsigned char const &l, mpq_class &M1){
    for(int j=0; j < l; j++)
        M1 += mpq_class(int(pq >> (j) & 1)) * mpq_class(std::pow(3,j));
}

void printRationals(unsigned char const &n, unsigned long long int const &minJ, unsigned long long int const &maxJ){
	std::stringstream ss;
	mpq_class M2(1, pow(3,n) - 1);
	for(unsigned long long int j=minJ; j < maxJ;){
		std::stringstream ss;
		for(unsigned long long int newMaxJ=j+((minJ-maxJ)>>6 > 1 ? (minJ-maxJ)>>6 : minJ-maxJ); j < newMaxJ && j <= maxJ; j++){
			bool skip = false;
			for(int i = 1; i<=n/2 && !skip; i++){
				if(n%i == 0){
					if(j  % (((1<<n)-1)/((1<<i)-1)) == 0) skip = true;
				}
			}
			if(skip) continue;
			mpq_class M1(0);
			convCTernary(j, n, M1);
			ss << mpq_class(mpq_class(2)*M1*M2) << std::endl;
		}
		outMutex.lock();
		outfile << ss.str();
		outMutex.unlock();
	}
}



int main(int argc, char* argv[]){
	std::vector<std::thread> ourThreads;
	outfile.open("out.csv", std::fstream::out);
    if(argc == 2){
		for(int i = 3; i<std::min(5,atoi(argv[1])); i++){
			printRationals(i, 1, 1<<i);
		}
		for(int i = 5; i<atoi(argv[1]); i++){
			for(int j=0; j < 2*THREADS; j++){
				ourThreads.push_back(std::thread(printRationals, i, (j*(1<<(i-1)))/THREADS+1, ((j+1)*(1<<(i-1)))/THREADS));
			}
		}
    }
    else if(argc == 3){
		for(int i = atoi(argv[1]); i<std::min(5,atoi(argv[2])); i++){
			printRationals(i, 1, 1<<i);
		}
		for(int i = 5; i<atoi(argv[2]); i++){
			for(int j=0; j < 2*THREADS; j++){
				ourThreads.push_back(std::thread(printRationals, i, (j*(1<<(i-1)))/THREADS+1, ((j+1)*(1<<(i-1)))/THREADS));
			}
		}
    }
    else if(argc == 4){
		unsigned char i = atoi(argv[1]);
		unsigned long long int minJ = atoi(argv[2]);
		unsigned long long int maxJ = atoi(argv[3]);
		printRationals(i, minJ, maxJ);
	}

	for(auto& t : ourThreads){
		t.join();
	}
}
