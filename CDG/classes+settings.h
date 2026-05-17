#pragma once

#include <iostream> 
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <random>
#include <array>
#include <SFML/Audio.hpp>
#include <optional>

const unsigned int windowSize_x = 1000;
const unsigned int windowSize_y = 500;
const unsigned int groundOffset = windowSize_y - 150.f;
float groundLevel = windowSize_y - 175.f;
int gameSpeed = 8;
bool playerDead = false;
bool playDeadSound = false; 

struct Fps_s
{
    sf::Font font;
    std::optional<sf::Text> text;
    sf::Clock clock;
    int Frame = 0;
    int fps = 0;
};

class Fps
{
    Fps_s fps;
    public:
        Fps()
        {
            if(fps.font.openFromFile("C:/Users/bkacp/Desktop/CDG/Assets/Font.ttf"))
            {
                fps.text.emplace(fps.font);
                fps.text->setCharacterSize(15);
                fps.text->setPosition(sf::Vector2f(fps.text->getCharacterSize() + 10.f, fps.text->getCharacterSize()));
                fps.text->setFillColor(sf::Color(83, 83, 83));
            }
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
            if(fps.text)
                fps.text->setString("FPS :- " + std::to_string(fps.fps));
        }    
        void drawTo(sf::RenderWindow& window)
        {
            if(fps.text)
                window.draw(*fps.text);
        }
};

class SoundManager
{
    public:
        sf::SoundBuffer dieBuffer;
        sf::SoundBuffer jumpBuffer;
        sf::SoundBuffer pointBuffer;
        std::optional<sf::Sound> dieSound;
        std::optional<sf::Sound> jumpSound;
        std::optional<sf::Sound> pointSound;

