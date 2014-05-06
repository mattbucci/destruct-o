
#pragma once

#include "stdafx.h"

typedef glm::detail::tvec2<int> vec2i;
typedef glm::detail::tvec3<int> vec3i;
typedef glm::detail::tvec4<int> vec4i;

// Comparator objects for vec2
template <typename T>
struct vec2comparator_less
{
    // Compare two vec2's
    bool operator()(const glm::detail::tvec2<T>& a, const glm::detail::tvec2<T>& b) const
    {
        // If the x coordinates are not equal, sort based on them
        if(a.x != b.x)
        {
            return a.x < b.x;
        }
        
        // Otherwise sort on y
        return a.y < b.y;
    }
};

template <typename T>
struct vec2comparator_greater
{
    // Compare two vec2's
    bool operator()(const glm::detail::tvec2<T>& a, const glm::detail::tvec2<T>& b) const
    {
        // If the x coordinates are not equal, sort based on them
        if(a.x != b.x)
        {
            return a.x > b.x;
        }
        
        // Otherwise sort on y
        return a.y > b.y;
    }
};
