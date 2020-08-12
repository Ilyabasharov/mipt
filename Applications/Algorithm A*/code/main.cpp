#include "application.hpp"
#include "additional.hpp"

int main()
{
    Application application("resources/map.png", "resources/icon.png");
    
    while (application.window.isOpen())
    {
        sf::Vector2f pixelPos = application.window.mapPixelToCoords(sf::Mouse::getPosition(application.window),
                                                application.view.getView());
        
        sf::Vector2i coursorAtMap(int(pixelPos.x / 32), int(pixelPos.y / 32));
        
        sf::Event event;
        
        while (application.window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::MouseButtonPressed:
                {
                    if (coursorAtMap.x < application.grid.getWidth() &&
                        coursorAtMap.y < application.grid.getHeight() &&
                        coursorAtMap.x >= 0 &&
                        coursorAtMap.y >= 0)
                        
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            char chosedPoint = application.grid.getElementFromMap(coursorAtMap);
                            
                            if (chosedPoint == OBSTACLE)
                                application.grid.updateMap(coursorAtMap, EMPTY);
                            
                            else if (chosedPoint == EMPTY || chosedPoint > 3)
                                application.grid.updateMap(coursorAtMap, OBSTACLE);
                            
                            else if (chosedPoint == START || chosedPoint == FINISH)
                            {
                                application.grid.setObject(pixelPos, chosedPoint, true);
                                application.grid.updateMap(coursorAtMap, EMPTY);
                            }
                        }

                    break;
                }
                    
                case sf::Event::MouseButtonReleased:
                {
                    if (coursorAtMap.x < application.grid.getWidth() &&
                        coursorAtMap.y < application.grid.getHeight() &&
                        coursorAtMap.x >= 0 &&
                        coursorAtMap.y >= 0)
                    {
                        char chosedPoint = application.grid.getAliveOfPoints();
                        
                        if (chosedPoint == START || chosedPoint == FINISH)
                        {
                            application.grid.updateMap(coursorAtMap, chosedPoint);
                            application.grid.setAliveOfPoints(chosedPoint, false);
                        }
                    }
                    
                    break;
                }
                    
                case sf::Event::Closed:
                {
                    application.window.close();
                    break;
                }
                    
                case sf::Event::KeyPressed:
                {
                    application.view.changeView(event.key.code);
                    break;
                }
                    
                default:
                    break;
            }
        }
        
        char chosedPoint = application.grid.getAliveOfPoints();
        
        if (chosedPoint != UNDEFINED)
            application.grid.setPositionOfPoint(chosedPoint, pixelPos);
        
        application.update();
    }
    
    return EXIT_SUCCESS;
}
