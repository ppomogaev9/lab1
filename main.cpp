#include "hash_table.hpp"
#include "randomize.hpp"
#include <iostream>
#include "miscellaneous.hpp"
#include <clocale>

int main() {
	HashTable A;
	A.insert("jgfkhgjfj", Value("", 99));
	A.erase("jgfkhgjfj");
	std::cout << A.empty();
	return 0;
}
