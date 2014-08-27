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

int THREADS = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 8;
std::mutex outMutex;

std::default_random_engine generator;

inline void convCTernary(unsigned long long int const &pq, unsigned char const &l, mpq_class &M1){
    for(int j=0; j < l; j++)
        M1 += mpq_class(int(pq >> (j) & 1)) * mpq_class(std::pow(3,j));
}

inline void convCTernaryOpt(unsigned long long int const &pq, unsigned char const &l, mpq_class &M1, mpq_class const &lastR){
	int length=0;
	if (pq != 0) for(int i = 0; i < l; i++){
		if (!((pq-1 >> i)&1)){
			length = i;
			break;
		}
	}
	//length = __builtin_ctz(~last);
	M1 = lastR;
    for(int j=0; j < length; j++)
        M1 -= mpq_class(std::pow(3,j));
	M1 += mpq_class(std::pow(3,length));
}

void printRationals(unsigned char const &n, unsigned long long int const &minJ, unsigned long long int const &maxJ){
	std::stringstream ss;
	std::uniform_int_distribution<unsigned long long int> distribution(1,(minJ-maxJ)>>5);
	mpq_class M2(1, pow(3,n) - 1), M1(0), lastR(0);
	convCTernary(minJ, n, M1);
	outMutex.lock();
	outfile << mpq_class(mpq_class(2)*M1*M2) << std::endl;
	outMutex.unlock();
	for(unsigned long long int j=minJ+1; j < maxJ;){
		std::stringstream ss;
		unsigned long long jump = distribution(generator);
		for(unsigned long long int newMaxJ=jump > 1 ? jump: minJ-maxJ; j < newMaxJ && j <= maxJ; j++){
			bool skip = false;
			for(int i = 1; i<=n/2 && !skip; i++){
				if(n%i == 0){
					if(j  % (((1<<n)-1)/((1<<i)-1)) == 0) skip = true;
				}
			}
			if(skip) continue;
			M1 = 0;
			convCTernaryOpt(j, n, M1, lastR);
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
			for(int j=0; j < THREADS; j++){
				ourThreads.push_back(std::thread(printRationals, i, (j*(1<<(i)))/THREADS+1, ((j+1)*(1<<(i)))/THREADS));
			}
		}
    }
    else if(argc == 3){
		for(int i = atoi(argv[1]); i<std::min(5,atoi(argv[2])); i++){
			printRationals(i, 1, 1<<i);
		}
		for(int i = 5; i<atoi(argv[2]); i++){
			for(int j=0; j < THREADS; j++){
				ourThreads.push_back(std::thread(printRationals, i, (j*(1<<(i)))/THREADS+1, ((j+1)*(1<<(i)))/THREADS));
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
