#ifndef algorithm_hpp
#define algorithm_hpp

#include <SFML/Graphics.hpp>
#include <iostream>

class ImprovedVector
{
public:
    sf::Vector3f function;
    
    ///////////////
    // function: //
    // x = g     //
    // y = h     //
    // z = f     //
    ///////////////
    
    sf::Vector2i position;
    
    ImprovedVector(const sf::Vector3f &, const sf::Vector2i &);
};

class ImprovedVectorCompare
{
public:
    bool operator () (const ImprovedVector& lhs, const ImprovedVector& rhs) const
    {
        if (lhs.position.x == rhs.position.x)
            return lhs.position.y < rhs.position.y;
        else
            return lhs.position.x < rhs.position.x;
    }
};

class VectorCompare
{
public:
    bool operator () (const sf::Vector2i& lhs, const sf::Vector2i& rhs) const
    {
        if (lhs.x == rhs.x)
            return lhs.y < rhs.y;
        else return lhs.x < rhs.x;
    }
};
        
#endif /* algorithm_hpp */
