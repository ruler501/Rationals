#include<mpirxx.h>
#include<cmath>
#include<cstdlib>
#include<iostream>
#include<fstream>
#include<sstream>
#include<thread>
#include<future>

void convCTernary(long long int const &pq, unsigned char const &l, mpq_class &M1){
    for(int j=0; j < l; j++)
        M1 += mpq_class(int(pq >> (j) & 1)) * mpq_class(std::pow(3,j));
}

std::string printRationals(unsigned char const &n, unsigned long long int const &minJ, unsigned long long int const &maxJ){
	std::stringstream ss;
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
		ss << mpq_class(mpq_class(2)*M1*M2) << std::endl;
	}
	return ss.str();
}



int main(int argc, char* argv[]){
	int THREADS = std::thread::hardware_concurrency() != 0 ? std::thread::hardware_concurrency() : 8;
    if(argc == 2){
		for(int i = 3; i<std::min(5,atoi(argv[1])); i++){
			std::cout << printRationals(i, 1, 1<<i);
		}
		for(int i = 5; i<atoi(argv[1]); i++){
			std::vector<std::future<std::string>> futures;
			std::vector<std::string> results;
			for(int j=0; j < 2*THREADS; j++){
				futures.emplace_back(std::async(std::launch::async, printRationals, i, (j*(1<<(i-1)))/THREADS+1, ((j+1)*(1<<(i-1)))/THREADS));
			}
			for(auto& f : futures) {
				results.push_back(f.get());
			}

			for(auto& r : results){
					std::cout << r;
			}
		}
    }
    if(argc == 3){
		for(int i = atoi(argv[1]); i<std::min(5,atoi(argv[2])); i++){
			std::cout << printRationals(i, 1, 1<<i);
		}
		for(int i = 5; i<atoi(argv[2]); i++){
			std::vector<std::future<std::string>> futures;
			std::vector<std::string> results;
			for(int j=0; j < 2*THREADS; j++){
				futures.emplace_back(std::async(std::launch::async, printRationals, i, (j*(1<<(i-1)))/THREADS+1, ((j+1)*(1<<(i-1)))/THREADS));
			}
			for(auto& f : futures) {
				results.push_back(f.get());
			}

			for(auto& r : results){
					std::cout << r;
			}
		}
    }
    if(argc == 4){
		unsigned char i = atoi(argv[1]);
		unsigned long long int minJ = atoi(argv[2]);
		unsigned long long int maxJ = atoi(argv[3]);
		printRationals(i, minJ, maxJ);
	}

}
