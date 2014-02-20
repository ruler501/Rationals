#include <iostream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <cmath>
#include <sstream>
#include <cstdlib>

using namespace std;

ostream* outfile = &cout;

bool testCantor(unsigned long long p, unsigned long long q){
	while(q % 3 == 0){
		q /= 3;
		if (p/q == 1) return p==q;
		p %= q;
	}
	unsigned long long p_start = p;
	do{
		unsigned long long p3 = p * 3;
		if(p3/q == 1) return false;
		p = p3 % q;
	} while(p != p_start);
	return true;
}

void genRational(unsigned long long minNum, unsigned long long maxNum, int step, string *result){
    ostringstream buffer;
    for(unsigned long long q = minNum; q <= maxNum; q += step)
        for(unsigned long long p = 1; p <= q/3; p += 2){
            if(testCantor(p, q))
                for(unsigned long long i = p; i <= q/3; i *= 3)
                    if(__gcd(i,q) == 1)
                        buffer << q << ',' << 2 << endl;
            if(testCantor(++p, q))
                for(unsigned long long i = p; i <= q/3; i *= 3)
                    if(__gcd(i,q) == 1)
                        buffer << q << ',' << 2 << endl;
        }
    *result = buffer.str();
}

int main(int argc, char* argv[]){
    unsigned long long minNum, maxNum;
    int THREADS = thread::hardware_concurrency() != 0 ? thread::hardware_concurrency() : 8;
    if (argc == 2){
        minNum = pow(3, atoi(argv[1]));
        maxNum = pow(3, atoi(argv[1]) + 1);
    }
    else if (argc == 3){
        minNum = pow(3, atoi(argv[1]));
        maxNum = pow(3, atoi(argv[2]));
    }
    else if (argc == 4){
        minNum = pow(3, atoi(argv[1]));
        maxNum = pow(3, atoi(argv[2]));
        THREADS = atoi(argv[3]);
    }
    else if (argc == 5){
        minNum = atoi(argv[1]);
        maxNum = atoi(argv[2]);
        THREADS = atoi(argv[3]);
        outfile = new ofstream(argv[4]);
    }
    else return -1;
    string* tempStream = nullptr;
    thread* tempThread = nullptr;
    vector<thread*> usedThreads;
    vector<string*> results;
    for(int i = 0; i < THREADS; i++){
        tempStream = new string("");
        results.push_back(tempStream);
        tempThread = new thread(genRational, minNum + i, maxNum, THREADS, tempStream);
        usedThreads.push_back(tempThread);
    }
    for(auto i = usedThreads.begin(); i != usedThreads.end(); i++) (*i)->join();
    for(auto i = results.begin(); i != results.end(); i++) *outfile << **i ;
    return 0;
}
