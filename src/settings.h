#pragma once



struct SimulationSettings
{
	static const unsigned screen_width = 1920;
	static const unsigned screen_height = 950;

};

struct VoronoiSettings
{
	static const int cells_y = 15;
	static const int cells_x = cells_y * (SimulationSettings::screen_width / SimulationSettings::screen_height);
	static const unsigned cell_size = SimulationSettings::screen_width / cells_x;

};