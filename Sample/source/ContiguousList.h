
#pragma once

#include "stdafx.h"
#include <string.h>

//A dummy allocator to make contiguouslist's template signature
//the same as vector and list
template <class T>
class __listdummyallocator {};

//A contiguous list is an implementation which 
//supports pushing members at the end
//and removing members from any point
//It will automatically do both of these things fairly quickly
//it is designed to contain pointers, not data
//Note: order of contents /not/ maintained!
//		and allocator does not actually do anything
template <class T, class _Alloc = __listdummyallocator<T>>
class ContiguousList{
	//The array of stored data
	T * data;
	//The current listCapacity
	unsigned int listCapacity;
	//The current listSize
	unsigned int listSize;
	//Whether or not to autoreduce the capacity based off the size
	//enabled by default
	bool autoreduceCapacity;

public:
	ContiguousList() {
		autoreduceCapacity = true;
		listCapacity = 10;
		listSize = 0;
		data = new T[listCapacity];
	}
	ContiguousList(unsigned int initialCapacity) {
		autoreduceCapacity = true;
		listCapacity = initialCapacity;
		listSize = 0;
		data = new T[listCapacity];
	}
	ContiguousList(const ContiguousList & original) {
		autoreduceCapacity = original.autoreduceCapacity;
		listCapacity = original.listCapacity;
		listSize = original.listSize;
		data = new T[listCapacity];
		memcpy(data,original.data,original.listSize*sizeof(T));
	}

	void reserve(unsigned int newCapacity) {
		//Allocate the new storage
		T * newData = new T[newCapacity];
		//Copy over the data
		memcpy(newData,data,sizeof(T)*((listSize > newCapacity) ? newCapacity : listSize));
		//Clean up the old storage
		delete [] data;
		//Remember only the new storage
		data = newData;
		listCapacity = newCapacity;
	}

	//Resize data to the new listCapacity
	void resize(unsigned int newSize) {
		//Check you have enough capacity
		if (listCapacity < newSize) 
			reserve(newSize);
		//Increase size
		listSize = newSize;
	}

	const ContiguousList & operator=(const ContiguousList & original) {
		//This could be optimized to take advantage of existing space, but for now it doesnt
		delete[] data;

		autoreduceCapacity = original.autoreduceCapacity;
		listCapacity = original.listCapacity;
		listSize = original.listSize;
		data = new T[listCapacity];
		memcpy(data,original.data,original.listSize*sizeof(T));

		return *this;
	}

	virtual ~ContiguousList() {
		delete [] data;
	}


	//Mutable access
	class iterator : public std::iterator<random_access_iterator_tag,T*>	 {
		//The iterators current position
		T * at;
		//Build a new iterator giving it only the raw start position
		iterator(T * at) {
			this->at = at;
		}
		friend class ContiguousList<T>;
	public:
		T & operator*() const {
			return *at;
		}

		T * operator->() {
			return at;
		}

		//prefix operator
		iterator & operator++() {
			// Move the pointer forwards
			at++;
			return *this;
		}
		//postfix operator (not sure what the argument does?)
		iterator operator++(int) {
			at++;
			return *this;
		}
		//prefix operator
		iterator & operator--() {
			// Move the pointer forwards
			at--;
			return *this;
		}
		//postfix operator (not sure what the argument does?)
		iterator operator--(int) {
			at--;
			return *this;
		}

		T *& operator[](const int index) const {
			return at[index];
		}

		//Additional STL operators needed for std::sort to work
		//may be different with different versions of STL unfortuantely
		int operator-(iterator & other) {
			return at - other.at;
		}
		iterator operator+(const int & other) {
			return iterator(at+other);
		}
		iterator operator-(const int & other) {
			return iterator(at - other);
		}
		bool operator<(const iterator & other) const{
			return at < other.at;
		}
		bool operator>(const iterator & other) const {
			return at > other.at;
		}

		bool operator<=(const iterator & other) const {
			return at <= other.at;
		}
		bool operator>=(const iterator & other) const {
			return at >= other.at;
		}
		
		bool operator==(const iterator & other) const {
			return at == other.at;
		}
		bool operator!=(const iterator & other) const {
			return at != other.at;
		}
	};

	iterator begin() {
		return iterator(data);
	}
	iterator end() {
		return iterator(data+listSize);
	}

