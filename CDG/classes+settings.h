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
        sf::SoundBuffer cooldownBuffer;
        sf::SoundBuffer dashBuffer;
        std::optional<sf::Sound> dieSound;
        std::optional<sf::Sound> jumpSound;
        std::optional<sf::Sound> pointSound;
        std::optional<sf::Sound> cooldownSound;
        std::optional<sf::Sound> dashSound;


        SoundManager()
        {
            if(dieBuffer.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/die.wav"))
                dieSound.emplace(dieBuffer);
            if(jumpBuffer.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/jump.wav"))
                jumpSound.emplace(jumpBuffer);
            if(pointBuffer.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/point.wav"))
                pointSound.emplace(pointBuffer);
            if(cooldownBuffer.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/cooldown.wav"))
                cooldownSound.emplace(cooldownBuffer);
            if(dashBuffer.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/dash.wav"))
                dashSound.emplace(dashBuffer);
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
        //sf::Texture knightTex;
        sf::FloatRect knightBounds;
        SoundManager soundManager;
        //std::array<sf::IntRect, 8> frames;
        sf::Time timeTracker;
        int animationCounter{0};
        bool dieSoundPlayed = false;
        bool dashReadySoundPlayed = false;

        // --- STATE ENGINE CONTROLS ---
        enum class KnightState { Normal, Dashing, Dead };
        KnightState currentState = KnightState::Normal;

        // --- MULTI-PNG TEXTURE SYSTEM ---
        sf::Texture walkSheet; //original
        sf::Texture jumpSheet; //jump
        sf::Texture dashSheet; //dash
        sf::Texture deathSheet; //death
        sf::Texture* activeTexture{nullptr}; // point to active texture

        // --- DISTINCT FRAME STORAGE LAYOUTS ---
        std::array<sf::IntRect, 8> walkFrames; // replaced "frames"
        std::array<sf::IntRect, 5> jumpFrames; 
        std::array<sf::IntRect, 6> dashFrames;
        std::array<sf::IntRect, 12> deathFrames;

        // --- MECHANICS PARAMETERS ---
        sf::Time dashTimer = sf::Time::Zero;
        sf::Time dashDuration = sf::seconds(0.2f);
        sf::Vector2f dashDirection{0.f , 0.f};
        float dashSpeed = 25.f;
        bool isInvincible = false;
        sf::Time dashCooldownTimer = sf::seconds(10.f);
        sf::Time dashCooldownDuration = sf::seconds(10.f);
        bool isDashReady = true;

        bool pendingPowerUpRoll = false;

        // Dynamic Death Physics variables
        float deathSlideSpeed = 0.f;
        float friction = 0.95f;
        float deathAnimDelay = 0.08f;
        sf::Time deathAnimTimer = sf::Time::Zero;


        Knight()
        {
            // 1. Load all 4 files safely
            if(!walkSheet.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/PlayerSpriteSheet.png") ||
               !jumpSheet.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/KnightJump.png") ||
               !dashSheet.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/KnightDash.png") ||
               !deathSheet.loadFromFile("C:/Users/bkacp/Desktop/CDG/Assets/KnightDeath.png"))
            {
                std::cout<<"Error loading one or more Knight texture sheets!"<<std::endl;
            }
             
            // 2. Initialize your core sprite container
            activeTexture = &walkSheet;
            knight.emplace(*activeTexture);
            knight->setScale(sf::Vector2f(2,2));

            // 3. Slice Walking Sheets
            for(int i = 0; i < walkFrames.size(); i++)
            {
                walkFrames[i] = sf::IntRect(sf::Vector2i(i * 96, 0), sf::Vector2i(96, 84));
            }

            // 4. Slice Jumping Sheets
            for(int i = 0; i < jumpFrames.size(); i++)
            {
                jumpFrames[i] = sf::IntRect(sf::Vector2i(i * 96, 0), sf::Vector2i(96, 84));
            }

            // 5. Slice Dashing Sheets
            for(int i = 0; i < dashFrames.size(); i++)
            {
                dashFrames[i] = sf::IntRect(sf::Vector2i(i * 96, 0), sf::Vector2i(96, 84));
            }

            // 6. Slice Death Sheets
            for(int i = 0; i < deathFrames.size(); i++)
            {
                deathFrames[i] = sf::IntRect(sf::Vector2i(i * 96, 0), sf::Vector2i(96, 84));
            }

            // 7. Establish Startup Defaults
            knight->setTextureRect(walkFrames[0]);
            knightPos = knight->getPosition();

        }

        void handleDashInput (sf::Vector2f mousePos)
        {
            // Check if 10 seconds of real-time have passed since the last dash lock
            isDashReady = (dashCooldownTimer >= dashCooldownDuration);

            if(currentState == KnightState::Normal && isDashReady && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
            {
                sf::Vector2f targetVector;

                if (mousePos.x > knightPos.x)
                {
                    // MOUSE IS TO THE RIGHT: Dash towards cursor (allows vertical)
                    targetVector = mousePos - knightPos;
                }
                else
                {
                    // MOUSE IS TO THE LEFT: Lock X to knight, follow mouse Y
                    sf::Vector2f adjustedTarget = sf::Vector2f(knightPos.x, mousePos.y);
                    targetVector = adjustedTarget - knightPos;
                }

                float length = std::sqrt(targetVector.x * targetVector.x + targetVector.y * targetVector.y);
                if(length != 0.f)
                {
                    dashDirection = targetVector / length;
                }
                else
                {
                    dashDirection = sf::Vector2f(1.f, 0.f);
                }

                currentState = KnightState::Dashing;
                isInvincible = true;
                dashTimer = sf::Time::Zero;
                animationCounter = 0; 
                
                // LOCK THE COOLDOWN: Reset the time counter to 0 seconds
                dashCooldownTimer = sf::Time::Zero;

                if (soundManager.dashSound)
                {
                    soundManager.dashSound->play();
                }

                // --- RESET READY SOUND GATE ---
                dashReadySoundPlayed = false;

                activeTexture = &dashSheet;
                knight->setTexture(*activeTexture);
                knight->setTextureRect(dashFrames[0]);
            }
        }

        void update(sf::Time& deltaTime, std::vector<std::unique_ptr<Obstacle>>& obstacles, sf::Vector2f mousePos)
        {
            if(!knight) return;

            knightPos = knight->getPosition();
            knightBounds = knight->getGlobalBounds();
            sf::Vector2f boundsSize = knightBounds.size;

            float shrinkX = 120.f;
            float shrinkY = 80.f;

            knightBounds = sf::FloatRect(
                sf::Vector2f(knightBounds.position.x + (shrinkX / 2.f), knightBounds.position.y + (shrinkY / 2.f)),
                sf::Vector2f(boundsSize.x - shrinkX, boundsSize.y - shrinkY)
            ); 
            
            timeTracker += deltaTime;

            // --- TICK THE COOLDOWN CLOCK ---
            if(!playerDead)
            {
                dashCooldownTimer += deltaTime; // Constantly count up towards 10 seconds

                if (dashCooldownTimer >= dashCooldownDuration)
                {
                    if (!dashReadySoundPlayed)
                    {
                        if (soundManager.cooldownSound)
                        {
                            soundManager.cooldownSound->play();
                        }
                        dashReadySoundPlayed = true; // Lock the gate so it plays exactly once
                    }
                }

                handleDashInput(mousePos);      // Run input check
            }

            // ADVANCED COLLISION LOOP
            if(currentState != KnightState::Dead)
            {
                for(size_t i = 0; i < obstacles.size(); i++)
                {
                    if(knightBounds.findIntersection(obstacles[i]->obstacleBounds).has_value())
                    {
                        if(isInvincible)
                        {
                            obstacles.erase(obstacles.begin() + i);
                            i--; 

                            if((rand() % 100) + 1 <= 10)
                            {
                                std::cout << "Power-up Drop Rolled Successfully!" << std::endl;
                            }
                        }
                        else
                        {
                            playerDead = true;
                            currentState = KnightState::Dead;
                            animationCounter = 0;
                            deathAnimTimer = sf::Time::Zero;

                            activeTexture = &deathSheet;
                            knight->setTexture(*activeTexture);
                            knight->setTextureRect(deathFrames[0]);

                            deathSlideSpeed = gameSpeed; 
                            //knightMotion = sf::Vector2f(0.f, knightMotion.y); // Keep X at 0 so screen stops around him
                            gameSpeed = 0.f;
                        }
                    }
                }
            }

            // ===========================================================
            // STATE MACHINE CONTROLS
            // ===========================================================
            
            // --- BRANCH A: DASHING (Camera Tracking Simulation) ---
            if(!playerDead && currentState == KnightState::Dashing)
            {
                dashTimer += deltaTime;

                // SCREEN MOVEMENT FIX: 
                // We set his horizontal speed to 0 so he doesn't leave his spot on screen.
                // Instead, we multiply your global gameSpeed so the world flies past him!
                knightMotion.x = 0.f; 
                knightMotion.y = dashDirection.y * dashSpeed; // Allow vertical diving/climbing
                
                // Dynamically adjust the global scrolling speed to simulate high momentum
                gameSpeed = 35.f; 

                // Run Dash Animation Loop (Using your new array size: 6)
                int dashFrameIndex = (animationCounter / 3) % dashFrames.size();
                knight->setTextureRect(dashFrames[dashFrameIndex]);
                animationCounter++;

                // Handle landing limits mid-dash so he doesn't clip through floor
                if(knightPos.y >= groundLevel && knightMotion.y > 0.f)
                {
                    knight->setPosition(sf::Vector2f(knightPos.x, groundLevel));
                    knightMotion.y = 0.f;
                }

                if(dashTimer >= dashDuration)
                {
                    currentState = KnightState::Normal;
                    isInvincible = false;
                    knightMotion = sf::Vector2f(0.f, 0.f);
                    gameSpeed = 8.f; // Revert game speed to standard configuration
                }

                knight->move(knightMotion);
            }
            // --- BRANCH B: NORMAL RUNNING / JUMPING PIPELINES ---
            else if(!playerDead && currentState == KnightState::Normal)
            {
                knightMotion.x = 0.f; // Keep horizontal locked in place

                if(knightPos.y >= groundLevel)
                {
                    knight->setPosition(sf::Vector2f(knight->getPosition().x, groundLevel));
                    knightMotion.y = 0.f;

                    if(activeTexture != &walkSheet)
                    {
                        activeTexture = &walkSheet;
                        knight->setTexture(*activeTexture);
                        animationCounter = 0;
                    }

                    walk();

                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) == true)
                    {
                        animationCounter = 0;
                        knightMotion.y = -20.f; 
                        
                        activeTexture = &jumpSheet;
                        knight->setTexture(*activeTexture);
                        knight->setTextureRect(jumpFrames[0]);
                        
                        if(soundManager.jumpSound) soundManager.jumpSound->play();
                    }
                }
                else if(knightPos.y < groundLevel)
                {
                    knightMotion.y += 1.f; // Gravity pulling the knight down
                    
                    if(activeTexture != &jumpSheet)
                    {
                        activeTexture = &jumpSheet;
                        knight->setTexture(*activeTexture);
                    }

                    // --- PHYSICS-BASED JUMP ANIMATION ---
                    // Instead of using animationCounter, we look at knightMotion.y to pick the frame!
                    int jumpFrameIndex = 2; // Default to Frame 2 (The peak/weightless frame)

                    if (knightMotion.y < -12.f)      jumpFrameIndex = 0; // Fast Rising
                    else if (knightMotion.y < -3.f)  jumpFrameIndex = 1; // Slowing down near peak
                    else if (knightMotion.y > 12.f)  jumpFrameIndex = 4; // Fast Falling
                    else if (knightMotion.y > 3.f)   jumpFrameIndex = 3; // Beginning to fall

                    // Apply the frame safely
                    knight->setTextureRect(jumpFrames[jumpFrameIndex]);
                }

                knight->move(knightMotion);
            }
            // --- BRANCH C: DEATH PROCESS (Using your new 12 frames container) ---
            else if(currentState == KnightState::Dead)
            {
                if(knightPos.y < groundLevel)
                {
                    knightMotion.y += 1.f;
                }
                else
                {
                    knight->setPosition(sf::Vector2f(knight->getPosition().x, groundLevel));
                    knightMotion.y = 0.f;

                    deathSlideSpeed *= friction;
                    if(deathSlideSpeed < 0.1f) deathSlideSpeed = 0.f;
                }

                // The background handles sliding past him when dead; player stays centered
                knightMotion.x = deathSlideSpeed;

                // Animating through your expanded 12 death frames
                deathAnimTimer += deltaTime;
                if(deathSlideSpeed > 0.f)
                {
                    deathAnimDelay = 0.05f + (1.f / (deathSlideSpeed + 1.f)) * 0.12f;
                }
                else
                {
                    deathAnimDelay = 0.20f; 
                }

                if(deathAnimTimer.asSeconds() > deathAnimDelay)
                {
                    // Protect your new array capacity ceiling limit (12 - 1 = 11)
                    if(animationCounter < deathFrames.size() - 1)
                    {
                        animationCounter++;
                        knight->setTextureRect(deathFrames[animationCounter]);
                    }
                    deathAnimTimer = sf::Time::Zero;
                }

                knight->move(knightMotion);

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
            int frameIndex = (animationCounter / 3) % walkFrames.size();
            knight->setTextureRect(walkFrames[frameIndex]);
            animationCounter++;
        }

        void reset()
        {
            knightMotion = sf::Vector2f(0.f, 0.f);
            currentState = KnightState::Normal;
            isInvincible = false;
            dashTimer = sf::Time::Zero;
            animationCounter = 0;
            deathSlideSpeed = 0.f;
            
            // Give the player a full charge immediately on game reset
            dashCooldownTimer = sf::seconds(10.f); 

            dashReadySoundPlayed = true;

            if (knight)
            {
                knight->setPosition(sf::Vector2f(windowSize_x / 2.f - windowSize_x / 4.f, groundLevel));
                activeTexture = &walkSheet;
                knight->setTexture(*activeTexture);
                knight->setTextureRect(walkFrames[0]);
            }

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
                clouds.updateClouds(deltaTime);
                scores.update();
            }

            knight.update(deltaTime, obstacles.obstacles, mousePos);

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