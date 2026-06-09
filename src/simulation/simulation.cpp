#include "simulation.h"


Simulation::Simulation() : window(sf::VideoMode({ screen_width, screen_height }), "Voronoi Noise")
{

}

void Simulation::run()
{
	while (running_)
	{
		poll_events();
		update();
		render();
	}
}

void Simulation::update()
{
    voronoi.update();
}

void Simulation::render()
{
    window.clear();

	setCaption();

    voronoi.render();

    window.display();
}

void Simulation::poll_events()
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
                voronoi.drawg_ = not voronoi.drawg_;

            if (key->code == sf::Keyboard::Key::I)
                voronoi.invert_ = not voronoi.invert_;

         
            if (key->code == sf::Keyboard::Key::Space)
                paused_ = not paused_;
        }
    }
}

void Simulation::setCaption()
{
	// FPS management
	float timePerFrame = clock.restart().asSeconds();

	std::ostringstream oss;
	oss << "Voronoi Noise | fps:" << 1.0f / timePerFrame << "";
	std::string var = oss.str();
	window.setTitle(var);
}