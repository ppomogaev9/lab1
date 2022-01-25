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
TEST(SwapCheck, SwapTwoNonEmpty) {
	HashTable A;
	HashTable B;
	A.insert("1", default_value);
	A.insert("2", default_value);
	B.insert("3", default_value);
	A.swap(B);
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
	EXPECT_EQ(A.size(), 0);
}

TEST(SwapCheck, SwapWithItself) {
	HashTable A;
	EXPECT_NO_FATAL_FAILURE(A.swap(A));
	A.insert("1", default_value);
	EXPECT_TRUE(A.contains("1"));
	EXPECT_EQ(A.size(), 1);
	EXPECT_NO_FATAL_FAILURE(A.swap(A));
}

TEST(SwapCheck, SwapEmptyWithNonEmpty) {
	HashTable A;
	HashTable B;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.swap(B);
	EXPECT_TRUE(B.contains("2"));
	EXPECT_TRUE(B.contains("1"));
	EXPECT_EQ(B.size(), 2);
	EXPECT_EQ(A.size(), 0);
}


 //insert check
TEST(InsertCheck, EmptyHTDoesntContainsKey) {
	HashTable A;
	EXPECT_FALSE(A.contains("1"));
}

TEST(InsertCheck, CheckValueAfterInsert) {
	HashTable A;
	A.insert("a", Value("name", 21));
	EXPECT_TRUE(A.contains("a"));
	EXPECT_EQ(A.at("a"), Value("name", 21));
	EXPECT_EQ(A.size(), 1);
}

TEST(InsertCheck, ExpandCorrectivityCheck) {
	HashTable A;
	A.insert("f", Value("1", 1));
	A.insert("g", Value("2", 2));
	A.insert("h", Value("3", 3));
	A.insert("i", Value("4", 4)); // expand
	EXPECT_EQ(A.size(), 4);
	EXPECT_EQ(A["f"], Value("1", 1));
	EXPECT_EQ(A["g"], Value("2", 2));
	EXPECT_EQ(A["h"], Value("3", 3));
	EXPECT_EQ(A["i"], Value("4", 4));
}

TEST(InsertCheck, OneKeyTwiceSameValue) {
	HashTable A;
	EXPECT_TRUE(A.insert("1", default_value));
	EXPECT_FALSE(A.insert("1", default_value));
}

TEST(InsertCheck, SameKeyDifferentValue) {
	HashTable A;
	EXPECT_TRUE(A.insert("pw", default_value));
	EXPECT_FALSE(A.insert("pw", Value("name", 9)));
	EXPECT_EQ(A["pw"], Value("name", 9));
}

 //copy constructor check
TEST(CopyConstuctorCheck, CopyHT_SameSizesAndSameContent) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	HashTable B = A;
	EXPECT_EQ(B.size(), 3);
	EXPECT_TRUE(B.contains("1"));
	EXPECT_TRUE(B.contains("2"));
	EXPECT_TRUE(B.contains("3"));
}

TEST(CopyConstuctorCheck, PtrsDOESNTinherited) {
	HashTable A;
	A.insert("1", default_value);
	HashTable B = A;
	EXPECT_TRUE(&B["1"] != &A["1"]);
}

TEST(CopyConstuctorCheck, CopyEmptyHT) {
	HashTable A;
	HashTable B = A;
	EXPECT_EQ(B.size(), 0);
}

TEST(CopyConstructorCheck, ChangingValueDoesntInfluenceToAnother) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	HashTable B = A;
	A["1"] = Value("102", 120);
	EXPECT_NE(B["1"], Value("102", 120));
}


 //clear check
TEST(ClearCheck, EmptyHTCheck) {
	HashTable A;
	A.clear();
	EXPECT_EQ(A.size(), 0);
}

TEST(ClearCheck, EraseHTtoEmpty) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	A.insert("4", default_value); // expand 
	A.clear();
	EXPECT_EQ(A.size(), 0);
	EXPECT_FALSE(A.contains("1"));
	EXPECT_FALSE(A.contains("2"));
	EXPECT_FALSE(A.contains("3"));
	EXPECT_FALSE(A.contains("4"));
}

 // erase check
TEST(EraseCheck, DeleteFakeKey) {
	HashTable A;
	A.insert("1", default_value);
	EXPECT_NO_FATAL_FAILURE(A.erase("2"));
	EXPECT_FALSE(A.erase("2"));
	EXPECT_EQ(A.size(), 1);
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
	A.erase("c");
	A.erase("a");
	EXPECT_EQ(A.size(), 1);
	EXPECT_TRUE(A.contains("b"));
}

TEST(EraseCheck, RetValueCheck) {
	HashTable A;
	A.insert("1", default_value);
	EXPECT_TRUE(A.erase("1"));
	EXPECT_EQ(A.size(), 0);
	EXPECT_FALSE(A.erase("1"));
	EXPECT_EQ(A.size(), 0);
}

TEST(EraseCheck, EqualHashesCheck) {
	HashTable A;
	A.insert("\001", default_value);
	A.insert("\021", default_value);
	A.erase("\001");
	EXPECT_EQ(A.size(), 1);
	EXPECT_TRUE(A.contains("\021"));
	EXPECT_FALSE(A.contains("\001"));
}

