//
//  QuadTree.hpp
//  qttest
//
//  Created by Nathaniel Lewis on 12/16/13.
//  Copyright (c) 2013 Hoodoonet. All rights reserved.
//

#ifndef qttest_QuadTree_hpp
#define qttest_QuadTree_hpp

#include <iostream>
#include <cstdint>
#include <cmath>
#include <memory>

//#include <boost/pool/pool_alloc.hpp>

// A templated definition of a quadtree implemented based on information
// found in this paper
//
// http://www.merl.com/papers/docs/TR2002-41.pdf
//
template <class T>
class QuadTree
{
public:
    // The leaf class
    struct cell
    {
        // children of this cell
        std::shared_ptr<cell>  children[4];
        
        // The parent cell
        cell                  *parent;
        
        // The x location code of the quadtree
        uint64_t               x;
        uint64_t               y;
        
        // The current level
        uint16_t               level;
        
        // The data this leaf carries
        std::shared_ptr<T>     data;
        
        // Default constructor
        cell()
        {
            // Initialize everything
            x      = 0;
            y      = 0;
            level  = 0;
            data   = NULL;
            parent = NULL;
            
            // Nullify children
            for(int i = 0; i < 4; i++)
            {
                children[i] = NULL;
            }
        }
    };
    
private:
    // The root leaf of this quadtree
    std::shared_ptr<cell> root;

    // The depth of the quadtree
    double                pointMultiplier;
    
    // Traverse to a level
    inline cell* traverseToLevel(cell *quadrant, uint64_t xP, uint64_t yP, uint16_t level)
    {
        // Calculate the difference in the required levels
        uint16_t n = quadrant->level - level;
        
        // While we should decend
        while(n--)
        {
            // Calculate the next level
            uint16_t nextLevel = quadrant->level - 1;
            
            // Calculate the branching bit
            uint64_t childBranchBit = ((uint64_t) 1) << nextLevel;
            
            // Calculate the child index
            uint64_t childIndex = ((xP & childBranchBit) >> nextLevel) + ((yP & childBranchBit) >> (nextLevel - 1));
            
            // Get the child
            quadrant = quadrant->children[childIndex].get();
            
            // If there are no children to this quadrant, break
            if(quadrant->children[0].get() == NULL)
            {
                break;
            }
        }
        
        // Return the quadrant
        return quadrant;
    }
    
public:
    // Create a new Quadtree object
    QuadTree(uint16_t maximumDepth)
    {
        // Create the root leaf of the quadtree
        root = std::make_shared<cell>();
        
        // Set the root depth to maximumDepth - 1
        root->level = maximumDepth - 1;
        
        // Calculate the point multipler
        pointMultiplier = std::pow(2.0, (double) root->level);
    }
    
    // Subdivide a particular cell
    bool subdivide(cell *quadrant)
    {
        // Error upon cell being a level zero
        if(quadrant->level == 0) return false;
        
        // Allocate all child cells
        for(int i = 0; i < 4; i++)
        {
            // Allocate a new cell
            quadrant->children[i] = std::make_shared<cell>();
            
            // Set the new level
            quadrant->children[i]->parent = quadrant;
            quadrant->children[i]->level = quadrant->level - 1;
            
            // Setup the location of this cell
            quadrant->children[i]->x = quadrant->x + ((i % 2) * (((uint64_t) 1) << (quadrant->level - 1)));
            quadrant->children[i]->y = quadrant->y + ((i / 2) * (((uint64_t) 1) << (quadrant->level - 1)));
            
            // No data yet
            quadrant->children[i]->data = NULL;
        }
        
        // Success
        return true;
    }
    
    // Generate the coordinates of a cell containing a particular point at a particular level
    void coordinates(double x, double y, uint64_t& xP, uint64_t& yP, uint16_t level)
    {
        // Convert the floating point locations into level 0 coordinates
        xP = (uint64_t) (x * pointMultiplier);
        yP = (uint64_t) (y * pointMultiplier);
        
        // Shift the values over to remove precision from precision we don't want
        xP = xP >> level;
        yP = yP >> level;
        
        // Add zeros back in the old spot
        xP = xP << level;
        yP = yP << level;
    }
    
    // Locate a particular cell from the tree
    cell* locate(double x, double y)
    {
        // Calculate the location codes for these points
        uint64_t xP = (uint64_t) (x * pointMultiplier);
        uint64_t yP = (uint64_t) (y * pointMultiplier);
        
        // Cell we are currently exploring
        cell *quadrant = root.get();
        
        // Perform traversal
        while(quadrant->children[0] != NULL)
        {
            // Get the next level
            uint16_t nextLevel = quadrant->level - 1;
            
            // Calculate the bit for our current level
            uint64_t childBranchBit = ((uint64_t) 1) << nextLevel;
            
            // Calculate the child index
            uint8_t childIndex = ((xP & childBranchBit) >> nextLevel) + ((yP & childBranchBit) >> (nextLevel - 1));
            
            // Lookup the new cell
            quadrant = quadrant->children[childIndex].get();
        }
        
        // Traverse the tree
        return quadrant;
    }
    
    // Locate a neighbor of the cell
    cell *neighbor(cell *quadrant, int8_t u, int8_t v)
    {
        // Calculate the binary cell size
        uint64_t binaryCellSize = ((uint64_t) 1) << quadrant->level;
        
        // Get the location code for the cell left of us
        uint64_t xD = (u > 0) ? quadrant->x + binaryCellSize : ((u < 0) ? quadrant->x - 1 : quadrant->x);
        uint64_t yD = (v > 0) ? quadrant->y + binaryCellSize : ((v < 0) ? quadrant->y - 1 : quadrant->y);
        
        // Calculate the binary difference
        uint64_t diff = (quadrant->x ^ xD) | (quadrant->y ^ yD);
        
        // pointer to hold common ancestor
        cell *ancestor = quadrant;
        
        // Find the ancestor
        while (diff & (((uint64_t) 1) << ancestor->level))
        {
            //cout << "Exploring level - " << ancestor->level << endl;
            if(ancestor->parent == NULL)
            {
                break;
            }
            ancestor = ancestor->parent;
        }
        
        // Tranverse to the new cell
        return traverseToLevel(ancestor, xD, yD, quadrant->level);
    }
    
    // Get the root cell
    cell* getRoot()
    {
        return root.get();
    }
};


#endif
