#include <gtest/gtest.h>
#include "hash_table.hpp"
#include "miscellaneous.hpp"
#include "randomize.hpp"

namespace testing_constants {
	static const unsigned int NUM_TO_INSERT = 100;
	unsigned int def_HT_size = 100;
	unsigned int large_test = 1000;
	unsigned int age_min = 0;
	unsigned int age_max = 100;
	unsigned int string_len = 20;
	unsigned int large_string_len = 100;
	const Key key_example = "\001mf\013dmmd\00510&8|\022\0214";
	const std::string name_example = "Vitalya";
	const Value default_value("", 0);
	const Value value_example(name_example, age_max);
}

using namespace testing_constants;

using Cell = std::pair<Key, Value>;

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

bool operator== (const Value& a, const Value& b) {
	return a.age == b.age && a.name == b.name;
}

bool operator!= (const Value& a, const Value& b) {
	return !(a == b);
}

std::vector<Cell> random_table_filling(HashTable& table, int amount) {
	Random randomize;
	std::vector<Cell> ret;
	for (int i = 0; i < amount; ++i) {
		Key key = random_string_generator();
		Value val(random_name_generator(), randomize.get_random_num(age_min, age_max));
		table.insert(key, val);
		ret.emplace_back(key, val);
	}
	return ret;
}

std::vector<Cell> rand_fill_without_reps(HashTable& table, int amount) {
	Random randomize;
	std::vector<Cell> ret;
	for (int i = 0; i < amount; ++i) {
		Key key = random_string_generator();
		while (table.contains(key))
			key = random_string_generator();
		Value val(random_name_generator(), randomize.get_random_num(age_min, age_max));
		table.insert(key, val);
		ret.emplace_back(key, val);
	}
	return ret;
}

Key give_me_different_key(Key key, unsigned int key_length = string_len) {
	Key res = random_string_generator(key_length);
	while (key == res) 
		res = random_string_generator(key_length);
	return res;
}

TEST(PrepChecks, DefConsturctorCheck) {
	EXPECT_NO_FATAL_FAILURE(HashTable());
    // CR: check size = 0
}

// insert elementary check
TEST(PrepChecks, ElementaryInsertCheck) {
	HashTable A;
	EXPECT_NO_FATAL_FAILURE(A.insert(key_example, value_example));
	EXPECT_NO_FATAL_FAILURE(A.insert(key_example, value_example));
	EXPECT_NO_FATAL_FAILURE(A.insert(key_example, Value(random_name_generator(), age_min)));
	EXPECT_NO_FATAL_FAILURE(A.insert(random_string_generator(), value_example));
	for (int i = 0; i < large_test; ++i) {
		EXPECT_NO_FATAL_FAILURE(A.insert(random_string_generator(), value_example));
	}
}

// insert, returnable value check
TEST(PrepChecks, ElementaryCheckOfRetValOfInsert) {
	HashTable A;
	EXPECT_TRUE(A.insert(key_example, value_example));
	EXPECT_FALSE(A.insert(key_example, value_example));
	EXPECT_FALSE(A.insert(key_example, Value(random_name_generator(), age_min)));
	for (int i = 0; i < NUM_TO_INSERT; ++i) {
		Key key = random_name_generator(i + string_len);
		EXPECT_TRUE(A.insert(key, value_example));
		EXPECT_FALSE(A.insert(key, value_example));
	}
}

// contains elementary check: randomly filling HT and then check if contains returns true 
// on inserted numbers
// CR: remove test
TEST(PrepChecks, ElementaryContainsCheck) {
	HashTable A;
	std::vector<Cell> A_content = rand_fill_without_reps(A, def_HT_size);
    EXPECT_EQ(def_HT_size, A.size());
	for (auto x : A_content) {
		EXPECT_TRUE(A.contains(x.first));
	}
}

// randomly filling HT and then check if size() returns a number that equals to the amount
// of different keys inserted it
// CR: remove test
TEST(PrepChecks, ElementarySizeCheck) {
	HashTable A;
	for (size_t i = 1; i <= large_test; ++i) {
		Key key = random_name_generator(string_len);
		while (A.contains(key))
			key = random_name_generator(string_len);
		Value val(random_name_generator(), age_min);
		A.insert(key, val);
		EXPECT_TRUE(A.size() == i);
	}
}

