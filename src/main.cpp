#include "Arena.h"

int main() {
    srand(time(nullptr));
    /* std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes(); */ 	
    /* Arena arena(modes.front(), 100); */

    Arena arena(1920, 1080);
    arena.run();
    
    return 0;
}
