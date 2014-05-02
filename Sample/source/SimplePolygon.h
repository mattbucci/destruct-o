/*
 *  Copyright 2014 Nathaniel Lewis
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef __POLYGON_H__
#define __POLYGON_H__

#include "stdafx.h"

/**
 * Arbitrary convex Polygon intersection testing algorithm called the Separation of Axis method.
 * Based on the solution found here: http://www.codeproject.com/Articles/15573/D-SimplePolygon-Collision-Detection
 */

template <int N>
class SimplePolygon
{
    // Return the signed distance between two projections
    static float IntervalDistance(float minA, float maxA, float minB, float maxB)
    {
        if (minA < minB)
        {
            return minB - maxA;
        }
        else
        {
            return minA - maxB;
        }
    }

public:
    // Storage type
    typedef std::array<vec2, N> storage_type;
    
    // Test data
    storage_type vertices;
    storage_type edges;
    
    // Create an empty SimplePolygon
    SimplePolygon()
    {
        
    }
    
    // Compute the edges
    void compute_edges()
    {
        // Loop through all the vertices
        size_t eIdx = 0;
        for(typename storage_type::iterator vertex = vertices.begin(); vertex != vertices.end(); vertex++, eIdx++)
        {
            // Calculate the next vertex
            typename storage_type::iterator nVertex = vertex + 1;
            if(nVertex == vertices.end()) nVertex = vertices.begin();
            
            // Compute and store the edge vector
            edges[eIdx] = *nVertex - *vertex;
        }
    }
    
    // Project the SimplePolygon
    void Project(vec2 axis, float& min, float& max)
    {
        // To project a point on an axis use the dot product
        float d = glm::dot(axis, vertices.front());
        min = d;
        max = d;
        for (typename storage_type::iterator i = vertices.begin(); i != vertices.end(); i++)
        {
            d = glm::dot(*i, axis);
            if (d < min)
            {
                min = d;
            }
            else if (d > max)
            {
                max = d;
            }
        }
    }
    
    // Compute the axis aligned bounding rectange
    void BoundingRect(SimplePolygon<4>& b)
    {
        // Compute the minimum x and y, and the maximum x and y
        vec2 minimum = vec2(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
        vec2 maximum = vec2(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
        
        // Loop through all the points
        for(typename storage_type::iterator vertex = vertices.begin(); vertex != vertices.end(); vertex++)
        {
            minimum.x = (vertex->x < minimum.x) ? vertex->x : minimum.x;
            minimum.y = (vertex->y < minimum.y) ? vertex->y : minimum.y;
            maximum.x = (vertex->x > maximum.x) ? vertex->x : maximum.x;
            maximum.y = (vertex->y > maximum.y) ? vertex->y : maximum.y;
        }
        
        // Build the polygon
        b.vertices[0] = minimum;
        b.vertices[1] = vec2(maximum.x, minimum.y);
        b.vertices[2] = maximum;
        b.vertices[3] = vec2(minimum.x, maximum.y);
    }
    
    // Check whether this SimplePolygon has an arbitrary intersection with another
    template <int M>
    bool Intersects(SimplePolygon<M>& b)
    {
        // Minimum and maximum storage
        float minA = 0.0f;
        float minB = 0.0f;
        float maxA = 0.0f;
        float maxB = 0.0f;
        
        // Check the first SimplePolygon's edges for intersection
        for(typename storage_type::iterator edge = edges.begin(); edge != edges.end(); edge++)
        {
            // Calculate the perpendicular axis
            vec2 axis = glm::normalize(vec2(-edge->y, edge->x));
            
            // Find the projection of the SimplePolygon on the current axis
            Project(axis, minA, maxA);
            b.Project(axis, minB, maxB);
            
            // Check if the SimplePolygon projections are currentlty intersecting
            if (IntervalDistance(minA, maxA, minB, maxB) > 0)
            {
                return false;
            }
        }
        
        // Check the second SimplePolygon's edges for intersection
        for(typename SimplePolygon<M>::storage_type::iterator edge = b.edges.begin(); edge != b.edges.end(); edge++)
        {
            // Calculate the perpendicular axis
            vec2 axis = glm::normalize(vec2(-edge->y, edge->x));
            
            // Find the projection of the SimplePolygon on the current axis
            Project(axis, minA, maxA);
            b.Project(axis, minB, maxB);
            
            // Check if the SimplePolygon projections are currentlty intersecting
            if (IntervalDistance(minA, maxA, minB, maxB) > 0)
            {
                return false;
            }
        }
        
        // We intersect
        return true;
    }
};

#endif