// [] elementary check
TEST(PrepChecks, ElementarySquareBrackCheck) {
	HashTable A;
	A.insert(key_example, value_example);
	EXPECT_NO_FATAL_FAILURE(A[key_example]);
	EXPECT_EQ(A[key_example], value_example);
	EXPECT_EQ(A[give_me_different_key(key_example)], default_value);

	std::vector<Cell> cells = rand_fill_without_reps(A, NUM_TO_INSERT);

	for (int i = 0; i < NUM_TO_INSERT; ++i) {
		EXPECT_NE(A[cells[i].first], default_value);
	}
}

// insert check: randomly filling HT and then check if HT contains inserted numbers or not and
// if contains returns true: check if values corresponding to this keys are equal to the
// values that we actually insert with this keys or not
TEST(InsertCheck, Check) {
	HashTable A;
	std::vector<Cell> Cells = rand_fill_without_reps(A, large_test);
	
	EXPECT_TRUE(A.size() == large_test);
	for (int i = 0; i < large_test; ++i) {
		EXPECT_TRUE(A.contains(Cells[i].first));
		EXPECT_EQ(A[Cells[i].first], Cells[i].second);
	}
}

// check contains: check_contains(N, M) consist of some random tests with N inserting
// in HT keys and M fake keys that don't inserting in HT. Then starts random 
// requests to contains about was some key inserted in HT or not 
TEST(ContainsCheck, UltimateCheck) {
	EXPECT_TRUE(check_contains(0, 1));
	EXPECT_TRUE(check_contains(1, 0));
	EXPECT_TRUE(check_contains(0, NUM_TO_INSERT));
	EXPECT_TRUE(check_contains(NUM_TO_INSERT, 0));
	EXPECT_TRUE(check_contains(NUM_TO_INSERT, NUM_TO_INSERT));
	EXPECT_TRUE(check_contains(large_test, large_test));
}

TEST(EmptyCheck, Check) {
	HashTable A;
	EXPECT_TRUE(A.empty());
	A.insert(key_example, value_example);
	EXPECT_FALSE(A.empty());
}

// elementary clear check 
TEST(PrepChecks, ElementaryEmptyCheck) {
	HashTable A;
	random_table_filling(A, def_HT_size);
	A.clear();
	EXPECT_EQ(A, HashTable());
}

// check ==, !=
TEST(CheckEqOps, CheckEquiSizeHT_Eq) {
	Random randomizer;
	HashTable A;
	HashTable B;
	int keys_num = randomizer.get_random_num(1, NUM_TO_INSERT);
	for (int i = 0; i < keys_num; ++i) {
		Key key = random_string_generator();
		Value v(random_name_generator(), randomizer.get_random_num(1, NUM_TO_INSERT));
		A.insert(key, v);
		B.insert(key, v);
	}
	EXPECT_TRUE(A == B);
	EXPECT_FALSE(A != B);

	HashTable C;
	HashTable D;
	EXPECT_TRUE(C == D);
    EXPECT_FALSE(C != D);
}

TEST(CheckEqOps, CheckEquiSizeHT_NotEq) {
	Random randomizer;
	HashTable A;
	HashTable B;
	int keys_num = randomizer.get_random_num(1, NUM_TO_INSERT);
	for (int i = 0; i < keys_num; ++i) {
		Key key = random_string_generator();
		Value v(random_name_generator(), randomizer.get_random_num(age_min, age_max));
		A.insert(key, v);
		key = random_string_generator();
		v.name = random_name_generator();
		v.age = randomizer.get_random_num(age_min, age_max);
		B.insert(key, v);
	}
    // CR: might fail
	EXPECT_FALSE(A == B);
	EXPECT_TRUE(A != B);
}

TEST(CheckEqOps, CheckDiffSizeHT) {
	Random randomizer;
	int sizeA = randomizer.get_random_num(1, NUM_TO_INSERT);
	int sizeB = sizeA;
	while (sizeB == sizeA)
		sizeB = randomizer.get_random_num(1, NUM_TO_INSERT);
	Key key = "test";
	Value val("testName", 0);
	HashTable A;
	for (int i = 0; i < sizeA; ++i) {
		A.insert(key, val);
	}
	HashTable B;
	for (int i = 0; i < sizeB; ++i) {
		B.insert(key, val);
	}
    // CR: probably wrong test?
	EXPECT_FALSE(A == B);
	EXPECT_TRUE(A != B);
}

