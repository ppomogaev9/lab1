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
	// creates an ampty HT. Empty HT consist of vector of 16 empty lists(std::list)
	// so that constructor inizializes corresponding values and resizes the lists vector
	HashTable();

	~HashTable() = default;

	// Assignment operator. Copies the content of HT to another HT. As a result, 
	// left and right operands of "=" are indistinguishable
	HashTable& operator=(const HashTable& b);

	// Creates an instance of HT on base of another HT
	HashTable(const HashTable& b);

	// swap content of two HT with a third HT
	void swap(HashTable& b);

	// clears a storage(vector of lists) and assign to all inner variables default values 
	void clear();

	// if our HT contains k then "erase" calls a list corresponding to k and then with function 
	// std::list<...>::erase erases corresponding to k value and k itself from the 
	// storage(vector of lists) and returns true; if it does't then the function just returns
	// false 
	bool erase(const Key& k);

	// turns the key into a hash from 0 to the actual HT capacity using a function 
	// with even distribution
	bool insert(const Key& k, const Value& v);

	// checks if HT contains cell with the key or not. Returns the value corresponding to answer to
	// last question
	bool contains(const Key& k) const;
	
	// if HT contains requesting key then [] returns a reference to the value, corresponding to the
	// HT cell which contains that key. If it doesn't then default value inserted to HT table with that key
	Value& operator[](const Key& k);
	
	// behave the same as the operator[] except if HT doesn't contain the key. If that then 
	// "at" will throw an std::out_of_range exception
	Value& at(const Key& k);
	const Value& at(const Key& k) const;

	// returns an actual amount of keys contained in HT
	size_t size() const;

	// returns true if size of the HT equals. If HT size doesn;t equal to 0 returns false
	bool empty() const;

	// if a and b are indistinguishable, it means if their sizes are equal and they are contain 
	// equal keys, then the operator returns true. In any other cases it returns false.
	friend bool operator==(const HashTable& a, const HashTable& b);

	// behaves opositelly to operator==
	friend bool operator!=(const HashTable& a, const HashTable& b);
private:
	size_t _size;
	size_t _capacity;

	class Cell {
	public:
		Key key;
		Value val;
		Cell(const Key&, const Value&);
	};

	std::vector<std::list<Cell>> _storage;

	uint32_t calc_prime_hash(const Key&) const;
	
	uint32_t calc_hash(const Key&) const;

	const std::list<Cell>& get_list(const Key& k) const;

	std::list<Cell>& get_list(const Key& k);

	std::list<Cell>::iterator find(const Key& k, std::list<Cell>& List);

	std::list<Cell>::iterator find(const Key& k);
	
	std::list<Cell>::const_iterator find(const Key& k, const std::list<Cell>& List) const;
	
	std::list<Cell>::const_iterator find(const Key& k) const;

	bool prime_insert(const Key& k, const Value& v);
};
