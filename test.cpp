#include "hash_table.hpp"
#include "gtest/gtest.h"

namespace testing_constants {
	const Value default_value("", 0);
}

using namespace testing_constants;

bool operator==(const Value& a, const Value& b) {
	return a.age == b.age && a.name == b.name;
}

std::vector<std::pair<Key, Value>> add_100_entries(HashTable& HT) {
	std::vector<std::pair<Key, Value>> inserted;
	Key key;
	for (int i = 1; i <= 100; ++i) {
        std::stringstream ss;
        ss << "name" << i;
		key = std::to_string(i);
		Value val = Value(ss.str(), i);
		HT.insert(key, val);
		inserted.emplace_back(key, val);
	}
	return inserted;
}

std::vector<std::pair<Key, Value>> many_equal_hashes(HashTable& HT) {
	std::vector<std::pair<Key, Value>> result;
	Key key;
	key = "\001\002";
	Value val = default_value;
	for (int i = 0; i < 100; ++i) {
		val = Value(std::string("name") + static_cast<char>(i), i);
		HT.insert(key, val);
		result.emplace_back(key, val);
		key.pop_back();
		key += 2 * i + 4;
	}
	return result;
}

//swap check

/*
 * CR: tests to add:
 * - swap empty with non-empty
 * - swap 2 non-empty, different capacity
 */

TEST(SwapCheck, Check) {
	HashTable A;
	HashTable B;
	A.insert("1", default_value);
	A.insert("2", default_value);
	B.insert("3", default_value);
	A.swap(B);
    // CR: add similar checks in other swaps
	EXPECT_TRUE(B.contains("1"));
	EXPECT_TRUE(B.contains("2"));
    EXPECT_EQ(B.size(), 2);
	EXPECT_TRUE(A.contains("3"));
    EXPECT_EQ(A.size(), 1);
}

TEST(SwapCheck, SwapEmptyHTs) {
	HashTable A;
	HashTable B;
	EXPECT_NO_FATAL_FAILURE(A.swap(B));
}

TEST(SwapCheck, SwapWithItself) {
	HashTable A;
	EXPECT_NO_FATAL_FAILURE(A.swap(A));
	A.insert("1", default_value);
	EXPECT_NO_FATAL_FAILURE(A.swap(A));
}

/*
 * CR: tests to add:
 * - insert same key twice, different value (value was changed, second time returns false)
 * - insert same key twice, same value (second time returns false)
 * - insert key, check value
 * - insert keys until capacity is changed, then check that the same keys are in ht (and same size)
 *
 */

//insert check
TEST(InsertCheck, EmptyHT) {
	HashTable A;
	EXPECT_FALSE(A.contains("1"));
}

TEST(InsertCheck, ElemCheck) {
	HashTable A;
	A.insert("a", default_value);
	EXPECT_TRUE(A.contains("a"));
}

TEST(InsertCheck, ExpandCorrectivityCheck) {
	HashTable A;
	A.insert("f", Value("1", 1));
	A.insert("g", Value("2", 2));
	A.insert("h", Value("3", 3));
	A.insert("i", Value("4", 4)); // expand
    // CR: add similar checks in other insert tests
    EXPECT_EQ(A.size(), 4);
    EXPECT_EQ(A["f"], Value("1", 1));
    EXPECT_EQ(A["g"], Value("2", 2));
    EXPECT_EQ(A["h"], Value("3", 3));
    EXPECT_EQ(A["i"], Value("4", 4));
}

TEST(InsertCheck, RetValCheck) {
	HashTable A;
	EXPECT_TRUE(A.insert("1", default_value));
	EXPECT_FALSE(A.insert("1", default_value));
}

TEST(InsertCheck, DoubleInsert) {
	HashTable A;
	A.insert("pw", default_value);
	A.insert("pw", Value("name", 9));
	EXPECT_TRUE(A["pw"] == Value("name", 9));
}

/*
 * CR: tests to add:
 * - copy empty hashtable, size is 0 afterwards
 * - copy hash table b with entries, this.size == b.size, same entries are inserted
 * - copy hash table b with entries, change one of the values in b. value in this shouldn't change
 */

//copy constructor check
TEST(CopyConstuctorCheck, ContentCheck) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	HashTable B = A;
    // CR: better check all of the entries manually
	EXPECT_TRUE(A == B);
}

TEST(CopyConstuctorCheck, PtrsDOESNTinherited) {
	HashTable A;
	A.insert("1", default_value);
	HashTable B = A;
	EXPECT_TRUE(&B["1"] != &A["1"]);
}