TEST(EraseCheck, EraseKeyFromEmptyHT) {
	HashTable A;
	A.erase("1");
	EXPECT_EQ(A.size(), 0);
	EXPECT_FALSE(A.contains("1"));
}

 // contains check
TEST(ContainsCheck, DoContainTrueCell) {
	HashTable A;
	A.insert("4", default_value);
	EXPECT_TRUE(A.contains("4"));
}

TEST(ContainsCheck, ErasePlusContains) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	A.insert("4", default_value);
	A.erase("1");
	A.erase("2");
	A.erase("3");
	EXPECT_FALSE(A.contains("1"));
	EXPECT_FALSE(A.contains("2"));
	EXPECT_FALSE(A.contains("3"));
	EXPECT_TRUE(A.contains("4"));
}

// [] check

TEST(SqBracketsCheck, ChangingSomeValueDoesntInfluenceTpAnother) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A["1"] = Value("122", 12);
	EXPECT_EQ(A.at("2"), default_value);
	EXPECT_EQ(A.at("1"), Value("122", 12));
}

TEST(SqBracketsCheck, NonExistingValueGivesDefaultValue) {
	HashTable A;
	A.insert("1", Value("12121", 12121));
	EXPECT_EQ(A["2"], default_value);
}

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
	A.insert("a", Value("namename", 10));
	EXPECT_EQ(A.at("a"), Value("namename", 10));
	EXPECT_EQ(A.size(), 1);
}

TEST(AtCheck, ChangingTrueElem) {
	HashTable A;
	A.insert("t", default_value);
	A.at("t") = Value("namename", 10);
	EXPECT_EQ(A.at("t"), Value("namename", 10));
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
	A.insert("a", Value("namename", 10));
	const HashTable B = A;
	EXPECT_EQ(B.at("a"), Value("namename", 10));
	EXPECT_EQ(A.size(), 1);
}

 //size check
TEST(SizeCheck, EmptyHTSize) {
	HashTable A;
	EXPECT_EQ(A.size(), 0);
}

TEST(SizeCheck, EmptyHTErase) {
	HashTable A;
	A.erase("1");
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
	A.erase("4");
	EXPECT_EQ(A.size(), 3);
	A.erase("3");
	EXPECT_EQ(A.size(), 2);
	A.erase("2");
	EXPECT_EQ(A.size(), 1);
	A.erase("1");
	EXPECT_EQ(A.size(), 0);
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
	EXPECT_EQ(A.size(), 0);
}

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

TEST(EqualityOpCheck, Reflexivity) {
	HashTable A;
	A.insert("1", Value("1", 2));
	A.insert("2", Value("2", 3));
	EXPECT_TRUE(A == A);
	EXPECT_EQ(A.size(), 2);
	EXPECT_EQ(A.at("1"), Value("1", 2));
	EXPECT_EQ(A.at("2"), Value("2", 3));
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

TEST(EqualityOpCheck, Symmetry) {
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
	EXPECT_TRUE(!(A == B) || (B == A));
	B["1"] = Value("1", 2);
	EXPECT_TRUE(!(A == B) || (B == A));
}

TEST(EqualityOpCheck, Transitivity) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	HashTable B;
	A.insert("1", default_value);
	A.insert("2", default_value);
	HashTable C;
	EXPECT_TRUE((!(A == B) || !(B == C)) || (A == C));
	C.insert("1", default_value);
	C.insert("2", default_value);
	EXPECT_TRUE((!(A == B) || !(B == C)) || (A == C)); 
	A.erase("1");
	EXPECT_TRUE((!(A == B) || !(B == C)) || (A == C)); 
	B.erase("2");
	EXPECT_TRUE((!(A == B) || !(B == C)) || (A == C));
}

 // assignment check
TEST(AssignmentOpCheck, AssignmentToItself) {
	HashTable A;
	A = A;
	EXPECT_EQ(A.size(), 0);
	A.insert("1", Value("foo", 1));
	A = A;
	EXPECT_EQ(A.size(), 1);
	EXPECT_EQ(A["1"], Value("foo", 1));
}

TEST(AssignmentOpCheck, AssignmentToAnother1) {
	HashTable A;
	A.insert("0", Value("a", 2));
	HashTable B;
	B.insert("1", Value("b", 3));
	B.insert("2", Value("b", 4));
	A = B;
	EXPECT_EQ(A.size(), 2);
	EXPECT_EQ(B.size(), 2);
	EXPECT_EQ(A.at("1"), Value("b", 3));
	EXPECT_EQ(A.at("2"), Value("b", 4));
	EXPECT_EQ(B.at("1"), Value("b", 3));
	EXPECT_EQ(B.at("2"), Value("b", 4));
}

TEST(AssignmentOpCheck, ChangingInADoesntLeadToChangingInB) {
	HashTable A;
	A.insert("1", Value("1", 2));
	HashTable B;
	B = A;
	EXPECT_EQ(B.size(), 1);
	EXPECT_EQ(B["1"], Value("1", 2));
	B["1"] = default_value;
	EXPECT_EQ(B.size(), 1);
	EXPECT_EQ(B["1"], default_value);
	EXPECT_EQ(A.size(), 1);
	EXPECT_EQ(A["1"], Value("1", 2));
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
