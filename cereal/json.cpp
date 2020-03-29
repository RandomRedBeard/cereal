#include "json.h"

// Simplify namimg
using namespace cereal::json;

Value::Value() {
	type = OBJECT;
	value = nullptr;
}

Value::Value(Type type, void* value): type(type), value(value) {}

Value::~Value() {
    // Quick exit if value is null
    if (!value) {
        return;
    }

    switch(type) {
    case Type::NUMBER:
    case Type::STRING:
        free(value);
    }
}

Type Value::get_type() {
    return type;
}

bool Value::is_null() {
	return value == nullptr;
}

List::List() : Value(Type::LIST, new T()) {}

List::~List() {
	// Clean up contained vector
	T* list = get_value<T*>();

	// List should never store actual nullptr
	while (list->size() != 0) {
		Value* v = list->front();
		list->erase(list->begin());
		delete v;
	}

	// ~Value will not touch list
	delete list;
}

void List::add_value(Value* value) {
	if (!value) {
		return;
	}

    T* list = get_value<T*>();

    list->push_back(value);
}

Value* List::find_value(unsigned int index) {
    T* list = get_value<T*>();

    if (index >= list->size()) {
        return nullptr;
    }

    return list->at(index);
}

List::T::iterator List::begin() {
	return get_value<T*>()->begin();
}

List::T::iterator List::end() {
	return get_value<T*>()->end();
}

size_t List::size() {
	return get_value<T*>()->size();
}

Tree::Tree() : Value(Type::OBJECT, new T()) {}

Tree::~Tree() {
	// Clean up contents of tree before ~Value is called
	T* tree = get_value<T*>();

	for (std::pair<char*, Value*> p : *tree) {
		free(p.first);
		delete p.second;
	}

	delete tree;
}

Value* Tree::find_value(char* key) {
    T* tree = get_value<T*>();

    T::iterator iter = tree->find(key);
    if (iter == tree->end()) {
        return nullptr;
    }

    return iter->second;
}

Value* Tree::find_value(const char* key) {
    char* key_copy = strdup(key);

    Value* value = find_value(key_copy);
    free(key_copy);

    return value;
}

Value* Tree::find_value(std::string key) {
    return find_value(key.c_str());
}

int Tree::put_value(const char* key, Value* value) {
    // Validate key does not exist in map already
    if (find_value(key) || !value) {
        return -1;
    }

    T* tree = get_value<T*>();

    tree->insert({strdup(key), value});

    return 0;
}

Tree::T::iterator Tree::begin() {
	return get_value<T*>()->begin();
}

Tree::T::iterator Tree::end() {
	return get_value<T*>()->end();
}

size_t Tree::size() {
	return get_value<T*>()->size();
}
