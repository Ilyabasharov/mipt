//
//  grid.c
//  algorithm
//
//  Created by Илья Башаров on 01.01.2019.
//  Copyright © 2019 MIPT. All rights reserved.
//

#include "grid.hpp"

SquareGrid::SquareGrid(const std::string &objectsFileName):
mapWasChanged(true)
{
    std::cout << "Print field size: " << std::endl;
    std::cin >> width >> height;
    
    map = new char [width*height];
    
    map[0] = START;
    
    for (std::size_t i = 1; i < width*height - 1; i++)
        map[i] = EMPTY;
    
    map[width*height - 1] = FINISH;
    
    sf::Image objectImages;
    
    if (!objectImages.loadFromFile(objectsFileName))
    {
        std::cerr << "Cannot open file: " << objectsFileName << std::endl;
        std::exit(2);
    }
    
    for (char type = START; type <= DOWN; type++)
        textures[type].loadFromImage(objectImages, sf::IntRect(type * 32, 0, 32, 32));
    
    for (char type: {START, FINISH})
    {
        points[type].object.setTexture(textures[type], true);
        points[type].object.setPosition(type * (width - 1) * 32,
                                        type * (height - 1) * 32);
    }
}

SquareGrid::~SquareGrid()
{
    delete [] map;
    
    map = nullptr;
}

std::vector<ImprovedVector> SquareGrid::passableNeighbours(const sf::Vector2i &coordinate)
{
    std::vector<ImprovedVector> neighbours;
    ImprovedVector temp({0, 0, 0}, coordinate);
    
    for (int i: {-1, 1})
    {
        temp.position = {coordinate.x + i, coordinate.y};
        
        if (inBounds(temp.position) && getElementFromMap(temp.position) != OBSTACLE)
            neighbours.push_back(temp);
        
        temp.position = {coordinate.x, coordinate.y + i};
        
        if (inBounds(temp.position) && getElementFromMap(temp.position) != OBSTACLE)
            neighbours.push_back(temp);
    }
    
    return neighbours;
}

float SquareGrid::hFunction(const sf::Vector2i &start, const sf::Vector2i &finish)
{
    auto difference = start - finish;
    
    return std::abs(difference.x) + std::abs(difference.y);
}

std::set<ImprovedVector, ImprovedVectorCompare>::iterator SquareGrid::minF(std::set<ImprovedVector, ImprovedVectorCompare> &open_set)
{
    auto minIterator = open_set.begin();
    
    float min = minIterator -> function.z;
    
    for (auto currentIterator = ++open_set.begin();
         currentIterator != open_set.end();
         currentIterator++)
        
        if (currentIterator -> function.z < min)
        {
            min = currentIterator -> function.z;
            minIterator = currentIterator;
        }
    
    return minIterator;
}

std::map<sf::Vector2i, sf::Vector2i, VectorCompare> SquareGrid::aStar()
{
    std::set<ImprovedVector, ImprovedVectorCompare> closedSet, openSet;
    std::map<sf::Vector2i, sf::Vector2i, VectorCompare> from;
    
    auto startPoint = points[START].getPositionAtMap(),
         finishPoint = points[FINISH].getPositionAtMap();
    
    float currentH = hFunction(startPoint, finishPoint);
    openSet.insert(ImprovedVector({0, currentH, currentH}, startPoint));
    
    while (!openSet.empty())
    {
        auto openSetMinF = minF(openSet);
        
        if (openSetMinF -> position == finishPoint)
            return from;
        
        auto currentMinF = *openSetMinF;
        
        openSet.erase(openSetMinF);
        closedSet.insert(currentMinF);
        
        for (auto neighbour: passableNeighbours(currentMinF.position))
            
            if (closedSet.find(neighbour) == closedSet.end())
            {
                float tentativeG = currentMinF.function.x + 1;
                auto iterator = openSet.find(neighbour);
                
                if (iterator == openSet.end() || tentativeG < neighbour.function.x)
                {
                    neighbour.function.x = tentativeG;
                    neighbour.function.y = hFunction(neighbour.position, finishPoint);
                    neighbour.function.z = neighbour.function.x + neighbour.function.y;

                    from[neighbour.position] = currentMinF.position;
                }
                
                if (iterator == openSet.end())
                    openSet.insert(neighbour);
            }
    }
    
    return from;
}

void SquareGrid::updateMatrixByAlgorithm()
{
    for (std::size_t i = 0; i < width*height - 1; i++)
        if (map[i] > 3)
            map[i] = EMPTY;
    
    auto path = aStar();
    
    auto currentPoint = path[points[FINISH].getPositionAtMap()],
         startPoint = points[START].getPositionAtMap();
    
    sf::Vector2i delta;
    
    while (currentPoint != startPoint)
    {
        delta = path[currentPoint] - currentPoint;
        
        if (delta.x > 0 && delta.y == 0)
            updateMap(currentPoint, LEFT);
        
        if (delta.x == 0 && delta.y < 0)
            updateMap(currentPoint, DOWN);
        
        if (delta.x == 0 && delta.y > 0)
            updateMap(currentPoint, UP);
        
        if (delta.x < 0 && delta.y == 0)
            updateMap(currentPoint, RIGHT);
        
        currentPoint = path[currentPoint];
    }
}

void SquareGrid::setObject(const sf::Vector2f &coordinate,
                           const char type, const bool alive)
{
    points[type].object.setPosition(coordinate);
    points[type].alive = alive;
}

void SquareGrid::draw(sf::RenderWindow &window)
{
    char type = getAliveOfPoints();
    
    if (mapWasChanged)
    {
        if (type == UNDEFINED)
            updateMatrixByAlgorithm();
        
        mapWasChanged = false;
    }
    
    for (std::size_t i = 0; i < width*height; i++)
    {
         mapBasis.setTexture(textures[map[i]], true);
         mapBasis.setPosition((i % width) * 32, (i / width) * 32);
         window.draw(mapBasis);
    }
    
    if (type != UNDEFINED)
        window.draw(points[type].object);
}
