//
//  application.cpp
//  algorithm
//
//  Created by Илья Башаров on 05.08.2020.
//  Copyright © 2020 MIPT. All rights reserved.
//

#include "application.hpp"

Application::Application(const std::string &mapName,
                         const std::string &iconName):
grid(mapName), view(grid.getWidth()*32, grid.getHeight()*32),
window(sf::VideoMode(grid.getWidth()*32, grid.getHeight()*32), "A* visualisation")
{
    if (!icon.loadFromFile(iconName))
    {
        std::cerr << "Cannot open file: " << iconName << std::endl;
        std::exit(2);
    }
    
    window.setIcon(icon.getSize().x,
                   icon.getSize().y,
                   icon.getPixelsPtr());
}

void Application::update()
{
    if (view.getWasChanged())
    {
        window.setView(view.getView());
        view.setWasChanged(false);
    }
    
    window.clear(sf::Color(128, 106, 89));
    
    grid.draw(window);
    
    window.display();
}