/*
 * CR: tests to add:
 * - empty hash table clear - same size after
 * - non-empty hash table is cleared - size changed to 0, contains returns false for all of the old entries
 */

//clear check
TEST(ClearCheck, EmptyHTCheck) {
	HashTable A;
	A.clear();
	EXPECT_EQ(A, HashTable());
}

TEST(ClearCheck, EraseHTtoEmpty) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	A.insert("4", default_value); // expand 
	A.clear();
    // CR: better check manually
	EXPECT_EQ(A, HashTable());
}

/*
 * CR: tests to add:
 * - erase key from empty hash table - returns false, size is 0 afterwards
 * - erase key that is present in hash table - returns true, size changed afterwards
 * - erase key that is not present - returns false, size is the same
 * - insert many values (until resize), erase all of them but one (so hash table would shrink).
 *   check that size is 1 and entries are gone (using contains)
 * - erase same key twice, check that second time returns false
 */

// erase check
TEST(EraseCheck, DeleteFakeKey) {
	HashTable A;
	A.insert("1", default_value);
	EXPECT_NO_FATAL_FAILURE(A.erase("2"));
	EXPECT_TRUE(A.contains("1"));
}

TEST(EraseCheck, EraseDOESNTterminate) {
	HashTable A;
	A.insert("1", default_value);
	EXPECT_NO_FATAL_FAILURE(A.erase("1"));
}

TEST(EraseCheck, DoubleErase) {
	HashTable A;
	A.insert("1", default_value);
	A.erase("1");
	EXPECT_NO_FATAL_FAILURE(A.erase("1"));
}

TEST(EraseCheck, BasicCheck) {
	HashTable A;
	A.insert("1", default_value);
    // CR: same checks in other erase test methods
    EXPECT_TRUE(A.erase("1"));
    EXPECT_EQ(A.size(), 0);
    EXPECT_FALSE(A.contains("1"));
}

TEST(EraseCheck, ReduceCheck) {
	HashTable A;
	A.insert("a", default_value);
	A.insert("b", default_value);
	A.insert("c", default_value);
	A.insert("d", default_value); // expand
	A.erase("d");
	EXPECT_TRUE(A.contains("a"));
	EXPECT_TRUE(A.contains("b"));
	EXPECT_TRUE(A.contains("c"));
}

TEST(EraseCheck, RetValueCheck) {
	HashTable A;
	A.insert("1", default_value);
	EXPECT_TRUE(A.erase("1"));
	EXPECT_FALSE(A.erase("1"));
}

TEST(EraseCheck, EqualHashesCheck) {
	HashTable A;
	A.insert("\001", default_value);
	A.insert("\021", default_value);
	A.erase("\001");
	EXPECT_TRUE(A.contains("\021"));
	EXPECT_FALSE(A.contains("\001"));
}

/*
 * CR: tests to add:
 * - contains for inserted key returns true
 * - contains for inserted and then erased key firstly returns true, then false
 */

// contains check
TEST(ContainsCheck, DOESNTcontainFakeCell) {
	HashTable A;
	A.insert("5", default_value);
    // CR: what if it returns true for some other number, e.g. 7?
    // CR: I would've removed this test completely
	EXPECT_FALSE(A.contains("8"));
}

TEST(ContainsCheck, DoContainTrueCell) {
	HashTable A;
	A.insert("4", default_value);
	EXPECT_TRUE(A.contains("4"));
}

// [] check

/*
 * CR: add tests:
 * 3. [] with non-existing key returns default value
 * HT a; a.insert(foo, bar); EXPECT(a[baz], default_val);
 * 5. 2 default value inserts, change one, check that second also changed(not changed)?
 * HT a; value & v = a["foo"]; a["bar"]; v.age = 42; EXPECT(a["bar"].v, 0);
 */

TEST(SqBracketsCheck, InsertDefaultValueWhenNoKeyInTable) {
	HashTable A;
	A["a"];
    EXPECT_EQ(A.size(), 1);
    EXPECT_EQ(A.at("a"), default_value);
}

TEST(SqBracketsCheck, FindsCellAfterInsert) {
	HashTable A;
	A.insert("a", Value("21", 32));
	EXPECT_EQ(A["a"], Value("21", 32));
}

TEST(SqBracketsCheck, InsertAndThenChangeValue) {
	HashTable A;
	A.insert("t", default_value);
	A["t"] = Value("namename", 10);
    EXPECT_EQ(A.at("t"), Value("namename", 10));
}

