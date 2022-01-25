#include "pch.h"
#include "hash_table.hpp"
#include "miscellaneous.hpp"
#include "randomize.hpp"

/*
progress:

	operator=
	1. присвоение себя
	3. Присвоение наполненной таблицы: ссылки не сохраняются, размер наследуется, 
	находящиеся элементы исчезают, элементы из другой появляются. 

	HashTable(const HashTable& a): 
	1. не передаются указателями
	2. содержимое копируется

	swap: V
	1. check
	2. swap empty
	3. with itself

	clear 
	1. clear пустой таблицы не приводит к terminate
	2. clear заполненной чем-то таблицы приводит её дефолтной

	erase
	0. удаление элемента не приводит к проблемам
	1. удаление не содержащегося в таблице элемента не приводит к проблемам
	2. повторное удаление не приводит к проблемам
	3. удаление элемента действительно его стирает
	4. удаление элемента после сокращения таблицы не приводит к потере элементов
	5. проверки возвращаемого значения. 
	6. проверка не удалится ли не то значение, если будем удалять из списка, в котором не один элемент
	
	insert:
	1. empty ничего не содержит
	2. если поклали, то содержит
	3. после расширения содержит положенное до
	4. проверка возвращаемого значения            
	5. инсерт одного ключа два раза               

	contains:
	1. пустая таблица не содержит элемента
	2. заполненная таблица не содержит несодержащийся в ней элемент
	3. заполненная таблица содержит содержащийся в ней элемент

	operator[]
	1.[] не содержащегося ведет себя правильно
	2.[] содержашегося возвращает то, что нужно
	3. изменение элемента с помощью[] приводит к ожидаемому результату
	4. изменение фейкового элемента сразу приводит к ожидаемому результату
	
	at
	1. когда надо бросает исключение, а когда не надо - нет
	2. изменение элемента с помощью at приводит к ожидаемому результату
	3. загруженный ключ корректно выдается при помощи at

	at const
	1. когда надо бросает исключение, а когда не надо - нет
	2. загруженный ключ корректно выдается при помощи at

	size:
	1. size пустой таблицы = 0
	2. от insert'а вплоть до expand'a увеличивается корректно
	3. от erase'a вплоть до reduce'a уменьшается корректно
	4. при добавлении того же ключа не увеличивается

	empty
	1. пустая таблица пуста
	2. не пустая таблица не пуста
	3. после удаления всех элементов пуста
	4. после clear пуста

	operator==
	1. если загружены одинаковые элементы, то равны
	2. пустые таблицы равны
	3. если загржены разные элементы, то не равны
	4. если есть разные элементы и одинаковые, то не равны
	5. сама себе таблица равна

	operator!=

*/

namespace testing_constants {
	const Value default_value("", 0);
}

using namespace testing_constants;

bool operator== (const Value& a, const Value& b) {
	return a.age == b.age && a.name == b.name;
}

std::vector<std::pair<Key, Value>> table_filling_evenly(HashTable& HT) {
	std::vector<std::pair<Key, Value>> result;
	Key key;
	Value val = default_value;
	for (int i = 1; i < 101; ++i) {
		key = i;
		val = Value(std::string("name") + static_cast<char>(i), i);
		HT.insert(key, val);
		result.emplace_back(key, val);
	}
	return result;
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
	EXPECT_EQ(A, HashTable());
}

TEST(ClearCheck, EraseHTtoEmpty) {
	HashTable A;
	A.insert("1", default_value);
	A.insert("2", default_value);
	A.insert("3", default_value);
	A.insert("4", default_value); // expand 
	A.clear();
	EXPECT_EQ(A, HashTable());
}

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

// contains check
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
	EXPECT_EQ(A, B);
}

TEST(SqBracketsCheck, TrueLoadCellCheck) {
	HashTable A;
	A.insert("a", Value("21", 32));
	EXPECT_EQ(A["a"], Value("21", 32));
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

// larger tests
TEST(LargeTests, ClearTest) {
	HashTable A;
	table_filling_evenly(A);
	A.clear();
	EXPECT_EQ(A, HashTable());
	many_equal_hashes(A);
	A.clear();
	EXPECT_EQ(A, HashTable());
}

TEST(LargeTests, SizeCheck) {
	HashTable A;
	table_filling_evenly(A);
	EXPECT_EQ(A.size(), 100);
	A.clear();
	many_equal_hashes(A);
	EXPECT_EQ(A.size(), 100);
}

TEST(LargeTests, EqualityOpCheck) {
	HashTable A;
	HashTable B;
	table_filling_evenly(A);
	table_filling_evenly(B);
	EXPECT_EQ(A, B);
	A.clear();
	B.clear();
	many_equal_hashes(A);
	many_equal_hashes(B);
	EXPECT_EQ(A, B);
}

TEST(LargeTests, ContainsCheck) {
	HashTable A;
	std::vector<std::pair<Key, Value>> cells_A = table_filling_evenly(A);
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
	std::vector<std::pair<Key, Value>> cells = table_filling_evenly(A);
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
	table_filling_evenly(A);
	HashTable B;
	B = A;
	EXPECT_EQ(A, B);
	A.clear();
	many_equal_hashes(A);
	B = A;
	EXPECT_EQ(A, B);
}
