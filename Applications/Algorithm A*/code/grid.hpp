//
//  grid.h
//  algorithm
//
//  Created by Илья Башаров on 01.01.2019.
//  Copyright © 2019 MIPT. All rights reserved.
//

#ifndef grid_h
#define grid_h

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <cmath>

#include "additional.hpp"

#define NUM_OBJECTS 8

enum OBJECTS
{
    UNDEFINED = -1,
    START,
    FINISH,
    EMPTY,
    OBSTACLE,
    UP,
    RIGHT,
    LEFT,
    DOWN
};

class ChosenObject
{
public:
    sf::Sprite object;
    bool alive;
    
    ChosenObject():
    alive(false) {}
    
    ChosenObject(const sf::Texture &texture, const sf::Vector2f &coordinate):
    alive(false)
    {
        object.setTexture(texture);
        object.setPosition(coordinate);
    }
    
    sf::Vector2i getPositionAtMap() const
    {
        auto position = object.getPosition();
        
        return {int(position.x / 32), int(position.y / 32)};
    }
};

class SquareGrid
{
public:
    sf::Texture textures[NUM_OBJECTS];
    ChosenObject points[2];
    
    sf::Sprite mapBasis;
    
    int width, height;
    char *map;
    
    bool mapWasChanged;
    
    SquareGrid(const std::string &);
    ~SquareGrid();
    
    SquareGrid(const SquareGrid&) = delete;
    SquareGrid(SquareGrid&&) = delete;
    SquareGrid& operator=(const SquareGrid&) = delete;
    SquareGrid& operator=(SquareGrid&&) = delete;

    int getWidth() const
    {
        return width;
    }
            
    int getHeight() const
    {
        return height;
    }
    
    char getElementFromMap(const sf::Vector2i &coordinate) const
    {
        return map[coordinate.x + width*coordinate.y];
    }
            
    void updateMap(const sf::Vector2i &coordinate, const char value)
    {
        mapWasChanged = true;
        map[coordinate.x + width*coordinate.y] = value;
    }
    
    char getAliveOfPoints() const
    {
        for (char type : {START, FINISH})
            if (points[type].alive)
                return type;
        
        return UNDEFINED;
    }
    
    void setAliveOfPoints(const char type, const bool alive)
    {
        if (type != UNDEFINED)
            points[type].alive = alive;
        else
            for (char i : {START, FINISH})
                points[i].alive = alive;
    }
    
    void setPositionOfPoint(const char type, const sf::Vector2f &coordinate)
    {
        points[type].object.setPosition(coordinate);
    }
    
    sf::Vector2f getPositionOfPoint(const char type) const
    {
        return points[type].object.getPosition();
    }
    
    sf::Vector2i getPositionOfPointAtMap(const char type) const
    {
        return points[type].getPositionAtMap();
    }
    
    bool inBounds(const sf::Vector2i &vector)
    {
        return (vector.x >= 0) && (vector.x < width) && (vector.y >= 0) && (vector.y < height);
    }
    
    void updateMatrixByAlgorithm();
    float hFunction(const sf::Vector2i &, const sf::Vector2i &);
    std::map<sf::Vector2i, sf::Vector2i, VectorCompare> aStar();
    std::set<ImprovedVector, ImprovedVectorCompare>::iterator minF(std::set<ImprovedVector, ImprovedVectorCompare> &open_set);
    std::vector<ImprovedVector> passableNeighbours(const sf::Vector2i &);
    void setObject(const sf::Vector2f &, const char, const bool);
    void draw(sf::RenderWindow &);
    
};

#endif /* grid_h */
