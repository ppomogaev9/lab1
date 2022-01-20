#include <gtest/gtest.h>
#include "hash_table.hpp"

namespace testing_constants {
	const Value default_value("", 0);
}
using namespace testing_constants;

bool operator== (const Value& a, const Value& b) {
	return a.age == b.age && a.name == b.name;
}

//swap check
TEST(SwapCheck, Check) {
	HashTable A;
	HashTable B;
	A.insert("1", default_value);
	A.insert("2", default_value);
	B.insert("3", default_value);
	A.swap(B);
	EXPECT_TRUE(B.contains("1"));
	EXPECT_FALSE(A.contains("1"));
	EXPECT_TRUE(A.contains("3"));
	EXPECT_FALSE(B.contains("3"));
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
	A.insert("f", default_value);
	A.insert("g", default_value);
	A.insert("h", default_value);
	A.insert("i", default_value); // expand
	EXPECT_TRUE(A.contains("f"));
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

//copy constructor check
TEST(CopyConstuctorCheck, ContentCheck) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	HashTable B = A;
	EXPECT_TRUE(A == B);
}

TEST(CopyConstuctorCheck, PtrsDOESNTinherited) {
	HashTable A;
	A.insert("1", default_value);
	HashTable B = A;
	EXPECT_TRUE(&B["1"] != &A["1"]);
}

//clear check
TEST(ClearCheck, EmptyHTCheck) {
	HashTable A;
	EXPECT_NO_FATAL_FAILURE(A.clear());
	EXPECT_TRUE(A == HashTable());
}

TEST(ClearCheck, EraseHTtoEmpty) {
	HashTable A;
	A.insert("1", default_value);
	A.clear();
	EXPECT_TRUE(A == HashTable());
}

// erase check
TEST(EraseCheck, DeleteFakeKey) {
	HashTable A;
	A.insert("1", default_value);
	EXPECT_NO_FATAL_FAILURE(A.erase("2"));
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
	A.erase("1");
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

// contains check
TEST(ContainsCheck, EmptyHTCheck) {
	HashTable A;
	EXPECT_FALSE(A.contains("9"));
}

TEST(ContainsCheck, DOESNTcontainFakeCell) {
	HashTable A;
	A.insert("5", default_value);
	EXPECT_FALSE(A.contains("8"));
}

TEST(ContainsCheck, DoContainTrueCell) {
	HashTable A;
	A.insert("4", default_value);
	EXPECT_TRUE(A.contains("4"));
}

// [] check
TEST(SqBracketsCheck, FakeCellCheck) {
	HashTable A;
	A["a"];
	EXPECT_TRUE(A.contains("a"));
	HashTable B;
	B.insert("a", default_value);
	EXPECT_TRUE(A == B);
}

TEST(SqBracketsCheck, TrueLoadCellCheck) {
	HashTable A;
	A.insert("a", default_value);
	EXPECT_TRUE(A["a"] == default_value);
}

TEST(SqBracketsCheck, ChangingTrueElem) {
	HashTable A;
	A.insert("t", default_value);
	A["t"] = Value("namename", 10);
	HashTable B;
	B.insert("t", Value("namename", 10));
	EXPECT_EQ(A, B);
}

TEST(SqBracketsCheck, ChangingFakeElem) {
	HashTable A;
	A["t"] = Value("namename", 10);
	HashTable B;
	B.insert("t", Value("namename", 10));
	EXPECT_EQ(A, B);
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
	A.insert("5", default_value); 
	EXPECT_EQ(A.size(), 5);
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

// operator== check 
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

// operator!=
TEST(UnequalityOpCheck, EmptyTables) {
	HashTable A, B;
	EXPECT_FALSE(A != B);
}

TEST(UnequalityOpCheck, OnlyDifferentKeys) {
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
	EXPECT_TRUE(A != B);
}

TEST(UnequalityOpCheck, OnlyEqualKeys) {
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
	EXPECT_FALSE(A != B);
}

TEST(UnequalityOpCheck, EqualAndUnequalKeys) {
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
	EXPECT_TRUE(A != B);
}

TEST(UnequalityOpCheck, EqualToItself) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	EXPECT_FALSE(A != A);
}

TEST(UnequalityOpCheck, EqualKeysUnequalValues) {
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
	EXPECT_TRUE(A != B);
}

TEST(UnequalityOpCheck, DiffSizes) {
	HashTable A;
	HashTable B;
	B.insert("1", default_value);
	EXPECT_TRUE(A != B);
}

// assignment check
TEST(AssignmentOpCheck, AssignmentToItself) {
	HashTable A;
	EXPECT_NO_FATAL_FAILURE(A = A);
	A.insert("1", default_value);
	EXPECT_NO_FATAL_FAILURE(A = A);
	EXPECT_TRUE(A.contains("1"));
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
