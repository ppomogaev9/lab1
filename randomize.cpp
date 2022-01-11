#include "randomize.hpp"
#include <vector>
#include <random>
#include <iostream>
#include <ctime>

Random::Random(int Begin, int End, uint32_t BuffLen) : Start(Begin), Limit(End - Begin), BuffLen(BuffLen) {
	if (wasSRAND == false) {
		srand(time(0));
		int x = rand();
		wasSRAND = true;
	}
	mersenne = std::mt19937(rand());
	buff_refilling();
}

Random::Random() : Random(0, INT_MAX) {}

int Random::get_random_num() {
	if (NextNumID == BuffLen) {
		buff_refilling();
		NextNumID = 0;
	}
	return Buffer[NextNumID++];
}

void Random::print(int NumOfNums = 1) {
	for (int i = 0; i < NumOfNums; ++i)
		std::cout << get_random_num() << ' ';
}

int Random::inner_rand() {
	return Start + (mersenne() % (Limit + 1));
}
void Random::buff_refilling() {
	mersenne = std::mt19937(rand());
	if (Buffer.size() != BuffLen)
		Buffer.resize(BuffLen);
	for (uint32_t i = 0; i < BuffLen; ++i)
		Buffer[i] = inner_rand();
}

bool Random::wasSRAND = false;

int Random::get_random_num(unsigned int begin, unsigned int end) {
	return (begin + mersenne() % (end - begin + 1));
}
