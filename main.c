#include <stdio.h>
#include <stdlib.h>
#include "engine.h"
#include "draw.h"

int main(int argc, char *argv[])
{
    const int size=31;
    tile **map;

    createmap(&map,size,size);

    draw(map,size,size);

    free(map[0]);
    free(map);
    return 0;
}
