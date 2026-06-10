#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

#include "rect.h"
#include "../settings.h"

#include <ctime>
#include <string>
#include <sstream>

#include <omp.h>

inline int randint(int start, int end)
{
	return rand() % (end - start) + start;
}

inline float randfloat(float start, float end)
{
	return (float(rand()) / float((RAND_MAX)) * (end - start)) + start;
}

inline float distance_squared(const sf::Vector2f& pos1, const sf::Vector2f& pos2)
{
    sf::Vector2f delta = pos2 - pos1;
    return (delta.x * delta.x) + (delta.y * delta.y);
}


class Voronoi : public VoronoiSettings, public SimulationSettings
{
    sf::RenderWindow* window_;

    const int pixel_array_size_x = screen_width / pixels_per_cell;
    const int pixel_array_size_y = screen_height / pixels_per_cell;

    sf::VertexArray pixels_{ sf::PrimitiveType::Points, size_t(pixel_array_size_x) * size_t(pixel_array_size_y) };
	std::vector<std::vector<Rectangle>> grid_;
    std::vector<sf::Vector2f> point_positions;

    sf::RenderTexture renderTex{ {size_t(pixel_array_size_x), size_t(pixel_array_size_y)} };
    sf::Shader blurShader;
    


public:
    bool invert_ = false;
    bool drawg_ = true;


	Voronoi(sf::RenderWindow* window) : window_(window)
	{
        generateRectangleGrid();
        generatePixelsArray();

        point_positions.resize(cells_x * cells_y);

        auto result = blurShader.loadFromFile("blur.frag", sf::Shader::Type::Fragment);
        if (!result) {
            // shader failed silently before — this is likely why nothing happened
            std::cerr << "Shader failed to load!\n";
        }
	}

	void update()
	{
        updateRects();
        update_point_positions();
        colorPixles();
	}

	void render()
	{
        renderTex.clear();
        renderTex.draw(pixels_);  // draw points at 1:1
        renderTex.display();

        sf::Sprite scaled(renderTex.getTexture());
        scaled.setScale({pixels_per_cell, pixels_per_cell});
        
        blurShader.setUniform("texture", sf::Shader::CurrentTexture);
        blurShader.setUniform("texelSize", sf::Glsl::Vec2(
            1.0f / (float)pixel_array_size_x,
            1.0f / (float)pixel_array_size_y
        ));
        blurShader.setUniform("blur_radius", .2f); // tweak this

        window_->draw(scaled, &blurShader);

        if (drawg_ == true) {
            drawGrid();
        }

	}

private:
    void generatePixelsArray()
    {
        // this function generates a vertex array for all the pixels on the screen


        for (int x = 0; x < pixel_array_size_x; x++)
        {
            for (int y = 0; y < pixel_array_size_y; y++)
            {
                const int current = x + y * pixel_array_size_x;
                pixels_[current].position = sf::Vector2f(x, y);
                pixels_[current].color = sf::Color(randint(0, 255), randint(0, 255), randint(0, 255));
            }
        }
    }

    void drawGrid() {
        for (std::vector<Rectangle>& row : grid_) 
        {
            for (Rectangle& rect : row) 
            {
                sf::RectangleShape drawRect;
                drawRect.setOutlineThickness(1);
                drawRect.setOutlineColor(sf::Color::White);
                drawRect.setFillColor(sf::Color(0, 0, 0, 0));

                drawRect.setPosition(sf::Vector2f(rect.x, rect.y));
                drawRect.setSize(sf::Vector2f(rect.w, rect.h));
                window_->draw(drawRect);

                // drawing the point inside the rect
                sf::CircleShape point(3);
                point.setFillColor(sf::Color(255, 0, 0));
                point.setPosition(rect.iPos);
                window_->draw(point);
            }
        }
    }

    void updateRects() {
        for (std::vector<Rectangle>& row : grid_) 
        {
            for (Rectangle& rect : row) 
            {
                rect.update();
            }
        }
    }

    void generateRectangleGrid() 
    {
        float v = 0.6f;

        // initilising and fillinng the vertex array
        for (size_t i = 0; i < cells_x; i++) 
        {
            std::vector<Rectangle> cellsRow;

            for (size_t j = 0; j < cells_y; j++) 
            {
                Rectangle rect{ i * cell_size, j * cell_size, cell_size, cell_size };
                rect.iPos = sf::Vector2f(randfloat(rect.x, rect.x + rect.w), randfloat(rect.y, rect.y + rect.h));
                rect.iVel = sf::Vector2f(randfloat(-v, v), randfloat(-v, v));


                cellsRow.push_back(rect);
            }
            grid_.push_back(cellsRow);
        }
    }

    sf::Vector2i wrapIndex(int indexX, int indexY)
    {
        if (indexX >= cells_x)
            indexX = 0;

        if (indexY >= cells_y)
            indexY = 0;

        if (indexX < 0)
            indexX = cells_x - 1;

        if (indexY < 0)
            indexY = cells_y - 1;

        return { indexX, indexY };
    }

    void colorPixles()
    {
        // The furthest distance any particle can be from a point is if the particle is in the middle of its cell and all points
        // around it are at their furthest corners, the left, right, up, down cells ill always be closer than the diagonal ones

        // for each pixel on the screen
#pragma omp parallel for schedule(static)
        for (int x = 0; x < pixel_array_size_x; x++)
        {
            for (int y = 0; y < pixel_array_size_y; y++)
            {
                // converting the 2d index to 1d
                int idx = y + x * pixel_array_size_y;
                sf::Vector2f currentPos = pixels_[idx].position;
				currentPos.x *= pixels_per_cell;
				currentPos.y *= pixels_per_cell;

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
                        int pointIndex = index.x + index.y * cells_x;

                        float newDist = distance_squared(currentPos, point_positions[pointIndex]);

                        if (newDist < closestDist)
                            closestDist = newDist;
                    }
                }

                // mapping the dist between 0 and 255
                int col = (closestDist / (cell_size * cell_size)) * 255;

                if (invert_ == true) col = 255 - col;
                col = std::clamp(col, 0, 255);

                // setting the attributes
                pixels_[idx].position = sf::Vector2f{ float(x), float(y) };
                pixels_[idx].color = sf::Color(col, col, col);
            }
        }
    }

    void update_point_positions() {
        for (size_t i = 0; i < cells_x; i++) {
            for (size_t j = 0; j < cells_y; j++) {
                point_positions[i + j * cells_x] = grid_[i][j].iPos;
            }
        }
    }
};