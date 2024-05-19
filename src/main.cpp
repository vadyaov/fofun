#include "Arena.h"

int main() {
    srand(time(nullptr));
    /* std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes(); */ 	
    /* Arena arena(modes.front(), 10); */

    Arena arena(800, 600);
    arena.run();
    
    return 0;
}
