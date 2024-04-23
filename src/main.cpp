#include "Arena.h"

int main() {
    srand(time(nullptr));

    Arena arena(800, 600, 150);
    arena.run();
    
    return 0;
}
