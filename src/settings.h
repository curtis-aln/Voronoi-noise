#pragma once

#include <SFML/Graphics.hpp>

struct SimulationSettings
{
	static const unsigned screen_width = 1920;
	static const unsigned screen_height = 950;

};

struct VoronoiSettings
{
	// This is the number of pixels in each cell, the less pixels per cell the more accurate the simulation will be
	static const int pixels_per_cell = 1; 
	static const bool blur = false;

	static const int cells_y = 15;
	static const int cells_x = cells_y * (SimulationSettings::screen_width / SimulationSettings::screen_height);
	static const unsigned cell_size = SimulationSettings::screen_width / cells_x;

	const sf::Color outside_color = sf::Color{ 30, 50, 50 };
	const sf::Color inside_color = sf::Color{ 250, 80, 250 };

};