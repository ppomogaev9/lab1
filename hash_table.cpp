#include "hash_table.hpp"
#include <exception>
#include <stdexcept>

using std::list;

namespace constants {
	uint32_t large_prime_number = 4294967291;
	uint32_t large_prime_number2 = INT_MAX;
	uint32_t chiselka = 256; 
	size_t start_capacity = 16;
	size_t overflow_coeff = 5;
	int expand_coeff = 2;
	const Value default_value("", 0);
}

using namespace constants;

HashTable::Cell::Cell(const Key& k, const Value& v) : key(k), val(v) {};

uint32_t HashTable::calc_prime_hash(const Key& key) const {
	uint64_t powered_chiselka = 1; 
	uint64_t hash = 0;

	for (unsigned char x : key) {
		hash += x * powered_chiselka;
		hash %= large_prime_number;
		powered_chiselka = (pow_chiselka * static_cast<uint64_t>(chiselka)) % large_prime_number;
	}

	return static_cast<uint32_t>(hash);
}

uint32_t HashTable::calc_hash(const Key& key) const {
	return calc_prime_hash(key) % _capacity;
}

HashTable::HashTable() : _size(0), _capacity(start_capacity) {
	_storage.resize(start_capacity);
}

const std::list<HashTable::Cell>& HashTable::get_list(const Key& k) const {
	return _storage[calc_hash(k)];
}

std::list<HashTable::Cell>& HashTable::get_list(const Key& k) {
	return _storage[calc_hash(k)];
}

list<HashTable::Cell>::iterator HashTable::find(const Key& k, list<Cell>& List) {
	std::list<Cell>::iterator result = List.begin();
	for (result; result != List.end(); ++result) {
		if (result->key == k)
			break;
	}
	return result;
}

list<HashTable::Cell>::iterator HashTable::find(const Key& k) {
	return find(k, get_list(k));
}

list<HashTable::Cell>::const_iterator HashTable::find(const Key& k, const list<Cell>& List) const {
	std::list<Cell>::const_iterator result = List.begin();
	for (result; result != List.end(); ++result) {
		if (result->key == k)
			break;
	}
	return result;
}

list<HashTable::Cell>::const_iterator HashTable::find(const Key& k) const {
	return find(k, get_list(k));
}

bool HashTable::prime_insert(const Key& k, const Value& v)
{
	if (this->contains(k)) {
		find(k)->val = v;
		return false;
	}
	get_list(k).emplace_back(k, v);
	return true;
}

HashTable& HashTable::operator=(const HashTable& b) {
	_storage = b._storage;
	_size = b._size;
	_capacity = b._capacity;
	return *this;
}

HashTable::HashTable(const HashTable& b): _storage(b._storage), _size(b._size), _capacity(b._capacity) {}

void HashTable::swap(HashTable& b) {
	HashTable t = *this;
	*this = b;
	b = t;
}

void HashTable::clear() {
	_storage.clear();
	_size = 0;
	_capacity = start_capacity;
	_storage.resize(_capacity);
}

bool HashTable::erase(const Key& k) {
	if (this->contains(k)) {
		get_list(k).erase(find(k));
		--_size;
		return true;
	}
	return false;
}

bool HashTable::insert(const Key& k, const Value& v) {
	if (_size * overflow_coeff >= _capacity) {
		_capacity *= expand_coeff;

		std::vector<std::list<Cell>> old_storage = _storage;
		_storage.clear();
		_storage.resize(_capacity);

		for (auto list : old_storage) {
			for (auto cell : list) {
				prime_insert(cell.key, cell.val);
			}
		}
	}

	++_size;

	return prime_insert(k, v);
}

bool HashTable::contains(const Key& k) const {
	if (_size == 0)
		return false;
	return find(k) != get_list(k).end();
}

Value& HashTable::operator[](const Key& k) {
	if (!this->contains(k)) {
		this->insert(k, default_value);
	}
	return find(k)->val;
}

Value& HashTable::at(const Key& k) {
	if (this->contains(k))
		return find(k)->val;
	else
		throw std::out_of_range("at threw to you \"out of range\"-excpetion");
}

const Value& HashTable::at(const Key& k) const {
	if (this->contains(k))
		return find(k)->val;
	else
		throw std::out_of_range("at threw to you \"out of range\"-excpetion");
}

size_t HashTable::size() const {
	return _size;
}

bool HashTable::empty() const {
	if (_size == 0)
		return true;
	return false;
}

bool operator==(const HashTable& a, const HashTable& b)
{
	if (a._size != b._size)
		return false;
	if (a._size == 0)
		return true;
	for (int i = 0; i < a._storage.size(); ++i) {
		for (auto j = a._storage[i].begin(); j != a._storage[i].end(); ++j) {
			if (!b.contains((*j).key))
				return false;
		}
	}
	return true;
}

bool operator!=(const HashTable& a, const HashTable& b) {
	return !(a == b);
}