        SoundManager()
        {
            if(dieBuffer.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/die.wav"))
                dieSound.emplace(dieBuffer);
            if(jumpBuffer.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/jump.wav"))
                jumpSound.emplace(jumpBuffer);
            if(pointBuffer.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/point.wav"))
                pointSound.emplace(pointBuffer);
        }
};

class Ground
{
    public:
        std::optional<sf::Sprite> groundSprite;
        sf::Texture groundTexture;
        int offset{0};
        Ground()
        {
            if(groundTexture.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/GroundImage.png"))
            {
                groundSprite.emplace(groundTexture);
                groundSprite->setPosition(sf::Vector2f(0.f, windowSize_y - groundTexture.getSize().y - 50));
            }
        }

        void updateGround()
        {
            if(!groundSprite) return;
            if(playerDead == false)
            {
                if(offset > groundTexture.getSize().x - windowSize_x)
                    offset = 0;

                offset += gameSpeed;
                groundSprite->setTextureRect(sf::IntRect(sf::Vector2i(offset, 0), sf::Vector2i(windowSize_x, windowSize_y)));
            }

            if(playerDead == true)
                groundSprite->setTextureRect(sf::IntRect(sf::Vector2i(offset, 0), sf::Vector2i(windowSize_x, windowSize_y)));
        }
        void reset()
        {
            offset = 0;
            if(groundSprite)
                groundSprite->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(windowSize_x, windowSize_y)));
        }
};

class Obstacle
{
    public:
        std::optional<sf::Sprite> obstacleSprite;
        sf::FloatRect obstacleBounds{{0.f, 0.f}, {0.f, 0.f}};

        //animation atributes:
        std::vector<sf::IntRect> frames;
        int currentFrame = 0;
        sf::Time frameTimer = sf::Time::Zero;
        sf::Time timePerFrame = sf::seconds(0.12f);

        // hitbox settings:
        float shrinkX = 0.f;
        float shrinkY = 0.f;

        //base constructor:
        Obstacle(sf::Texture& texture, float windowSize_x, float groundOffset)
        {
            obstacleSprite.emplace(texture);
            obstacleSprite->setPosition(sf::Vector2f(windowSize_x, groundOffset));
        }

        //Virtual destructor
        virtual ~Obstacle() = default;

        //helper function
        void setupFrames(int frameWidth, int frameHeight, int totalFrames)
        {
            for(int i = 0; i < totalFrames; i++)
            {
                frames.push_back(sf::IntRect(sf::Vector2i(i * frameWidth, 0), sf::Vector2i(frameWidth, frameHeight)));
            }
            if(obstacleSprite && !frames.empty())
            {
                obstacleSprite->setTextureRect(frames[0]);
            }
        }
};

class CactusSmall : public Obstacle
{
    public:
        CactusSmall(sf::Texture& texture, float windowSize_x, float groundOffset)
        : Obstacle(texture, windowSize_x, groundOffset - 10.f)
        {
            obstacleSprite->setScale(sf::Vector2(2.f, 2.f));

            timePerFrame = sf::seconds(0.18f); // Slower animation
            // Assuming sheet has 4 frames, each 48x48
            setupFrames(150, 52, 4);

            // hitbox setting:
            shrinkX = 230.f;
            shrinkY = 200.f;
        }
};

class CactusLarge : public Obstacle
{
    public:
        CactusLarge(sf::Texture& texture, float windowSize_x, float groundOffset)
        : Obstacle(texture, windowSize_x, groundOffset - 250.f)
        {
            obstacleSprite->setScale(sf::Vector2(4.f, 4.f));

            timePerFrame = sf::seconds(0.10f); // Faster animation
            setupFrames(156, 156, 14);

            // hitbox setting:
            shrinkX = 500.f;
            shrinkY = 600.f;
        }
};

//flying class ex:
class FlyingObstacle : public Obstacle
{
    public:

        FlyingObstacle(sf::Texture& texture, float windowSize_x, float finalYPosition)
        : Obstacle(texture, windowSize_x, finalYPosition) // offset - 120
        {
            obstacleSprite->setScale(sf::Vector2(2.f, 2.f));

            timePerFrame = sf::seconds(0.08f); //Very fast
            setupFrames(87, 87, 11);

            // hitbox setting:
            shrinkX = 80.f;
            shrinkY = 160.f;
        }
};

class Obstacles
{
    public:
        // Changed to a vector of unique_ptrs to safely handle derived classes
        std::vector<std::unique_ptr<Obstacle>> obstacles;

        float distanceTracker{0.f};
        float nextSpawnDistance{0.f};
        
        sf::Texture obstacleTexture_1;
        sf::Texture obstacleTexture_2;
        sf::Texture obstacleTexture_3;
        int randomNumber{0};

        Obstacles() : distanceTracker(0.f)
        {
            nextSpawnDistance = static_cast<float>((rand() % 500) + 400);

            // (Keep your standard texture loadFromFile blocks here...)
            obstacleTexture_1.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/Skeleton.png");
            obstacleTexture_2.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/Slime.png");
            obstacleTexture_3.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/Bat.png");
        }

        void update(sf::Time& deltaTime)
        {
            if (playerDead == false)
            {
                distanceTracker += gameSpeed * deltaTime.asSeconds() * 60.f;

                if (distanceTracker >= nextSpawnDistance)
                {
                    randomNumber = (rand() % 3) + 1;
                    
                    // Instantiate the specific derived object using std::make_unique
                    if(randomNumber == 1) 
                        obstacles.push_back(std::make_unique<CactusSmall>(obstacleTexture_1, windowSize_x, groundOffset));
                    if(randomNumber == 2) 
                        obstacles.push_back(std::make_unique<CactusLarge>(obstacleTexture_2, windowSize_x, groundOffset));
                    if(randomNumber == 3) 
                    {
                        float randomAirOffset = static_cast<float>((rand() % 200) + 120);
                        float randomFlyingHeight = groundOffset - randomAirOffset;

                        obstacles.push_back(std::make_unique<FlyingObstacle>(obstacleTexture_3, windowSize_x, randomFlyingHeight));
                    }
                    distanceTracker = 0.f;
                    nextSpawnDistance = static_cast<float>((rand() % 600) + 400);
                }
            }

            if(playerDead == false)
            {
                for(int i = 0; i < obstacles.size(); i++)
                {
                    // Remember to use -> instead of . since obstacles[i] is now a pointer!
                    if(!obstacles[i]->obstacleSprite) continue;

                    // 1. Process unique object animation frames
                    if (!obstacles[i]->frames.empty())
                    {
                        obstacles[i]->frameTimer += deltaTime;
                        if (obstacles[i]->frameTimer >= obstacles[i]->timePerFrame)
                        {
                            obstacles[i]->currentFrame = (obstacles[i]->currentFrame + 1) % obstacles[i]->frames.size();
                            obstacles[i]->obstacleSprite->setTextureRect(obstacles[i]->frames[obstacles[i]->currentFrame]);
                            obstacles[i]->frameTimer -= obstacles[i]->timePerFrame;
                        }
                    }

                    // 2. Physics & Collisions
                    obstacles[i]->obstacleBounds = obstacles[i]->obstacleSprite->getGlobalBounds();
                    sf::Vector2f boundsSize = obstacles[i]->obstacleBounds.size;
                    obstacles[i]->obstacleBounds.size = {boundsSize.x - 10.f, boundsSize.y};
                    
                    sf::FloatRect globalBounds = obstacles[i]->obstacleSprite->getGlobalBounds();
                    
                    float sX = obstacles[i]->shrinkX;
                    float sY = obstacles[i]->shrinkY;

                    obstacles[i]->obstacleBounds = sf::FloatRect(
                        sf::Vector2f(globalBounds.position.x + (sX / 2.f), globalBounds.position.y + (sY / 2.f)),
                        sf::Vector2f(globalBounds.size.x - sX, globalBounds.size.y - sY)
                    );

                    obstacles[i]->obstacleSprite->move(sf::Vector2f(-1 * gameSpeed, 0.f));
                    
                    if(obstacles[i]->obstacleSprite->getPosition().x < -300.f)
                    {
                        // unique_ptr automatically cleans up the allocated memory when erased
                        obstacles.erase(obstacles.begin() + i);
                        i--; 
                    }
                }
            }

            if(playerDead == true)
            {
                for(auto& obs : obstacles)
                {
                    if(obs->obstacleSprite)
                        obs->obstacleSprite->move(sf::Vector2f(0.f, 0.f));
                }
            }
        }

        void drawTo(sf::RenderWindow& window)
        {
            for(auto& obs : obstacles)
            {
                if(obs->obstacleSprite)
                    window.draw(*obs->obstacleSprite);
            }
        }

        void reset()
        {
            obstacles.clear(); // Safely clears pointers and destroys instances
            distanceTracker = 0.f;
            nextSpawnDistance = static_cast<float>((rand() % 500) + 400);
        }
};

class Knight
{
    public:
        std::optional<sf::Sprite> knight;
        sf::Vector2f knightPos{0.f, 0.f};
        sf::Vector2f knightMotion{0.f, 0.f};
        sf::Texture knightTex;
        sf::FloatRect knightBounds;
        SoundManager soundManager;
        std::array<sf::IntRect, 8> frames;
        sf::Time timeTracker;
        int animationCounter{0};
        bool dieSoundPlayed = false;

        Knight()
        {
            if(knightTex.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/PlayerSpriteSheet.png"))
            {
                
                knight.emplace(knightTex);

                knight->setScale(sf::Vector2f(2, 2));//rozmiar

                for(int i = 0; i < frames.size(); i++){
                    frames[i] = sf::IntRect(sf::Vector2i(i * 96, 0), sf::Vector2i(96, 84));
                }
                knight->setTextureRect(frames[0]);
                knightPos = knight->getPosition();
            }
            else
            {
                std::cout << "Error loading the PlayerSprite texture" << std::endl;
            }
        }

        void update(sf::Time& deltaTime, std::vector<std::unique_ptr<Obstacle>>& obstacles)
        {
            knightPos = knight->getPosition();
            knightBounds = knight->getGlobalBounds();
            sf::Vector2f boundsSize = knightBounds.size;

            float shrinkX = 120.f;
            float shrinkY = 80.f;

            knightBounds = sf::FloatRect(
                sf::Vector2f(knightBounds.position.x + (shrinkX / 2.f), knightBounds.position.y + (shrinkY / 2.f)),
                sf::Vector2f(boundsSize.x - shrinkX, boundsSize.y - shrinkY)
            );  //og value x = 10.f y=15.f
            
            timeTracker += deltaTime;


            for(const auto& obstacles: obstacles)
            {
                if(knightBounds.findIntersection(obstacles->obstacleBounds).has_value())
                {
                    playerDead = true;
                }
            }
            if(!playerDead)
            {
                walk();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) == true && knightPos.y >= groundLevel)
                {
                    animationCounter = 0;
                    knightMotion.y = -20.f; knight->setTextureRect(frames[1]);
                    if(soundManager.jumpSound) soundManager.jumpSound->play();
                }
                if(knightPos.y < groundLevel)
                {
                    knightMotion.y += 1.f; knight->setTextureRect(frames[1]);
                }
                if(knightPos.y > groundLevel)
                {
                    knight->setPosition(sf::Vector2f(knight->getPosition().x, groundLevel));
                    knightMotion.y = 0.f;
                }
                knight->move(knightMotion);
            }
            if(playerDead == true)
            {
                knightMotion.y = 0.f;
                //knight->setTextureRect(frames[3]);
                if(soundManager.dieSound && !dieSoundPlayed)
                {
                    soundManager.dieSound->setLooping(false);
                    soundManager.dieSound->play();
                    dieSoundPlayed = true;
                }
                
            }
        }

        void walk()
        {
            int frameIndex = (animationCounter / 3) % frames.size();
            knight->setTextureRect(frames[frameIndex]);
            animationCounter++;
        }

        void reset()
        {
            knightMotion.y = 0;
            if (knight)
                knight->setPosition(sf::Vector2f(windowSize_x / 2.f - windowSize_x / 4.f, groundLevel));
                //knight->setPosition(sf::Vector2f(windowSize_x / 2.f - windowSize_x / 4.f, 250.f));
            knight->setTextureRect(frames[0]);
            animationCounter = 0;

            dieSoundPlayed = false;
            playerDead = false;
        }
};

class Scores
{
    public:
        std::optional<sf::Text> previousScoreText;
        std::optional<sf::Text> HIText;
        std::optional<sf::Text> scoresText;
        sf::Font scoresFont;
        SoundManager soundManager;
        short scores{0};
        short previousScore{0};
        short scoresIndex{0};
        short scoresDiff{0};
        short scoresInital;

