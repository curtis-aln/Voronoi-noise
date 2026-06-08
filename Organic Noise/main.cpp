#include <iostream>

#include <SFML/Graphics.hpp>

#include <ctime>
#include <string>
#include <sstream>

const int screenWidth = 1250;
const int screenHeight = 700;
const sf::Vector2u cellIdx(15, 12);

int multiplier = 7;
bool invert = false;

struct Rectangle {
    float x{};
    float y{};
    float w{};
    float h{};

    sf::Vector2f iPos{};
    sf::Vector2f iVel{};

    bool checkPointCollision(sf::Vector2f position) {
        return position.x >= x and position.x < (x + w) and position.y >= y and position.y < (y + h);
    }

    void update() {
        iPos += iVel;

        if (iPos.x <= x or iPos.x >= x + w)
            iVel.x *= -1;

        if (iPos.y <= y or iPos.y >= y + h)
            iVel.y *= -1;
    }
};

void setCaption(sf::RenderWindow& window, sf::Clock& clock) {
    // FPS management
    float timePerFrame = clock.restart().asSeconds();

    std::ostringstream oss;
    oss << "Organic Noise | Fps:" << 1.0f / timePerFrame << "";
    std::string var = oss.str();
    window.setTitle(var);
}

int randint(int start, int end) {
    return rand() % (end - start) + start;
}

float randfloat(float start, float end) {
    return (float(rand()) / float((RAND_MAX)) * (end - start)) + start;
}


void generatePixelsArray(sf::VertexArray* pixels, const int screenWidth, const int screenHeight) {
    // this function generates a vertex array for all the pixels on the screen
    for (int x = 0; x < screenWidth; x++) {
        for (int y = 0; y < screenHeight; y++) {
            const int current = x + y * screenWidth;
            (*pixels)[current].position = sf::Vector2f(x, y);
            (*pixels)[current].color = sf::Color(randint(0, 255), randint(0, 255), randint(0, 255));
        }
    }
}

void drawGrid(sf::RenderWindow& window, std::vector<std::vector<Rectangle>> grid) {
    for (std::vector<Rectangle>& row : grid) {
        for (Rectangle& rect : row) {
            sf::RectangleShape drawRect;
            drawRect.setOutlineThickness(1);
            drawRect.setOutlineColor(sf::Color::White);
            drawRect.setFillColor(sf::Color(0, 0, 0, 0));

            drawRect.setPosition(sf::Vector2f(rect.x, rect.y));
            drawRect.setSize(sf::Vector2f(rect.w, rect.h));
            window.draw(drawRect);

            // drawing the point inside the rect
            sf::CircleShape point(3);
            point.setFillColor(sf::Color(255, 0, 0));
            point.setPosition(rect.iPos);
            window.draw(point);
        }
    }
}

void updateRects(std::vector<std::vector<Rectangle>>& grid) {
    for (std::vector<Rectangle>& row : grid) {
        for (Rectangle& rect : row) {
            rect.update();
        }
    }
}

std::vector<std::vector<Rectangle>> generateRectangleGrid(const int cellsX, const int cellsY,
    float minX, float minY, float maxX, float maxY) {

    // calculating the cell sizes
    float cellWidth = (maxX - minX) / cellsX;
    float cellHeight = (maxY - minY) / cellsY;

    std::vector<std::vector<Rectangle>> grid;

    float v = 0.6f;

    // initilising and fillinng the vertex array
    for (size_t i = 0; i < cellsX; i++) {
        std::vector<Rectangle> cellsRow;
        for (size_t j = 0; j < cellsY; j++) {
            Rectangle rect{ i * cellWidth, j * cellHeight, cellWidth, cellHeight };
            rect.iPos = sf::Vector2f(randfloat(rect.x, rect.x + rect.w), randfloat(rect.y, rect.y + rect.h));
            rect.iVel = sf::Vector2f(randfloat(-v, v), randfloat(-v, v));


            cellsRow.push_back(rect);
        }
        grid.push_back(cellsRow);
    }

    return grid;
}

sf::Vector2i wrapIndex(int indexX, int indexY, std::vector<std::vector<Rectangle>>& grid) {
    if (indexX >= grid.size())
        indexX = 0;

    if (indexY >= grid[0].size())
        indexY = 0;

    return sf::Vector2i(indexX, indexY);
}


float distance(sf::Vector2f pos1, sf::Vector2f pos2) {
    sf::Vector2f delta = pos2 - pos1;
    return (delta.x * delta.x) + (delta.y * delta.y);
}

void colorPixles(sf::VertexArray* pixels, std::vector<std::vector<Rectangle>> grid) {
    for (int x = 0; x < screenWidth; x++) {
        for (int y = 0; y < screenHeight; y++) {

            float closestDist = 100000;
            int idx = x + y * screenWidth;
            sf::Vector2f currentPos = (*pixels)[idx].position;

            int idxX = (int)(currentPos.x / grid[0][0].w);
            int idxY = (int)(currentPos.y / grid[0][0].h);

            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    sf::Vector2i index = wrapIndex(idxX + i, idxY + j, grid);
            
                    float newDist = distance(currentPos, grid[index.x][index.y].iPos);
                    if (newDist < closestDist)
                        closestDist = newDist;
                }
            }

            // mapping the dist between 0 and 255
            int col = ((int)closestDist / multiplier);
            if (invert == true) col = 255 - col;
            if (col > 255) col = 255;
            if (col < 0) col = 0;
     
            (*pixels)[idx].position = sf::Vector2f(x, y);
            (*pixels)[idx].color = sf::Color(col, col, col);
        }
    }
}


int main() {
    // setting up the window, FPS, and whatnot
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Organic Noise");
    sf::Clock clock;
    std::srand(static_cast<unsigned>(time(NULL)));

    bool paused = false;
    float speed = 2.0;

    // the pixles array
    sf::VertexArray pixels(sf::Points, screenWidth * screenHeight);
    std::vector<std::vector<Rectangle>> grid = generateRectangleGrid(cellIdx.x, cellIdx.y,
        0, 0, screenWidth, screenHeight);

    generatePixelsArray(&pixels, screenWidth, screenHeight);
    colorPixles(&pixels, grid);

    bool drawg = true;

    // main loop
    while (window.isOpen()) {
        // getting user input
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Escape:
                    window.close();
                    break;

                case sf::Keyboard::G:
                    drawg = not drawg;
                    break;

                case sf::Keyboard::I:
                    invert = not invert;
                    break;


                case sf::Keyboard::Space:
                    paused = not paused;
                    break;

                case sf::Keyboard::F:
                    speed += 0.1;
                    break;

                case sf::Keyboard::S:
                    speed -= 0.1;
                    break;

                case sf::Keyboard::O:
                    multiplier += 1;
                    break;

                case sf::Keyboard::P:
                    multiplier -= 1;
                    break;
                }
            }
        }

        // clearing screen
        window.clear();

        updateRects(grid);
        colorPixles(&pixels, grid);

        window.draw(pixels);

        if (drawg == true) {
            drawGrid(window, grid);
        }

        // drawing to screen
        setCaption(window, clock);
        window.display();
    }
}