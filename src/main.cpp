#include "simulation/simulation.h"

int main() 
{
    std::srand(static_cast<unsigned>(time(NULL)));

    Simulation().run();
}