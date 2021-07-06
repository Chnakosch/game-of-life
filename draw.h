#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

typedef enum menubutton
{
    defaultstate,

    meret,
    random,
    reset,
    load,
    save,
    manualturn,
    autoturn,
    speed,
    quit,

    x10,
    x20,
    x31,
    x62,
    x124,
    x155,

    s001,
    s005,
    s01,
    s02,
    s05,
    s1
}menubutton;

void draw(tile** map, int x, int y);

#endif // DRAW_H_INCLUDED