        Scores()
        {
            if(scoresFont.openFromFile("C:/Users/bkacp/Desktop/CDG/Assets/Font.ttf"))
            {
                scoresText.emplace(scoresFont);
                scoresText->setCharacterSize(15);
                scoresText->setPosition(sf::Vector2f(windowSize_x/2 + windowSize_x/4 + 185.f, scoresText->getCharacterSize() + 10.f));
                scoresText->setFillColor(sf::Color(83, 83, 83));

                previousScoreText.emplace(scoresFont);
                previousScoreText->setCharacterSize(15);
                previousScoreText->setPosition(sf::Vector2f(scoresText->getPosition().x - 100.f, scoresText->getPosition().y));
                previousScoreText->setFillColor(sf::Color(83, 83, 83));

                HIText.emplace(scoresFont);
                HIText->setCharacterSize(15);
                HIText->setPosition(sf::Vector2f(previousScoreText->getPosition().x - 50.f, previousScoreText->getPosition().y));
                HIText->setFillColor(sf::Color(83, 83, 83));
                HIText->setString("HI");
            }
            scoresInital = 0;
        }

        void update()
        {
            if(!scoresText) return;
            if(playerDead == false)
            {
                scoresIndex++;
                if(scoresIndex >= 5)
                {
                    scoresIndex = 0;
                    scores++;
                }
                scoresDiff = scores - scoresInital;
                if(scoresDiff > 100)
                {
                    scoresInital += 100;
                    gameSpeed += 1;
                    if(soundManager.pointSound)
                        soundManager.pointSound->play();
                }

                scoresText->setString(std::to_string(scores));
                if(previousScoreText)
                    previousScoreText->setString(std::to_string(previousScore));
            }
        }

