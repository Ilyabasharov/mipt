//
//  application.h
//  algorithm
//
//  Created by Илья Башаров on 05.08.2020.
//  Copyright © 2020 MIPT. All rights reserved.
//

#ifndef application_h
#define application_h

#include <iostream>
#include <SFML/Graphics.hpp>

#include "view.hpp"
#include "grid.hpp"
#include "additional.hpp"

class Application
{
public:
    
    SquareGrid grid;
    ImprovedView view;
    
    sf::RenderWindow window;
    sf::Image icon;
    
    Application(const std::string &, const std::string &);
    
    void update();
};

#endif /* application_h */
