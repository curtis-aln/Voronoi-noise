#include <iostream>

#include <SFML/Graphics.hpp>

#include <ctime>
#include <string>
#include <sstream>

#include <omp.h>


// screen dimensions
constexpr unsigned screenWidth = 1920;
constexpr unsigned screenHeight = 950;
constexpr float aspectRatio = float(screenWidth) / float(screenHeight);

constexpr int cellsY = 15;
constexpr int cellsX = cellsY * aspectRatio;

constexpr float cell_size = screenWidth / cellsX;

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

void setCaption(sf::RenderWindow& window, sf::Clock& clock) 
{
    // FPS management
    float timePerFrame = clock.restart().asSeconds();

    std::ostringstream oss;
    oss << "Voronoi Noise | fps:" << 1.0f / timePerFrame << "";
    std::string var = oss.str();
    window.setTitle(var);
}

int randint(int start, int end) 
{
    return rand() % (end - start) + start;
}

float randfloat(float start, float end) 
{
    return (float(rand()) / float((RAND_MAX)) * (end - start)) + start;
}


void generatePixelsArray(sf::VertexArray& pixels, const int screenWidth, const int screenHeight) 
{
    // this function generates a vertex array for all the pixels on the screen
    for (int x = 0; x < screenWidth; x++) 
    {
        for (int y = 0; y < screenHeight; y++) 
        {
            const int current = x + y * screenWidth;
            pixels[current].position = sf::Vector2f(x, y);
            pixels[current].color = sf::Color(randint(0, 255), randint(0, 255), randint(0, 255));
        }
    }
}

void drawGrid(sf::RenderWindow& window, std::vector<std::vector<Rectangle>>& grid) {
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

sf::Vector2i wrapIndex(int indexX, int indexY) 
{
    if (indexX >= cellsX)
        indexX = 0;

    if (indexY >= cellsY)
        indexY = 0;

	if (indexX < 0)
		indexX = cellsX - 1;

	if (indexY < 0)
		indexY = cellsY - 1;

    return {indexX, indexY};
}


float distance_squared(const sf::Vector2f& pos1, const sf::Vector2f& pos2) 
{
    sf::Vector2f delta = pos2 - pos1;
    return (delta.x * delta.x) + (delta.y * delta.y);
}


void colorPixles(sf::VertexArray& pixels, std::vector<sf::Vector2f>& point_positions, std::vector<std::vector<Rectangle>>& grid) 
{
    // The furthest distance any particle can be from a point is if the particle is in the middle of its cell and all points
    // around it are at their furthest corners, the left, right, up, down cells ill always be closer than the diagonal ones

    // for each pixel on the screen
#pragma omp parallel for schedule(static)
    for (int x = 0; x < screenWidth; x++) 
    {
        for (int y = 0; y < screenHeight; y++) 
        {
            // converting the 2d index to 1d
			int idx = y + x * screenHeight;
            sf::Vector2f currentPos = pixels[idx].position;

			// calculating the grid cell index of the current pixel
            int idxX = (int)(currentPos.x / cell_size);
            int idxY = (int)(currentPos.y / cell_size);

			// The color of the pixel is determined by the distance to the closest point in the grid.
            float closestDist = cell_size * cell_size;

            for (int i = -1; i <= 1; i++) 
            {
                for (int j = -1; j <= 1; j++) 
                {
                    sf::Vector2i index = wrapIndex(idxX + i, idxY + j);
					int pointIndex = index.x + index.y * cellsX;

                    float newDist = distance_squared(currentPos, point_positions[pointIndex]);

                    if (newDist < closestDist)
                        closestDist = newDist;
                }
            }

            // mapping the dist between 0 and 255
            int col = (closestDist / (cell_size * cell_size)) * 255;

            if (invert == true) col = 255 - col;
            col = std::clamp(col, 0, 255);

            // setting the attributes
            pixels[idx].position = sf::Vector2f{float(x), float(y)};
            pixels[idx].color = sf::Color(col, col, col);
        }
    }
}

void update_point_positions(std::vector<sf::Vector2f>& point_positions, std::vector<std::vector<Rectangle>>& grid) {
	for (size_t i = 0; i < cellsX; i++) {
		for (size_t j = 0; j < cellsY; j++) {
			point_positions[i + j * cellsX] = grid[i][j].iPos;
		}
	}
}

int main() 
{
    // setting up the window, FPS, and whatnot
    sf::RenderWindow window(sf::VideoMode({ screenWidth, screenHeight }), "Organic Noise");
    sf::Clock clock;
    std::srand(static_cast<unsigned>(time(NULL)));

    bool paused = false;
    float speed = 2.0;

    // the pixles array
    sf::VertexArray pixels(sf::PrimitiveType::Points, screenWidth * screenHeight);
    std::vector<std::vector<Rectangle>> grid = generateRectangleGrid(cellsX, cellsY,
        0, 0, screenWidth, screenHeight);

    std::vector<sf::Vector2f> point_positions;
	point_positions.resize(cellsX * cellsY);

    generatePixelsArray(pixels, screenWidth, screenHeight);
    colorPixles(pixels, point_positions, grid);

    bool drawg = true;

    // main loop
    while (window.isOpen()) 
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                // key is a pointer to the KeyPressed subtype struct — guaranteed valid
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();

                if (key->code == sf::Keyboard::Key::G)
                    drawg = not drawg;

                if (key->code == sf::Keyboard::Key::I)
                    invert = not invert;

                if (key->code == sf::Keyboard::Key::F)
                    speed += 0.1;

                if (key->code == sf::Keyboard::Key::S)
                    speed -= 0.1;

                if (key->code == sf::Keyboard::Key::Space)
                    paused = not paused;
            }
        }

        // clearing screen
        window.clear();

        updateRects(grid);
		update_point_positions(point_positions, grid);
        colorPixles(pixels, point_positions, grid);

        window.draw(pixels);

        if (drawg == true) {
            drawGrid(window, grid);
        }

        // drawing to screen
        setCaption(window, clock);
        window.display();
    }
}