#pragma once

#include "../settings.h"
#include "../voronoi/voronoi.h"
#include <SFML/Graphics.hpp>



class Simulation : public SimulationSettings
{
	sf::RenderWindow window;
	sf::Clock clock;

	bool running_ = true;
	bool paused_ = false;

	Voronoi voronoi{ &window };

public:
	Simulation();
	void run();

private:
	void update();
	void render();
	void poll_events();
	void setCaption();
};