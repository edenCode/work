#include "HashTable.hh"

HashTable::HashTable() 
{
}

HashTable::~HashTable()
{
}

HashTable::Iterator::Iterator() 
{
}

HashTable::Iterator::~Iterator()
{

}

void* HashTable::RemoveNext()
{
	char const* key = 0;
	Iterator* iter = Iterator::create(*this);
	void* removedValue = iter->next(key);
	if (removedValue != 0) Remove(key);

	delete iter;
	return removedValue;
}

void* HashTable::getFirst() 
{
	char const* key;
	Iterator* iter = Iterator::create(*this);

	void* firstValue = iter->next(key);

	delete iter;
	return firstValue;
}
