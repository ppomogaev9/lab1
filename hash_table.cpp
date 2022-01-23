#include "hash_table.hpp"
#include <exception>
#include <stdexcept>

using std::list;

HashTable::Cell::Cell(const Key& k, const Value& v) : key(k), val(v) {};

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
	return calc_prime_hash(key) % _capacity;
}

HashTable::HashTable() : _storage(INITIAL_CAPACITY, nullptr) {}

HashTable::~HashTable() {
    // CR: add optimization: check if we removed all of the elements based on size_
	for (int i = 0; i < _storage.size(); ++i) {
		if (_storage[i]) {
			delete _storage[i];
        }
	}
}

// CR: inline method
list<HashTable::Cell>::iterator HashTable::find(const Key& k, list<Cell>* list) {
    return std::find_if(list->begin(), list->end(), [&k] (Cell & c) { return c.key == k; });
//	for (result; result != list->end(); ++result) {
//        auto result = list->begin();
//        if (result->key == k)
//			break;
//	}
//	return result;
}

list<HashTable::Cell>::const_iterator HashTable::find(const Key& k, const list<Cell>* list) const {
	auto result = list->begin();
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

    // CR: move to separate method and merge with clear and destructor
	for (auto& list : _storage) {
		if (!list)
			continue;
		delete list;
		list = nullptr;
	}

	_storage.resize(b._storage.size(), nullptr);

    // CR: separate method copy
	for (int i = 0; i < _storage.size(); ++i) {
		if (!b._storage[i])
			continue;
		_storage[i] = new std::list<Cell>;
        // CR: std::copy()
		for (auto cell : *b._storage[i])
			_storage[i]->push_back(cell);
	}

	_size = b._size;
	_capacity = b._capacity;
	return *this;
}

HashTable::HashTable(const HashTable& b): _size(b._size), _capacity(b._capacity), _storage(b._storage.size(), nullptr) {
    // CR: separate method copy
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
    // CR: same optimization
	for (int i = 0; i < _storage.size(); ++i) {
		if (_storage[i])
			delete _storage[i];
	}
	_storage.clear();
	_storage.resize(INITIAL_CAPACITY, nullptr);
	_size = 0;
	_capacity = INITIAL_CAPACITY;
}

bool HashTable::erase(const Key& k) {
	std::list<Cell>*& list = _storage[calc_hash(k)];
	if (!list)
		return false;
    // CR: std::remove_if
	auto it = find(k, list);
	if (it == list->end())
		return false;
	list->erase(it);
	if (list->empty()) {
		delete list;
		list = nullptr;
	}

	--_size;
    // CR: merge this and resize from insert
	if ((_capacity > INITIAL_CAPACITY) && (_size * OVERFLOW_COEF < _capacity)) {
		_capacity /= EXPAND_COEF;

		const std::vector<std::list<Cell>*> old_storage = std::move(_storage);
		_storage.resize(_capacity, nullptr);

		for (auto list_ptr : old_storage) {
			if (!list_ptr)
				continue;
			for (const auto& cell : *list_ptr)
				prime_insert(cell.key, cell.val);
		}
	}

	return true;
}

bool HashTable::insert(const Key& k, const Value& v) {
	if ((_size + 1) * OVERFLOW_COEF >= _capacity) {
		_capacity *= EXPAND_COEF;

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
	if (!_size)
		return false;
	const std::list<Cell>* list = _storage[calc_hash(k)];
	if (!list)
		return false;
    return find(k, list) != list->end();
}

Value& HashTable::operator[](const Key& k) {
    // CR: do not init every time
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

    // CR: call find only one time
	return find(k, _storage[calc_hash(k)])->val;
}

Value& HashTable::at(const Key& k) {
    // CR: move common logic to separate method (e.g. const_at)
	std::list<Cell>* list = _storage[calc_hash(k)];
	if (!list)
		throw std::out_of_range("at threw to you \"out of range\"-exception");
	auto it = find(k, list);
	if (it != list->end())
		return it->val;
	else
		throw std::out_of_range("at threw to you \"out of range\"-exception");
}

const Value& HashTable::at(const Key& k) const {
	const std::list<Cell>* list = _storage[calc_hash(k)];
	if (!list)
		throw std::out_of_range("at threw to you \"out of range\"-exception");
	auto it = find(k, list);
	if (it != list->end())
		return it->val;
	else
		throw std::out_of_range("at threw to you \"out of range\"-exception");
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
	for (int i = 0; i < a._storage.size(); ++i) {
		if (!a._storage[i])
			continue;
		for (auto j = a._storage[i]->begin(); j != a._storage[i]->end(); ++j) {
            // CR: add find method, reuse in contains
//            Cell * cell = b.find(j->key);
//            if (cell == nullptr || cell.val != j->val) return false;
			if (!b.contains_both((*j).key, (*j).val))
				return false;
		}
	}
	return true;
}

bool operator!=(const HashTable& a, const HashTable& b) {
	return !(a == b);
}
