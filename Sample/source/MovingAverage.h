
#pragma once

template <class T>
inline T determineZero() {
	return value();
}
//For primitives
template <>
inline float determineZero<float>() {
	return 0.0f;
}
template <>
inline double determineZero<double>() {
	return 0.0;
}
template <>
inline int determineZero<int>() {
	return 0.0;
}

template <class T>
class MovingAverage {
	vector<T> samples;
	bool arrayFilled;
	int curSample;
	T lastAverage;
	
public:
	MovingAverage(int size) : samples(size) {
		curSample = 0;
		arrayFilled = false;
	}

	void AddSample(T sample) {
		samples[curSample] = sample;
		curSample = (curSample + 1) % (int)samples.size();
		//Check if you looped over
		if (curSample == 0)
			arrayFilled = true;

		//Find the current average
		if (arrayFilled) {
			T total = determineZero<T>();
			for (auto sample : samples)
				total += sample;
			total /= samples.size();
			lastAverage = total;
		}
	}

	T GetAverage() {
		return lastAverage;
	}
};