        void reset()
        {
            if(scores > previousScore)
                previousScore = scores;

            if(previousScoreText)
                previousScoreText->setString(std::to_string(previousScore));
            if(scoresText)
                scoresText->setString("0");

            scores = 0;
            scoresIndex = 0;
            scoresDiff = 0;
            scoresInital = 0;
        }
};

class RestartButton
{
    public:
        std::optional<sf::Sprite> restartButtonSprite;
        sf::FloatRect restartButtonSpriteBounds;
        sf::Texture restartButtonTexture;
        sf::Vector2f mousePos;
        bool checkPressed{false};

        RestartButton()
        :restartButtonSprite(), restartButtonTexture(), mousePos(0.f, 0.f), restartButtonSpriteBounds()
        {
            if (restartButtonTexture.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/RestartButton.png"))
            {
                restartButtonSprite.emplace(restartButtonTexture);
                restartButtonSprite->setPosition(sf::Vector2f(windowSize_x / 2.f - restartButtonTexture.getSize().x / 2.f,
                                                              windowSize_y / 2.f + 20.f));
                restartButtonSpriteBounds = restartButtonSprite->getGlobalBounds();
            }
        }
};

class Clouds
{
    public:
        std::vector<sf::Sprite> clouds;
        sf::Time currTime;
        sf::Texture cloudTexture;
        std::random_device dev;
        std::mt19937 rng{dev()};

