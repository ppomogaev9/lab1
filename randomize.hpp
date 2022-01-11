#pragma once
#include <random>


/*	This class creates a functor that gives you random number
in some range. 

	When you requesting an instance of this class, remember :
Begin and End - left and right ends of sector, respectively. 
So End must be GOE than Begin. In other cases - UB, nothing 
is guarantied*/
class Random {
public:
	explicit Random(int Begin, int End, uint32_t BuffLen = 1);

	Random();
	
	int get_random_num();

	int get_random_num(unsigned int, unsigned int);

	void print(int);

private:
	std::mt19937 mersenne;
	std::vector<int> Buffer;
	uint32_t BuffLen;
	int Start;
	int Limit;
	int NextNumID = 0;
	static bool wasSRAND;
	
	int inner_rand();

	void buff_refilling();
};
