#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <fstream>
#include <cmath>
#include <string>
#include <cstdlib>
#include <ctime> // Include this for seeding random number generator

// Constants
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const float PLANE_SPEED = 5.0f;

// Utility function to calculate Euclidean distance
float CalculateDistance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Class to manage the game
class Game {
private:
    sf::RenderWindow window;
    sf::RectangleShape plane;
    sf::CircleShape missile1, missile2;
    sf::Font font;
    sf::Text scoreText, highScoreText, gameOverText;
    int score = 0;
    int highScore = 0;
    bool gameOver = false;
    float missileSpeed = 2.0f;

    std::string highScoreFile = "highscore.txt";

    // Load high score from file
    void LoadHighScore() {
        std::ifstream file(highScoreFile);
        if (file.is_open()) {
            file >> highScore;
            file.close();
        }
    }

    // Save high score to file
    void SaveHighScore() {
        std::ofstream file(highScoreFile);
        if (file.is_open()) {
            file << highScore;
            file.close();
        }
    }

    // Reset game state
    void ResetGame() {
        plane.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
        missile1.setPosition(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
        missile2.setPosition(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
        score = 0;
        missileSpeed = 2.0f;
        gameOver = false;
    }

    // Handle missile movement
    void MoveMissiles() {
        sf::Vector2f planePos = plane.getPosition();

        // Missile 1
        sf::Vector2f missile1Pos = missile1.getPosition();
        sf::Vector2f direction1 = planePos - missile1Pos;
        float length1 = sqrt(pow(direction1.x, 2) + pow(direction1.y, 2));
        if (length1 != 0) {
            direction1 /= length1;
            missile1.move(direction1 * missileSpeed);
        }

        // Missile 2
        sf::Vector2f missile2Pos = missile2.getPosition();
        sf::Vector2f direction2 = planePos - missile2Pos;
        float length2 = sqrt(pow(direction2.x, 2) + pow(direction2.y, 2));
        if (length2 != 0) {
            direction2 /= length2;
            missile2.move(direction2 * missileSpeed);
        }
    }

    // Check collision
    bool CheckCollision() {
        float distance1 = CalculateDistance(plane.getPosition().x + 20, plane.getPosition().y + 10,
                                            missile1.getPosition().x + 10, missile1.getPosition().y + 10);
        float distance2 = CalculateDistance(plane.getPosition().x + 20, plane.getPosition().y + 10,
                                            missile2.getPosition().x + 10, missile2.getPosition().y + 10);

        return (distance1 < 30.0f || distance2 < 30.0f);
    }

    // Update game state
    void Update() {
        if (gameOver) return;

        MoveMissiles();

        // Check collision
        if (CheckCollision()) {
            gameOver = true;
            if (score > highScore) {
                highScore = score;
                SaveHighScore();
            }
        } else {
            score++;
            missileSpeed += 0.001f; // Gradually increase difficulty
        }
    }

    // Render all objects
    void Render() {
        window.clear();

        // Draw plane and missiles
        window.draw(plane);
        window.draw(missile1);
        window.draw(missile2);

        // Update and draw score
        scoreText.setString("Score: " + std::to_string(score));
        window.draw(scoreText);

        // Draw high score
        highScoreText.setString("High Score: " + std::to_string(highScore));
        window.draw(highScoreText);

        // Draw Game Over message
        if (gameOver) {
            gameOverText.setString("Game Over! Press R to Restart");
            window.draw(gameOverText);
        }

        window.display();
    }

public:
    Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Plane Apocalypse") {
        // Seed random number generator
        std::srand(std::time(nullptr));

        // Load font
        if (!font.loadFromFile("arial.ttf")) {
            throw std::runtime_error("Failed to load font");
        }

        // Initialize plane
        plane.setSize(sf::Vector2f(40.0f, 20.0f));
        plane.setFillColor(sf::Color::White);
        plane.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

        // Initialize missiles
        missile1.setRadius(10.0f);
        missile1.setFillColor(sf::Color::Red);
        missile1.setPosition(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);

        missile2.setRadius(10.0f);
        missile2.setFillColor(sf::Color::Red);
        missile2.setPosition(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);

        // Initialize text
        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setPosition(10.0f, 10.0f);

        highScoreText.setFont(font);
        highScoreText.setCharacterSize(20);
        highScoreText.setPosition(10.0f, 40.0f);

        gameOverText.setFont(font);
        gameOverText.setCharacterSize(30);
        gameOverText.setPosition(WINDOW_WIDTH / 4.0f, WINDOW_HEIGHT / 2.0f);

        // Load high score
        LoadHighScore();
    }

    void Run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (!gameOver && event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::W && plane.getPosition().y > 0) {
                        plane.move(0, -PLANE_SPEED);
                    } else if (event.key.code == sf::Keyboard::S && plane.getPosition().y < WINDOW_HEIGHT - 20) {
                        plane.move(0, PLANE_SPEED);
                    } else if (event.key.code == sf::Keyboard::A && plane.getPosition().x > 0) {
                        plane.move(-PLANE_SPEED, 0);
                    } else if (event.key.code == sf::Keyboard::D && plane.getPosition().x < WINDOW_WIDTH - 40) {
                        plane.move(PLANE_SPEED, 0);
                    }
                }

                if (gameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                    ResetGame();
                }
            }

            Update();
            Render();
        }
    }
};

int main() {
    try {
        Game game;
        game.Run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