TEST(SqBracketsCheck, Insert) {
	HashTable A;
	A["t"] = Value("namename", 10);
    EXPECT_EQ(A.at("t"), Value("namename", 10));
}

/*
 * CR: tests to add:
 * - at for inserted key returns inserted value, size is not changed
 * - at for unknown key throws exception
 * - same tests for const at
 */

// at check
TEST(AtCheck, ThrowingCheck) {
	HashTable A;
	EXPECT_THROW(A.at("1"), std::out_of_range);
	A.insert("1", default_value);
	EXPECT_NO_THROW(A.at("1"));
	EXPECT_THROW(A.at("2"), std::out_of_range);
}

TEST(AtCheck, TrueLoadCellCheck) {
	HashTable A;
	A.insert("a", default_value);
	EXPECT_EQ(A.at("a"), default_value);
}

TEST(AtCheck, ChangingTrueElem) {
	HashTable A;
	A.insert("t", default_value);
	A.at("t") = Value("namename", 10);
	HashTable B;
	B.insert("t", Value("namename", 10));
	EXPECT_EQ(A, B);
}

TEST(ConstAtCheck, ThrowingCheck) {
	const HashTable B;
	EXPECT_THROW(B.at("1"), std::out_of_range);
	HashTable A;
	A.insert("1", default_value);
	const HashTable C = A;
	EXPECT_NO_THROW(C.at("1"));
	EXPECT_THROW(C.at("2"), std::out_of_range);
}

TEST(ConstAtCheck, TrueLoadCellCheck) {
	HashTable A;
	A.insert("a", default_value);
	const HashTable B = A;
	EXPECT_EQ(B.at("a"), default_value);
}

/*
 * CR: tests to add:
 * - size = 0 for empty hashtable
 * - size is changed when values are added / removed
 * - size is still zero after erase on empty hash table
 * - size after clear is changed to 0
 * - if size == 0 then empty returns true (also check opposite case)
 */

//size check
TEST(SizeCheck, EmptyHTSize) {
	HashTable A;
	EXPECT_EQ(A.size(), 0);
}

TEST(SizeCheck, Inserting) {
	HashTable A;
	A.insert("1", default_value);
	EXPECT_EQ(A.size(), 1);
	A.insert("2", default_value);
	EXPECT_EQ(A.size(), 2);
	A.insert("3", default_value);
	EXPECT_EQ(A.size(), 3);
	A.insert("4", default_value); // expand
	EXPECT_EQ(A.size(), 4);
}

TEST(SizeCheck, Erasing) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	A.insert("4", default_value); // expand
	A.insert("5", default_value);
	EXPECT_EQ(A.size(), 5);
	A.erase("3");
	EXPECT_EQ(A.size(), 4);
	A.erase("1"); // reduce
	EXPECT_EQ(A.size(), 3);
}

TEST(SizeCheck, InsertingSameKey) {
	HashTable A;
	A.insert("12", default_value);
	A.insert("12", Value("2", 2));
	EXPECT_EQ(A.size(), 1);
}

// empty check
TEST(EmptyCheck, EmptyHTCheck) {
	HashTable A;
	EXPECT_TRUE(A.empty());
}

TEST(EmptyCheck, NonEmptyHTCheck) {
	HashTable A;
	A.insert("1", default_value);
	EXPECT_FALSE(A.empty());
}

TEST(EmptyCheck, ErasingToEmpty) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	A.insert("4", default_value);
	A.erase("1");
	A.erase("2");
	A.erase("3");
	A.erase("4");
	EXPECT_TRUE(A.empty());
}

TEST(EmptyCheck, ClearLeadsToEmpty) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	A.insert("4", default_value);
	A.clear();
	EXPECT_TRUE(A.empty());
}

/*
 * CR: tests to add:
 * - tables with different capacities but same elements are equal
 * - empty tables are equal
 * - tables with same capacities and same elements are equal
 * - transitivity: a == b && b == c -> a == c (can check only simple case with tables with same capacity)
 * - symmetry: a == b -> b == a
 * - reflexivity: a == a (also check that a is not corrupted after this check)
 * - check that tables with different values but same keys are not equal
 *
 */

// operator==
TEST(EqualityOpCheck, EmptyTables) {
	HashTable A, B;
	EXPECT_TRUE(A == B);
}

TEST(EqualityOpCheck, OnlyDifferentKeys) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	A.insert("4", default_value);
	HashTable B;
	B.insert("a", default_value);
	B.insert("b", default_value);
	B.insert("c", default_value);
	B.insert("d", default_value);
	EXPECT_FALSE(A == B);
}

