
#pragma once

#include "Interop.h"

template <class T>
class LinearInterop : public Interop<T> {
	T m;
	T b;
public:
	LinearInterop(float inputA, float inputB, T outputA, T outputB,T inTan, T outTan) {
		m = (outputA-outputB)/(inputA-inputB);
		b = outputA-m*inputA;
	}

	T CalculateAt(float sequence) {
		return sequence*m+b;
	}
};