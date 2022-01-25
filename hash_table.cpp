#pragma once
#include "pch.h"
#include "hash_table.hpp"
#include <exception>
#include <stdexcept>

using std::list;

HashTable::Cell::Cell(const Key& k, const Value& v) : key(k), val(v) {};

const Value HashTable::DEFAULT_VALUE = Value("", 0);

uint32_t HashTable::calc_prime_hash(const Key& key) const {
	static const uint32_t LARGE_PRIME = 4294967291;
	static const uint32_t CHARS_AMOUNT = 256;
	uint64_t powered_chars_amount = 1;
	uint64_t hash = 0;

	for (unsigned char x : key) {
		hash += x * powered_chars_amount;
		hash %= LARGE_PRIME;
		powered_chars_amount = (powered_chars_amount * static_cast<uint64_t>(CHARS_AMOUNT)) % LARGE_PRIME;
	}

	return static_cast<uint32_t>(hash);
}

uint32_t HashTable::calc_hash(const Key& key) const {
	return calc_prime_hash(key) % _storage.size(); 
}

HashTable::HashTable() : _storage(INITIAL_CAPACITY, nullptr) {}

void HashTable::free_storage(const std::vector<std::list<Cell>*>& storage, size_t elem_amount) {
	for (int i = 0; (i < storage.size()) && (elem_amount > 0); ++i) {
		if (storage[i]) {
			elem_amount -= storage[i]->size();
			delete storage[i];
		}
	}
}

// storage must be freed
void HashTable::copy_storage(const std::vector<std::list<Cell>*>& another_storage) {
	if (_storage.size() != another_storage.size())
		_storage.resize(another_storage.size());
	for (int i = 0; i < _storage.size(); ++i) {
		_storage[i] = another_storage[i] ? new std::list<Cell>(*another_storage[i]) : nullptr;
	}
}

HashTable::~HashTable() {
	this->free_storage(_storage, _size);
}

bool HashTable::prime_insert(const Key& k, const Value& v) {
	std::list<Cell>*& list = _storage[calc_hash(k)];
	if (!list)
		list = new std::list<Cell>;

	auto it = std::find_if(list->begin(), list->end(), [&k](Cell& c) { return c.key == k; }); 
	if (it != list->end()) {
		it->val = v;
		return false;
	}

	list->emplace_back(k, v);
	return true;
}

HashTable& HashTable::operator=(const HashTable& b) {
	if (this == &b)
		return *this;

	free_storage(_storage, _size);
	copy_storage(b._storage);
	_size = b._size;
	return *this;
}

HashTable::HashTable(const HashTable& b) : _size(b._size), _storage(b._storage.size(), nullptr) { 
	copy_storage(b._storage);
}

void HashTable::swap(HashTable& b) {
	std::swap(_storage, b._storage);
	std::swap(_size, b._size);
}

void HashTable::clear() {
	free_storage(_storage, _size);
	_storage.clear();
	_storage.resize(INITIAL_CAPACITY, nullptr);
	_size = 0;
}

void HashTable::storage_size_conversion(size_t new_size) {
	const std::vector<std::list<Cell>*> old_storage = std::move(_storage);
	_storage.resize(new_size, nullptr);

	for (auto list_ptr : old_storage) {
		if (!list_ptr)
			continue;
		for (auto cell : *list_ptr) {
			size_t cell_id = calc_hash(cell.key);
			if (!_storage[cell_id])
				_storage[cell_id] = new std::list<Cell>;
			_storage[cell_id]->push_back(cell);
		}
	}

	free_storage(old_storage, _size);
}

bool HashTable::erase(const Key& k) {
	std::list<Cell>*& list = _storage[calc_hash(k)];
	if (!list)
		return false;
	
	size_t prev_capacity = list->size();
	list->remove_if([&k](const Cell& c) { return c.key == k; });
	if (prev_capacity == list->size())
		return false;

	if (list->empty()) {
		delete list;
		list = nullptr;
	}

	--_size;

	if ((_storage.size() > INITIAL_CAPACITY) && (_size * OVERFLOW_COEF < _storage.size())) 
		storage_size_conversion(_storage.size() / EXPAND_COEF);

	return true;
}

bool HashTable::insert(const Key& k, const Value& v) {
	if ((_size + 1) * OVERFLOW_COEF >= _storage.size()) 
		storage_size_conversion(_storage.size() * EXPAND_COEF);

	bool result = prime_insert(k, v);
	if (result)
		++_size;

	return result;
}

HashTable::Cell* HashTable::find(const Key& k) const {
	std::list<Cell>* list = _storage[calc_hash(k)];
	if (!list)
		return nullptr;

	auto it = std::find_if(list->begin(), list->end(), [&k](const Cell& c) { return c.key == k; });
	if (it == list->end())
		return nullptr;

	return &(*it);
}

bool HashTable::contains(const Key& k) const {
	const Cell* c = find(k);
	return (c != nullptr) && (c->key == k);
}

Value& HashTable::operator[](const Key& k) {
	std::list<Cell>* list = _storage[calc_hash(k)];

	std::list<Cell>::iterator it;
	if ((!list) || ((it = std::find_if(list->begin(), list->end(), [&k](Cell& c) { return c.key == k; })) == list->end())) {
		insert(k, DEFAULT_VALUE);
		list = _storage[calc_hash(k)];
		return list->back().val;
	}

	return it->val;
}
 
const Value& HashTable::const_at(const Key& k) const {
	const std::list<Cell>* list = _storage[calc_hash(k)];
	if (!list)
		throw std::out_of_range("at threw to you \"out of range\"-exception");
	auto it = std::find_if(list->begin(), list->end(), [&k](const Cell& c) { return c.key == k; }); //const find if
	if (it != list->end())
		return it->val;
	else
		throw std::out_of_range("at threw to you \"out of range\"-exception");
}

Value& HashTable::at(const Key& k) {
	return const_cast<Value&>(const_at(k));
}

const Value& HashTable::at(const Key& k) const {
	return const_at(k);
}

size_t HashTable::size() const {
	return _size;
}

bool HashTable::empty() const {
	return _size == 0;
}

bool operator==(const HashTable& a, const HashTable& b) {
	if (a._size != b._size)
		return false;
	for (int i = 0; i < a._storage.size(); ++i) {
		if (!a._storage[i])
			continue;
		for (auto j = a._storage[i]->begin(); j != a._storage[i]->end(); ++j) {
			HashTable::Cell* cell = b.find(j->key);
			if (cell == nullptr || ((cell->val.age != j->val.age) && (cell->val.name != j->val.name)))
				return false;
		}
	}
	return true;
}

bool operator!=(const HashTable& a, const HashTable& b) {
	return !(a == b);
}