TEST(EqualityOpCheck, OnlyEqualKeys) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	A.insert("4", default_value);
	HashTable B;
	B.insert("1", default_value);
	B.insert("2", default_value);
	B.insert("3", default_value);
	B.insert("4", default_value);
	EXPECT_TRUE(A == B);
}

TEST(EqualityOpCheck, EqualAndUnequalKeys) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	A.insert("4", default_value);
	HashTable B;
	B.insert("1", default_value);
	B.insert("2", default_value);
	B.insert("a", default_value);
	B.insert("4", default_value);
	EXPECT_FALSE(A == B);
}

TEST(EqualityOpCheck, EqualToItself) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	EXPECT_TRUE(A == A);
}

TEST(EqualityOpCheck, EqualKeysUnequalValues) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	A.insert("4", default_value);
	HashTable B;
	B.insert("1", default_value);
	B.insert("2", default_value);
	B.insert("3", Value("d", 21));
	B.insert("4", default_value);
	EXPECT_FALSE(A == B);
}

TEST(EqualityOpCheck, DiffSizes) {
	HashTable A;
	HashTable B;
	B.insert("1", default_value);
	EXPECT_FALSE(A == B);
}

/*
 * CR: tests to add:
 * - a = a does not corrupt hash table
 * - a = b gets all the values from b
 * - a = b. than change something in b (insert key, change value). check that a didn't change
 */

// assignment check
TEST(AssignmentOpCheck, AssignmentToItself) {
	HashTable A;
	A = A;
    // CR: similar checks in other tests
    EXPECT_EQ(A.size(), 0);
	A.insert("1", Value("foo", 1));
	A = A;
    EXPECT_EQ(A.size(), 1);
    EXPECT_EQ(A["1"], Value("foo", 1));
}

TEST(AssignmentOpCheck, AssignmentToAnother) {
	HashTable A;
	A.insert("1", default_value);
	HashTable B;
	B.insert("a", Value("B", 0));
	B.insert("b", Value("B", 0));
	A = B;
	EXPECT_TRUE(A.contains("a"));
	EXPECT_TRUE(A.contains("b"));
	EXPECT_FALSE(A.contains("1"));
	EXPECT_EQ(A.size(), 2);
	EXPECT_NE(&A["a"], &B["a"]);
}

// larger tests
TEST(LargeTests, ClearTest) {
	HashTable A;
    add_100_entries(A);
	A.clear();
	EXPECT_EQ(A, HashTable());
	many_equal_hashes(A);
	A.clear();
	EXPECT_EQ(A, HashTable());
}

TEST(LargeTests, SizeCheck) {
	HashTable A;
    add_100_entries(A);
	EXPECT_EQ(A.size(), 100);
	A.clear();
	many_equal_hashes(A);
	EXPECT_EQ(A.size(), 100);
}

TEST(LargeTests, EqualityOpCheck) {
	HashTable A;
	HashTable B;
    add_100_entries(A);
    add_100_entries(B);
	EXPECT_EQ(A, B);
	A.clear();
	B.clear();
	many_equal_hashes(A);
	many_equal_hashes(B);
	EXPECT_EQ(A, B);
}

TEST(LargeTests, ContainsCheck) {
	HashTable A;
	std::vector<std::pair<Key, Value>> cells_A = add_100_entries(A);
	HashTable B;
	std::vector<std::pair<Key, Value>> cells_B = many_equal_hashes(B);

	for (int i = 0; i < 99; ++i) {
		EXPECT_TRUE(A.contains(cells_A[i].first));
		EXPECT_FALSE(A.contains(cells_B[i].first));
		EXPECT_FALSE(B.contains(cells_A[i].first));
		EXPECT_TRUE(B.contains(cells_B[i].first));
	}
}

TEST(LargeTests, EraseCheck) {
	HashTable A;
	std::vector<std::pair<Key, Value>> cells = add_100_entries(A);
	for (int i = 0; i < 100; ++i) {
		A.erase(cells[i].first);
	}
	EXPECT_EQ(A, HashTable());
	A.clear();
	cells = many_equal_hashes(A);
	for (int i = 0; i < 100; ++i) {
		A.erase(cells[i].first);
	}
	EXPECT_EQ(A, HashTable());
}

TEST(LargeTests, AssignmentOp) {
	HashTable A;
    add_100_entries(A);
	HashTable B;
	B = A;
	EXPECT_EQ(A, B);
	A.clear();
	many_equal_hashes(A);
	B = A;
	EXPECT_EQ(A, B);
}
