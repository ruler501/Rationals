#include <iostream>
#include <algorithm>
#include <thread>
#include <cmath>

using namespace std;

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

void genRational(unsigned long long minNum, unsigned long long maxNum, int jump, unsigned long long *result){
    for(unsigned long long q = minNum; q <= maxNum; q += jump)
        for(unsigned long long p = 1; p < q/3; p++)
            if(p % 3 != 0 && testCantor(p, q) && __gcd(p,q) == 1)
                for(unsigned long long i = p; i <= q/3; i *= 3)
                        (*result) += 2*(__gcd(i,q) == 1);
}

int main(int argc, char* argv[]){
    int n = argc, THREADS = thread::hardware_concurrency() != 0 ? thread::hardware_concurrency() : 8;
    unsigned long long minNum = pow(3, n), *tempLongLong = nullptr, maxNum = pow(3, n+1), result = 0;
    vector<thread*> usedThreads;
    vector<unsigned long long*> results;
    thread* tempThread = nullptr;
    for(int i = 0; i < THREADS; i++){
        tempLongLong = new unsigned long long(0);
        results.push_back(tempLongLong);
        tempThread = new thread(genRational, minNum + i, maxNum, THREADS, tempLongLong);
        usedThreads.push_back(tempThread);
    }
    for(auto i = usedThreads.begin(); i != usedThreads.end(); i++) (*i)->join();
    for(auto i = results.begin(); i != results.end(); i++) result += **i;
    cout << n << " " << result << endl;
}
