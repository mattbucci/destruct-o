
#pragma once

template <class T>
class Interop {
public:
	virtual T CalculateAt(float sequence) = 0;
};