// check erase
int find(Key key, std::vector<Cell> V) {
	int amount = 0;
	for (int i = 0; i < V.size(); ++i) {
		if (V[i].first == key)
			++amount;
	}
	return amount;
}

bool repetition_exist(std::vector<Cell> V) {
	for (int i = 0; i < V.size(); ++i) {
		if (find(V[i].first, V) != 1)
			return true;
	}
	return false;
}

TEST(EraseCheck, EraseContainedKeys) {
	Random randomizer;
	std::vector<Cell> Cells;
	Cells.resize(NUM_TO_INSERT, Cell("", Value("", 0)));
	std::vector<bool> HT_contain;
	HT_contain.resize(NUM_TO_INSERT);

	HashTable HT;
	do {
		HT.clear();
		for (int i = 0; i < NUM_TO_INSERT; ++i) {
			Key key = random_string_generator();
			Value val(random_name_generator(), randomizer.get_random_num(age_min, age_max));
			Cell cell(key, val);
			Cells[i] = cell;
			HT_contain[i] = true;
			HT.insert(cell.first, cell.second);
		}
	} while (repetition_exist(Cells));
	
	int tests_amount = randomizer.get_random_num(1, NUM_TO_INSERT / 3);
	for (int i = 0; i < tests_amount; ++i) {
		int id = randomizer.get_random_num(1, NUM_TO_INSERT - 1);
		while (!HT_contain[id])
			id = randomizer.get_random_num(1, NUM_TO_INSERT - 1);
		HT.erase(Cells[id].first);
		HT_contain[id] = false;
	}

	for (int i = 0; i < NUM_TO_INSERT; ++i) {
		EXPECT_EQ(HT.contains(Cells[i].first), HT_contain[i]);
	}
}

// check emptiness after erasing of all elements
TEST(CheckEmpty, EmptinessOfErasedHTCheck) {
	HashTable A;
	A.insert(key_example, value_example);
	A.erase(key_example);
	EXPECT_TRUE(A.empty());
	std::vector<Cell> Cells = rand_fill_without_reps(A, NUM_TO_INSERT);
	EXPECT_FALSE(A.empty());
	for (auto x : Cells)
		A.erase(x.first);
	EXPECT_TRUE(A.empty());
}

// operator = check
TEST(AssignmentCheck, Check) {
	HashTable A;
	HashTable B = A;
	EXPECT_TRUE(A == B);
	EXPECT_NO_FATAL_FAILURE(A = A);

	random_table_filling(B, def_HT_size);
	
	EXPECT_NO_FATAL_FAILURE(B = B);
	EXPECT_NO_THROW(A = B);
	EXPECT_TRUE(A == B);
}

//swap check
TEST(SwapCheck, Check) {
	HashTable A;
	HashTable B;
	EXPECT_NO_FATAL_FAILURE(A.swap(B));
	A.insert(random_string_generator(), Value(name_example, age_min));
	HashTable C = A, D = B;
	A.swap(B);
	EXPECT_TRUE(A == D);
	EXPECT_TRUE(A != C);
	EXPECT_TRUE(B != D);
	EXPECT_TRUE(B == C);
	random_table_filling(A, def_HT_size);
	random_table_filling(B, def_HT_size);
	C = A, D = B;
	A.swap(B);
	EXPECT_TRUE(A == D);
	EXPECT_TRUE(A != C);
	EXPECT_TRUE(B != D);
	EXPECT_TRUE(B == C);
}

// at check
TEST(AtCheck, Check) {
	HashTable A;
	EXPECT_THROW(A.at(key_example), std::out_of_range);
	A.insert(key_example, value_example);
	EXPECT_NO_THROW(A.at(key_example));
	EXPECT_THROW(A.at(give_me_different_key(key_example)), std::out_of_range);
}

TEST(ConstAtCheck, Check) {
	HashTable A;
	A.insert(key_example, value_example);
	const HashTable B = A;
	EXPECT_NO_THROW(B.at(key_example));
	EXPECT_THROW(B.at(give_me_different_key(key_example)), std::out_of_range);
}
