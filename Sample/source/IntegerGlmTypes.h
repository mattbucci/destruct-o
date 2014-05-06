
#pragma once

#include "stdafx.h"

typedef glm::detail::tvec2<int> vec2i;
typedef glm::detail::tvec3<int> vec3i;
typedef glm::detail::tvec4<int> vec4i;

//This type is used for comparison only
//such as for the key in the inside of std::map
template<class T>
class vec2c : public T{
public:
	//Implicit conversion to this type supported
	vec2c() : T() {}
	vec2c(const T & original) : T(original) {}
	//Comparison operators
	bool operator==(const vec2c<T> & other) const {
		return (x == other.x) && (y == other.y);
	}
	bool operator>(const vec2c<T> & other) const {
		if (x != other.x)
			return x > other.x;
		return y > other.y;
	}
	bool operator>=(const vec2c<T> & other) const {
		if (x != other.x)
			return x > other.x;
		return y >= other.y;
	}
	bool operator<(const vec2c<T> & other) const {
		if (x != other.x)
			return x < other.x;
		return y < other.y;
	}
	bool operator<=(const vec2c<T> & other) const {
		if (x != other.x)
			return x < other.x;
		return y <= other.y;
	}
};