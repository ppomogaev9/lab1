#pragma once
#include <string>
#include <vector>
#include <list>
#include <exception>
typedef std::string Key;

struct Value {
	Value(std::string n, unsigned int a = 0) : name(n), age(a) {}
	std::string name;
	unsigned int age;
};

class HashTable {
public:
	// creates an empty HT. Empty HT consist of vector of 16 empty lists(std::list)
	// so that constructor initializes corresponding values and resizes the lists vector
	HashTable();

	// frees all allocated memory
	~HashTable();

	// Assignment operator. Copies the content of HT to another HT. As a result, 
	// left and right operands of "=" are indistinguishable
	HashTable& operator=(const HashTable& b);

	// Creates an instance of HT on base of another HT
	HashTable(const HashTable& b);

	// swap content of two HT
	void swap(HashTable& b);

	// clears a storage(vector of lists) and assigns to all inner variables default values
	void clear();

	// if our HT contains k then "erase" calls a list corresponding to k and then 
	// erases corresponding to k value and k itself from the 
	// storage and returns true; if it doesn't then the function just returns
	// false. 
	bool erase(const Key& k);

	// turns the key into a hash from 0 to the actual HT capacity using a function 
	// with even distribution
	bool insert(const Key& k, const Value& v);

	// checks if HT contains cell with the key or not.
	// true if k is present in hash table, false otherwise
	bool contains(const Key& k) const;

	// if HT contains requesting key then [] returns a reference to the value, corresponding to the
	// HT cell which contains that key.
	// If it doesn't then default value inserted to HT table with that key and returns newly inserted value
	Value& operator[](const Key& k);

	// behave the same as the operator[] except if HT doesn't contain the key.
	// If that happens then "at" will throw a std::out_of_range exception
	Value& at(const Key& k);
	const Value& at(const Key& k) const;

	// returns an actual amount of keys contained in HT
	size_t size() const;

	// returns false if HT size doesn't equal 0. If it does, returns true
	bool empty() const;

	// if a and b are indistinguishable, it means that their sizes are equal and they contain
	// equal keys and equal values in any order. in this case operator returns true. In any other cases it returns false.
	friend bool operator==(const HashTable& a, const HashTable& b);
	friend bool operator!=(const HashTable& a, const HashTable& b);
private:
	static const size_t EXPAND_COEF = 2;
	static const size_t OVERFLOW_COEF = 2;
	static const size_t INITIAL_CAPACITY = 8;

	static const Value DEFAULT_VALUE;

	size_t _size = 0;

	struct Cell {
		Key key;
		Value val;
		Cell(const Key&, const Value&);
	};

	std::vector<std::list<Cell>*> _storage;

	void resize_storage(size_t new_size);

	void free_storage(const std::vector<std::list<Cell>*>&, size_t);

	void copy_storage(const std::vector<std::list<Cell>*>&);

	Cell* find(const Key&) const;

	uint32_t calc_prime_hash(const Key&) const;

	uint32_t calc_hash(const Key&) const;

	bool prime_insert(const Key& k, const Value& v);

	const Value& const_at(const Key&) const;

};
