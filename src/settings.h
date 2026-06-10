#pragma once



struct SimulationSettings
{
	static const unsigned screen_width = 1920;
	static const unsigned screen_height = 950;

};

struct VoronoiSettings
{
	// This is the number of pixels in each cell, the less pixels per cell the more accurate the simulation will be
	static const int pixels_per_cell = 2; 

	static const int cells_y = 15;
	static const int cells_x = cells_y * (SimulationSettings::screen_width / SimulationSettings::screen_height);
	static const unsigned cell_size = SimulationSettings::screen_width / cells_x;

};