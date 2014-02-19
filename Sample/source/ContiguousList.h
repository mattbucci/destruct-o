
#pragma once

#include "stdafx.h"

//A contiguous list is an implementation which 
//supports pushing members at the end
//and removing members from any point
//It will automatically do both of these things fairly quickly
//it is designed to contain pointers, not data
//Note, order of contents /not/ maintained!
template <class T>
class ContiguousList {
	//The array of stored data
	T ** data;
	//The current listCapacity
	unsigned int listCapacity;
	//The current listSize
	unsigned int listSize;

	//Resize data to the new listCapacity
	void resize(int newCapacity) {
		//Allocate the new storage
		T ** newData = new T*[newCapacity];
		//Copy over the data
		memcpy(newData,data,sizeof(T*)*listCapacity);
		//Clean up the old storage
		delete [] data;
		//Remember only the new storage
		data = newData;
		listCapacity = newCapacity;
	}
public:
	ContiguousList() {
		listCapacity = 10;
		listSize = 0;
		data = new T*[listCapacity];
	}
	ContiguousList(unsigned int initialCapacity) {
		listCapacity = initialCapacity;
		listSize = 0;
		data = new T*[listCapacity];
	}
	ContiguousList(const ContiguousList & original) {
		listCapacity = original.listCapacity;
		listSize = original.listSize;
		data = new T*[listCapacity];
		memcpy(data,original.data,original.listSize*sizeof(T*));
	}

	const ContiguousList & operator=(const ContiguousList & original) {
		//This could be optimized to take advantage of existing space, but for now it doesnt
		delete[] data;

		listCapacity = original.listCapacity;
		listSize = original.listSize;
		data = new T*[listCapacity];
		memcpy(data,original.data,original.listSize*sizeof(T*));
	}

	virtual ~ContiguousList() {
		delete [] data;
	}

	class iterator {
		//The iterators current position
		T ** at;
		//Build a new iterator giving it only the raw start position
		iterator(T ** at) {
			this->at = at;
		}
		friend class ContiguousList<T>;
	public:
		T* & operator*() const {
			return *at;
		}

		const iterator & operator++() {
			// Move the pointer forwards
			at++;
			return *this;
		}
		
		bool operator==(const iterator & other) const {
			return at == other.at;
		}
		bool operator!=(const iterator & other) const {
			return at != other.at;
		}

	};

	unsigned int size() {
		return listSize;
	}
	unsigned int capacity() {
		return listCapacity;
	}

	iterator begin() {
		return iterator(data);
	}
	iterator end() {
		return iterator(data+listSize);
	}

	T * operator[](const int index) const {
		return data[index];
	}
	//Now the complicated methods
	void insert(T * toInsert) {
		if (listSize == listCapacity)
			//Not enough room, make some more
			resize((int)(listCapacity * 1.5 + 4));
		//Assume the space after listSize is empty
		data[listSize++] = toInsert;
	}
	//Erase somthing based off an iterator
	//returns a new valid iterator
	iterator erase(iterator toErase) {
		_ASSERTE(toErase != end());
		_ASSERTE(listSize > 0);

		//When you remove something from the list, move something to fill its place if there is anything left
		*toErase = data[listSize-1];
		listSize--;

		//The old iterator is still valid actually, haha....
		return toErase;
	}
	//Finds the first matching entry and removes it
	//if there are no matching entries, has no effect
	void erase(T * toRemove) {
		for (unsigned int i = 0; i < listSize; i++) {
			if (data[i] == toRemove) {
				erase(iterator(data+i));
				return;
			}
		}
	}
};