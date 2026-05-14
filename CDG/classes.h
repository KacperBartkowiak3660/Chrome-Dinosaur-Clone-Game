#include <iostream> 
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <random>
#include <SFML/Audio.hpp>
#pragma once


struct Fps_s
{
    sf::Font font;
    sf::Text text;
    sf::Clock clock;
    int Frame;
    int fps;
};

class Fps
{
    Fps_s fps;
    public:
        Fps()
            :fps()
        {
            if(fps.font.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/font.ttf"))
            {
                fps.text.setFont(fps.font);
            }
            fps.text.setCharacterSize(15);
            fps.text.setPosition(sf::Vector2f(fps.text.getCharacterSize() + 10.f, fps.text.getCharacterSize()));
            fps.text.setFillColor(sf::Color(83, 83, 83));
        }
        void update()
        {
            if(fps.clock.getElapsedTime().asSeconds() >= 1.f)
            {
                fps.fps = fps.Frame; 
                fps.Frame = 0; 
                fps.clock.restart();
            }
            fps.Frame++;
            fps.text.setString("FPS :- " + std::to_string(fps.fps));
        }    
        void drawTo(sf::RenderWindow& window)
        {
            window.draw(fps.text);
        }
};

class SoundManager
{
    public:
        sf::SoundBuffer dieBuffer;
        sf::SoundBuffer jumpBuffer;
        sf::SoundBuffer pointBuffer;
        sf::Sound dieSound;
        sf::Sound jumpSound;
        sf::Sound pointSound;

        SoundManager()
        :dieBuffer(), jumpBuffer(), pointBuffer(), dieSound(), jumpSound(), pointSound()
        {
            dieBuffer.loadFromFile("assets/die.wav");
            jumpBuffer.loadFromFile("assets/jump.wav");
            pointBuffer.loadFromFile("assets/point.wav");
            dieSound.setBuffer(dieBuffer);
            jumpSound.setBuffer(jumpBuffer);
            pointSound.setBuffer(pointBuffer);
        }
};

class Ground
{
    public:
        sf::Sprite groundSprite;
        sf::Texture groundTexture;
        int offset{0};
        Ground()
        :groundSprite(), groundTexture()
        {
            if(groundTexture.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/ground.png"))
            {
                groundSprite.setTexture(groundTexture);
                groundSprite.setPosition(sf::Vector2f(0.f, windowSize_y - groundTexture.getSize().y - 50)); 
            }
        }

        void updateGround()
        {
            if(playerDead == false)
            {
                if(offset > groundTexture.getSize().x - windowSize_x)
                    offset = 0;

                offset += gameSpeed;
                groundSprite.setTextureRect(sf::IntRect(offset, 0, windowSize_x, windowSize_y));
            }

            if(playerDead == true)
                groundSprite.setTextureRect(sf::IntRect(offset, 0, windowSize_x, windowSize_y));

        }
        void reset()
        {
            offset = 0;
            groundSprite.setTextureRect(sf::IntRect(0, 0, windowSize_x, windowSize_y));
        }
};

class Obstacle
{
    public:
        sf::Sprite obstacleSprite;
        sf::FloatRect obstacleBounds{0.f, 0.f, 0.f, 0.f};
        Obstacle(sf::Texture& texture)
        :obstacleSprite(), obstacleBounds()
        {
            obstacleSprite.setTexture(texture);
            obstacleSprite.setPosition(sf::Vector2f(windowSize_x, groundOffset));
        }
};

class Obstacles
{
    public:
        std::vector<Obstacle> obstacles;

        sf::Time spawnTimer;
        sf::Texture obstacleTexture_1;
        sf::Texture obstacleTexture_2;
        sf::Texture obstacleTexture_3;
        int randomNumber{0};

        Obstacles()
        :spawnTimer(sf::Time::Zero)
        {
            obstacles.reserve(5);

            if(obstacleTexture_1.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/obstacle1.png"))
            {
                std::cout << "Obstacle 1 loaded successfully!" << std::endl;
            }
            if(obstacleTexture_2.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/obstacle2.png"))
            {
                std::cout << "Obstacle 2 loaded successfully!" << std::endl;
            }
            if(obstacleTexture_3.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/obstacle3.png"))
            {
                std::cout << "Obstacle 3 loaded successfully!" << std::endl;
            }
        }

        void update(sf::Time& deltaTime)
        {
            spawnTimer += deltaTime;
            if(spawn.Timer.asSeconds() > 0.5f + gameSpeed/8)
            {
                randomNumber = (rand() % 3) + 1;
                if(randomNumber == 1)
                {
                    obstacles.emplace_back(Obstacle(obstacleTexture_1));
                }
                if(randomNumber == 2)
                {
                    obstacles.emplace_back(Obstacle(obstacleTexture_2));
                }
                if(randomNumber == 3)
                {
                    obstacles.emplace_back(Obstacle(obstacleTexture_3));
                }
                spawnTimer = sf::Time::Zero;
            }

            if(playerDead == false)
            {
                for(int i = 0; i <obstacles.size(); i++)
                {
                    obstacles[i].obstacleBounds = obstacles[i].obstacleSprite.getGlobalBounds(); 
                    obstacles[i].obstacleBounds.width -= 10.f;
                    obstacles[i].obstacleSprite.move(-1*gameSpeed, 0.f);
                    if(obstacles[i].obstacleSprite.getPosition().x < -150.f)
                    {
                        std::vector<Obstacle>::iterator obstacleIter = obstacles.begin() + i;
                        obstacles.erase(obstacleIter);
                    }
                }
            }

            if(playerDead == true)
            {
                for(auto& obstacles : obstacles)
                {
                    obstacles.obstacleSprite.move(0.f, 0.f);
                }
            }
        }

        void drawTo(sf::RenderWindow& window)
        {
            for(auto& obstacles : obstacles)
            {
                window.draw(obstacles.obstacleSprite);
            }
        }

        void reset()
        {
            obstacles.erase(obstacles.begin(), obstacles.end());
        }
};

        class Knight