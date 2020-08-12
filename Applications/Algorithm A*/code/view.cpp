//
//  view.cpp
//  algorithm
//
//  Created by Илья Башаров on 24/10/2019.
//  Copyright © 2019 MIPT. All rights reserved.
//

#include "view.hpp"

ImprovedView::ImprovedView(const int &width, const int &height):
viewWasChanged(false)
{
    view.reset(sf::FloatRect(0, 0, width, height));
}

void ImprovedView::changeView(const sf::Keyboard::Key &code)
{
    switch (code)
    {
        case sf::Keyboard::M:
        {
            viewWasChanged = true;
            view.zoom(1.1f);
            break;
        }
            
        case sf::Keyboard::P:
        {
            viewWasChanged = true;
            view.zoom(0.9f);
            break;
        }
            
        case sf::Keyboard::A:
        {
            viewWasChanged = true;
            view.move(-0.1, 0);
            break;
        }
            
        case sf::Keyboard::W:
        {
            viewWasChanged = true;
            view.move(0, -0.1);
            break;
        }
            
        case sf::Keyboard::D:
        {
            viewWasChanged = true;
            view.move(0.1, 0);
            break;
        }
            
        case sf::Keyboard::S:
        {
            viewWasChanged = true;
            view.move(0, 0.1);
            break;
        }
            
        default:            
            break;
    }
}
