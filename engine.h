#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

typedef enum tile
{
    empty,
    cell,
    markedfordeath,
    markedforborn
}tile;

void createmap(tile ***map, int x, int y);
void randomgenerate(tile** map, int x, int y);
void resetmap(tile** map, int x, int y);
void savefile(tile** map, int x, int y, char* filename);
void loadfile(tile** map, char* filename, int* outx, int* outy); //????????????????
void turn(tile** map, int x, int y);
#endif // ENGINE_H_INCLUDED