	T & operator[](const int index) {
		return data[index];
	}


	//Const access
	class const_iterator {
		//The iterators current position
		const T * at;
		//Build a new iterator giving it only the raw start position
		const_iterator(const T * at) {
			this->at = at;
		}
		friend class ContiguousList<T>;
	public:
		const T & operator*() const {
			return *at;
		}

		const T * operator->() const {
			return *at;
		}

		//prefix operator
		const_iterator & operator++() {
			// Move the pointer forwards
			at++;
			return *this;
		}
		//postfix operator (not sure what the argument does?)
		const_iterator operator++(int) {
			iterator copy(*this);
			at++;
			return *this;
		}
		//prefix operator
		const_iterator & operator--() {
			// Move the pointer forwards
			at--;
			return *this;
		}
		//postfix operator (not sure what the argument does?)
		const_iterator operator--(int) {
			iterator copy(*this);
			at--;
			return *this;
		}

		const T *& operator[](const int index) const {
			return at[index];
		}

		//Additional STL operators needed for std::sort to work
		//may be different with different versions of STL unfortuantely
		int operator-(iterator & other) {
			return at - other.at;
		}
		const_iterator operator+(const int & other) {
			return iterator(at+other);
		}
		const_iterator operator-(const int & other) {
			return iterator(at - other);
		}
		bool operator<(const const_iterator & other) const {
			return at < other.at;
		}
		bool operator>(const const_iterator & other) const {
			return at > other.at;
		}

		bool operator<=(const const_iterator & other) const {
			return at <= other.at;
		}
		bool operator>=(const const_iterator & other) const {
			return at >= other.at;
		}
		
		bool operator==(const const_iterator & other) const {
			return at == other.at;
		}
		bool operator!=(const const_iterator & other) const {
			return at != other.at;
		}
	};

	const_iterator begin() const {
		return const_iterator(data);
	}
	const_iterator end() const {
		return const_iterator(data+listSize);
	}

	const T & operator[](const int index) const {
		return data[index];
	}


	
	void sort(function<bool(T a,T b)> aLessThanB) {
		//Perform an insertion sort
		//favored for its speed with already sorted data
		for (unsigned int a = 1; a < listSize; a++) {
			T temp = data[a];
			int b = a;
			for (; b > 0; b--) {
				if (aLessThanB(temp,data[b-1]))
					data[b] = data[b-1];
				else
					break;
			}
			data[b] = temp;
		}
	}


	//Disable/enable auto resizing when capacity is much greater than size
	void autoredeuce(bool reduce) {
		autoreduceCapacity = reduce;
	}
	
	unsigned int size() const {
		return listSize;
	}
	unsigned int capacity() {
		return listCapacity;
	}
	void clear() {
		listSize = 0;
	}

	//Now the complicated methods
	void push_back(T toInsert) {
		if (listSize == listCapacity)
			//Not enough room, make some more
			reserve((int)(listCapacity * 1.5 + 4));
		//Assume the space after listSize is empty
		//use "placement new" to avoid the assignment operator
		new (&data[listSize++]) T(toInsert);
	}
	//Erase somthing based off an iterator
	//returns a new valid iterator
	iterator erase(iterator toErase) {
		_ASSERTE(toErase != end());
#ifndef __linux__
		_ASSERTE(listSize > 0);
#endif

		//When you remove something from the list, move something to fill its place if there is anything left
		*toErase = data[listSize-1];
		listSize--;

		//Automatically resize the internal array if it's too large
		//for the amount of data we have
		if (autoreduceCapacity && (listSize*2.0+10 < listCapacity)) {
			//at this point looks like we could shrink some
			//Determine the position of the old iterator
			int pos = (int)(toErase.at-data);
			reserve((int)(listSize*1.25)+5);
			//Build a new iterator at the same position but with the new data
			return iterator(data+pos);
		}

		//The old iterator is still valid actually, haha....
		return toErase;
	}

	T & back() {
		return data[listSize-1];
	}

	T & front() {
		return data[0];
	}


	//Finds the first matching entry and removes it
	//if there are no matching entries, has no effect
	void erase(T toRemove) {
		for (unsigned int i = 0; i < listSize; i++) {
			if (data[i] == toRemove) {
				erase(iterator(data+i));
				return;
			}
		}
	}
};