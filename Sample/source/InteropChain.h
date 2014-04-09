
#pragma once

#include "stdafx.h"

template <class T>
class InteropChainType {
public:
	virtual ~InteropChainType() {}
	virtual T ValueAtSequence(float sequence) = 0;
	//for nonlinear
	virtual void AddValue(float input, T output, T inTan, T outTan) = 0;
	//For linear
	virtual void AddValue(float input, T output) = 0;
};

//For handiness
typedef InteropChainType<float> InteropChainType1;
typedef InteropChainType<vec2> InteropChainType2;
typedef InteropChainType<vec3> InteropChainType3;
typedef InteropChainType<quat> InteropChainType4;

//Use the magic of templates for one interpolation chain to handle all cases
template <class interopType,class chainType>
class InteropChain : public InteropChainType<chainType> {
	//a function which finds the zero
	template<class T>
	T findZero() {
		return T();
	}
	
	// The recommended method of "template specialization" for clang and gcc is to overload the template function
	float findZero() {
		return 0.0f;
	}

	struct step {
		step(interopType calc) : calculator(calc) {}
		float inputMin;
		float inputMax;
		interopType calculator;
	};
	vector<step> steps;
	bool foundFirstValue;

	float input;
	chainType output;
	chainType outputMin;
	chainType outputMax;
	float inputMin;
	float inputMax;
	chainType inTan;
	chainType outTan;

public:
	InteropChain()  {
		foundFirstValue = false;
	}
	~InteropChain() {

	}
	chainType ValueAtSequence(float sequence) override {
		//Determine which interpolation step the sequence number is in is in
		if (steps.size() <= 0) {
			//Everything must have at least one value
			_ASSERTE(foundFirstValue);
			return outputMin;
		}
		//Handle bounds
		if (sequence <= inputMin)
			return outputMin;
		else if (sequence >= inputMax)
			return outputMax;

		for (int i = 0;;i++) {
			//Search for the input chain containing this sequence
			if ((sequence < steps[i].inputMax) && (sequence >= steps[i].inputMin)) 
				return steps[i].calculator.CalculateAt(sequence);
			
		}
	}
	void ClearValues() {
		foundFirstValue = false;
		steps.clear();
	}
	void AddValue(float input, chainType output) override {
		AddValue(input,output,findZero<chainType>(),findZero<chainType>());
	}
	void AddValue(float input, chainType output, chainType inTan, chainType outTan) override {
		if (!foundFirstValue) {
			outputMin = output;
			inputMin = input;
			foundFirstValue = true;
		}
		else {
			step s(interopType(this->input,input,this->output,output,inTan,this->outTan));
			s.inputMin = this->input;
			s.inputMax = input;
			steps.push_back(s);
		}

		this->input = input;
		this->output = output;
		outputMax = output;
		inputMax = input;
		this->inTan = inTan;
		this->outTan = outTan;
	}
};



