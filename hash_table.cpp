#include "hash_table.hpp"
#include <exception>
#include <stdexcept>

using std::list;

HashTable::Cell::Cell(const Key& k, const Value& v) : key(k), val(v) {};

uint32_t HashTable::calc_prime_hash(const Key& key) const {
	const uint32_t large_prime_number = 4294967291;
	const uint32_t characters_amount = 256;
	uint64_t powered_chars_amount = 1;
	uint64_t hash = 0;

	for (unsigned char x : key) {
		hash += x * powered_chars_amount;
		hash %= large_prime_number;
		powered_chars_amount = (powered_chars_amount * static_cast<uint64_t>(characters_amount)) % large_prime_number;
	}

	return static_cast<uint32_t>(hash);
}

uint32_t HashTable::calc_hash(const Key& key) const {
	return calc_prime_hash(key) % _capacity;
}

HashTable::HashTable() : _capacity(_start_capacity), _storage(_start_capacity, nullptr) {}

HashTable::~HashTable() {
	for (int i = 0; i < _storage.size(); ++i) {
		if (_storage[i])
			delete _storage[i];
	}
}

list<HashTable::Cell>::iterator HashTable::find(const Key& k, list<Cell>* list) {
	std::list<Cell>::iterator result = list->begin();
	for (result; result != list->end(); ++result) {
		if (result->key == k)
			break;
	}
	return result;
}

list<HashTable::Cell>::const_iterator HashTable::find(const Key& k, const list<Cell>* list) const {
	std::list<Cell>::const_iterator result = list->begin();
	for (result; result != list->end(); ++result) {
		if (result->key == k)
			break;
	}
	return result;
}

bool HashTable::prime_insert(const Key& k, const Value& v)
{
	std::list<Cell>*& list = _storage[calc_hash(k)];
	if (!list) 
		list = new std::list<Cell>;

	auto it = find(k, list);
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

	for (auto& list : _storage) {
		if (!list)
			continue;
		delete list;
		list = nullptr;
	}

	_storage.resize(b._storage.size(), nullptr);

	for (int i = 0; i < _storage.size(); ++i) {
		if (!b._storage[i])
			continue;
		_storage[i] = new std::list<Cell>;
		for (auto cell : *b._storage[i])
			_storage[i]->push_back(cell);
	}

	_size = b._size;
	_capacity = b._capacity;
	return *this;
}

HashTable::HashTable(const HashTable& b): _size(b._size), _capacity(b._capacity), _storage(b._storage.size(), nullptr) {
	for (int i = 0; i < _storage.size(); ++i) {
		if (!b._storage[i])
			continue;
		_storage[i] = new std::list<Cell>;
		for (auto cell : *b._storage[i])
			_storage[i]->push_back(cell);
	}
}

void HashTable::swap(HashTable& b) {
	std::swap(_storage, b._storage);
	std::swap(_size, b._size);
	std::swap(_capacity, b._capacity);
}

void HashTable::clear() {
	for (int i = 0; i < _storage.size(); ++i) {
		if (_storage[i])
			delete _storage[i];
	}
	_storage.clear();
	_storage.resize(_start_capacity, nullptr);
	_size = 0;
	_capacity = _start_capacity;
	_storage.resize(_capacity);
}

bool HashTable::erase(const Key& k) {
	std::list<Cell>*& list = _storage[calc_hash(k)];
	if (!list)
		return false;
	auto it = find(k, list);
	if (it == list->end())
		return false;
	list->erase(it);
	if (list->size() == 0) {
		delete list;
		list = nullptr;
	}

	--_size;
	if ((_capacity > _start_capacity) && (_size * _overflow_coeff < _capacity)) {
		_capacity /= _expand_coeff;

		const std::vector<std::list<Cell>*> old_storage = _storage;
		_storage.clear();
		_storage.resize(_capacity, nullptr);

		for (auto list_ptr : old_storage) {
			if (!list_ptr)
				continue;
			for (auto cell : *list_ptr)
				prime_insert(cell.key, cell.val);
		}
	}

	return true;
}

bool HashTable::insert(const Key& k, const Value& v) {
	if ((_size + 1) * _overflow_coeff >= _capacity) {
		_capacity *= _expand_coeff;

		const std::vector<std::list<Cell>*> old_storage = _storage;
		_storage.clear();
		_storage.resize(_capacity, nullptr);

		for (auto list_ptr : old_storage) {
			if (!list_ptr)
				continue;
			for (auto cell : *list_ptr)
				prime_insert(cell.key, cell.val);
		}
	}
	
	bool result = prime_insert(k, v);
	if (result)
		++_size;
	return result;
}

// unfortunatelly, std::find doesn't work
bool HashTable::contains(const Key& k) const {
	if (_size == 0)
		return false;
	const std::list<Cell>* list = _storage[calc_hash(k)];
	if (!list)
		return false;
	std::list<Cell>::const_iterator it = find(k, list);
	return it != list->end();
}

Value& HashTable::operator[](const Key& k) {
	const Value default_value("", 0);
	std::list<Cell>* list = _storage[calc_hash(k)];
	
	if (!list) {
		insert(k, default_value);
	}
	else {
		auto it = find(k, list);
		if (it == list->end()) {
			insert(k, default_value);
		}
	}

	return find(k, _storage[calc_hash(k)])->val;
}

Value& HashTable::at(const Key& k) {
	std::list<Cell>* list = _storage[calc_hash(k)];
	if (!list)
		throw std::out_of_range("at threw to you \"out of range\"-excpetion");
	auto it = find(k, list);
	if (it != list->end())
		return it->val;
	else
		throw std::out_of_range("at threw to you \"out of range\"-excpetion");
}

const Value& HashTable::at(const Key& k) const {
	const std::list<Cell>* list = _storage[calc_hash(k)];
	if (!list)
		throw std::out_of_range("at threw to you \"out of range\"-excpetion");
	auto it = find(k, list);
	if (it != list->end())
		return it->val;
	else
		throw std::out_of_range("at threw to you \"out of range\"-excpetion");
}

size_t HashTable::size() const {
	return _size;
}

bool HashTable::empty() const {
	return _size == 0;
}

bool HashTable::contains_both(const Key& k, const Value& v) const {
	const std::list<Cell>* list = _storage[calc_hash(k)];
	if (!list)
		return false;
	auto it = find(k, list);
	if (it == list->end())
		return false;
	if (it->val.age != v.age || it->val.name != v.name)
		return false;
	return true;
}

bool operator==(const HashTable& a, const HashTable& b)
{
	if (a._size != b._size)
		return false;
	if (a._size == 0)
		return true;
	for (int i = 0; i < a._storage.size(); ++i) {
		if (!a._storage[i])
			continue;
		for (auto j = a._storage[i]->begin(); j != a._storage[i]->end(); ++j) {
			if (!b.contains_both((*j).key, (*j).val))
				return false;
		}
	}
	return true;
}

bool operator!=(const HashTable& a, const HashTable& b) {
	return !(a == b);
}