        Clouds()
        {
            if(cloudTexture.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/Clouds.png"))
            {
                std::cout << "Loaded CloudTexture" << std::endl;
                clouds.reserve(4);
                clouds.emplace_back(cloudTexture);
                clouds.back().setPosition(sf::Vector2f(windowSize_x, windowSize_y/2 - 40.f));
            }
        }

        void updateClouds(sf::Time& deltaTime)
        {
            currTime += deltaTime;
            if(currTime.asSeconds() > 8.f && cloudTexture.getSize().x > 0)
            {
                clouds.emplace_back(cloudTexture);
                std::uniform_int_distribution<std::mt19937::result_type> dist6( windowSize_y/2 - 200, windowSize_y/2 - 50);
                clouds.back().setPosition(sf::Vector2f(windowSize_x, dist6(rng)));
                currTime = sf::Time::Zero;
            }

            for(int i = 0; i < clouds.size(); i++)
            {
                if(playerDead == false)
                    clouds[i].move(sf::Vector2f(-1.f, 0.f));
                if(playerDead == true)
                    clouds[i].move(sf::Vector2f(-0.5f, 0.f));

                if(clouds[i].getPosition().x < 0.f - cloudTexture.getSize().x)
                {
                    std::vector<sf::Sprite>::iterator cloudIter = clouds.begin() + i;
                    clouds.erase(cloudIter);
                }
            }
        }

        void drawTo(sf::RenderWindow& window)
        {
            for(auto& clouds: clouds)
            {
                window.draw(clouds);
            }
        }
};

class GameState
{
    public:
        Fps fps;
        Knight knight;
        Ground ground;
        Obstacles obstacles;
        Scores scores;
        Clouds clouds;
        RestartButton restartButton;
        sf::Font gameOverFont;
        std::optional<sf::Text> gameOverText;
        sf::Vector2f mousePos{0.f, 0.f};

        GameState()
        {
            if (!gameOverFont.openFromFile("C:/Users/bkacp/Desktop/CDG/Assets/Font.ttf")) {
                std::cerr << "Failed to load font" << std::endl;
            }
            gameOverText.emplace(gameOverFont);
            gameOverText->setString("Game Over");
            gameOverText->setCharacterSize(40);
            if (knight.knight)
                knight.knight->setPosition(sf::Vector2f(windowSize_x/2.f - windowSize_x/4.f, groundLevel));
            if (gameOverText)
            {
                sf::FloatRect textBounds = gameOverText->getGlobalBounds();
                float textX = windowSize_x / 2.f - textBounds.size.x / 2.f;
                float textY = windowSize_y / 2.f - 120.f;
                gameOverText->setPosition(sf::Vector2f(textX, textY));
                gameOverText->setFillColor(sf::Color(83, 83, 83));
            }
        }
        void setMousPos(sf::Vector2i p_mousePos)
        {
            mousePos.x = p_mousePos.x;
            mousePos.y = p_mousePos.y;
        }

        void update(sf::Time deltaTime)
        {
            if (restartButton.restartButtonSprite)
                restartButton.restartButtonSpriteBounds = restartButton.restartButtonSprite->getGlobalBounds();
            restartButton.checkPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            if(playerDead == true && restartButton.restartButtonSpriteBounds.contains(mousePos) && restartButton.checkPressed == true)
            {
                ground.reset();
                obstacles.reset();
                knight.reset();
                scores.reset();
                gameSpeed = 8;

                playerDead = false;
            }
            else
            {
                ground.updateGround();
                obstacles.update(deltaTime);
                knight.update(deltaTime, obstacles.obstacles);
                clouds.updateClouds(deltaTime);
                scores.update();
            }
            fps.update();
        }

        void drawTo(sf::RenderWindow& window)
        {
            if(playerDead == true)
            {
                clouds.drawTo(window);
                if(ground.groundSprite) window.draw(*ground.groundSprite);
                obstacles.drawTo(window);
                if(scores.scoresText) window.draw(*scores.scoresText);
                if(scores.previousScoreText) window.draw(*scores.previousScoreText);
                if(scores.HIText) window.draw(*scores.HIText);
                if(knight.knight) window.draw(*knight.knight);
                if (gameOverText)
                window.draw(*gameOverText);
                if (restartButton.restartButtonSprite)
                    window.draw(*restartButton.restartButtonSprite);
                fps.drawTo(window);
            }
            else
            {
                clouds.drawTo(window);
                if(ground.groundSprite) window.draw(*ground.groundSprite);
                obstacles.drawTo(window);
                if(scores.scoresText) window.draw(*scores.scoresText);
                if(scores.previousScoreText) window.draw(*scores.previousScoreText);
                if(scores.HIText) window.draw(*scores.HIText);
                if(knight.knight) window.draw(*knight.knight);
                fps.drawTo(window);
            }
        }
};