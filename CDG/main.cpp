#include <iostream> 
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <random>
#include <SFML/Audio.hpp>
#include "classes+settings.h"

int main() 
{
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(windowSize_x, windowSize_y)), "CDG");
    window.setVerticalSyncEnabled(true);

    GameState gamestate;

    sf::Time deltaTime;
    sf::Clock deltaTimeClock;

    while(window.isOpen())
    {
        while(auto event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
                window.close();
        }
        gamestate.setMousPos(sf::Mouse::getPosition(window));
        deltaTime = deltaTimeClock.restart();

        gamestate.update(deltaTime);

        window.clear(sf::Color::White);
        gamestate.drawTo(window);
        window.display();
    }
    return 0;
}