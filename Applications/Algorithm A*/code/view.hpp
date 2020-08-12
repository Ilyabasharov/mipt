#ifndef view_hpp
#define view_hpp

#include <SFML/Graphics.hpp>

class ImprovedView
{
private:
    sf::View view;
    
    bool viewWasChanged;
    
public:
    ImprovedView(const int &, const int &);
    
    void changeView(const sf::Keyboard::Key &);
    void viewMap();
    
    sf::View getView() const
    {
        return view;
    }
    
    bool getWasChanged() const
    {
        return viewWasChanged;
    }
    
    void setWasChanged(bool change)
    {
        viewWasChanged = change;
    }
};

#endif /* view_hpp */
