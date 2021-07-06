#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "engine.h"

void createmap(tile ***map, int x, int y)
{
    tile **tmp;
    int i, j;
    tmp = (tile**) malloc(y*sizeof(tile*));
    for (i = 0; i < y; i++)
    {
        tmp[i] = (tile*) malloc(x*sizeof(tile));
    }
    for (j=0; j<x; j++) //feltöltés empty-vel
    {
        for (i=0; i<y; i++)
        {
            tmp[i][j]=empty;
        }
    }
    *map = tmp;
}

void randomgenerate(tile** map, int x, int y)
{
    srand(time(0));
    int i, j;
    for (j=0; j<y; j++)
    {
        for (i=0; i<x; i++)
        {
            if (rand()%2)
            {
                map[i][j]=cell;
            }
            else
            {
                map[i][j]=empty;
            }
        }
    }
}

void resetmap(tile** map, int x, int y)
{
    int i, j;
    for (j=0; j<y; j++)
    {
        for (i=0; i<x; i++)
        {
            map[i][j]=empty;
        }
    }
}

void savefile(tile** map, int x, int y, char* filename)
{
    FILE *fp;
    fp = fopen(filename,"wt");
    fprintf(fp,"%d ",x);
    fprintf(fp,"%d ",y);
    int i, j;
    for (j=0; j<y; j++)
    {
        for (i=0; i<x; i++)
        {
            fprintf(fp,"%d ",map[i][j]);
        }
    }
    fclose(fp);
}

void loadfile(tile** map, char* filename, int *outx, int *outy)
{
    FILE *fp;
    fp = fopen(filename,"rt");
    int x,y;

}

void turn(tile** map, int x, int y)
{
    int i, j, k;
    int counter=0;
    for (j=0; j<y; j++) //elhaló és születõ sejtek megjelölése
    {
        for (i=0; i<x; i++)
        {
            for (k=1; k<=8; k++)
            {
                switch (k)
                {
                    case 1: //bal felül
                        if ((i!=0) && (j!=0))
                        {
                            if ((map[i-1][j-1]==cell) || (map[i-1][j-1]==markedfordeath))
                            {
                                counter++;
                            }
                        }
                        break;
                    case 2: //felül
                        if (j!=0)
                        {
                            if ((map[i][j-1]==cell) || (map[i][j-1]==markedfordeath))
                            {
                                counter++;
                            }
                        }
                        break;
                    case 3: //jobb felül
                        if ((i!=x-1) && (j!=0))
                        {
                            if ((map[i+1][j-1]==cell) || (map[i+1][j-1]==markedfordeath))
                            {
                                counter++;
                            }
                        }
                        break;
                    case 4: //balra
                        if (i!=0)
                        {
                            if ((map[i-1][j]==cell) || (map[i-1][j]==markedfordeath))
                            {
                                counter++;
                            }
                        }
                        break;
                    case 5: //jobbra
                        if (i!=x-1)
                        {
                            if ((map[i+1][j]==cell) || (map[i+1][j]==markedfordeath))
                            {
                                counter++;
                            }
                        }
                        break;
                    case 6: //bal alul
                        if ((i!=0) && (j!=y-1))
                        {
                            if ((map[i-1][j+1]==cell) || (map[i-1][j+1]==markedfordeath))
                            {
                                counter++;
                            }
                        }
                        break;
                    case 7: //alul
                        if (j!=y-1)
                        {
                            if ((map[i][j+1]==cell) || (map[i][j+1]==markedfordeath))
                            {
                                counter++;
                            }
                        }
                        break;
                    case 8: //jobb alul
                        if ((i!=x-1) && (j!=y-1))
                        {
                            if ((map[i+1][j+1]==cell) || (map[i+1][j+1]==markedfordeath))
                            {
                                counter++;
                            }
                        }
                        break;
                }
            }
            switch (map[i][j])
            {
                case cell: //elhaló sejt megjelölés
                    if ((counter<2) || (counter>3))
                    {
                        map[i][j]=markedfordeath;
                    }
                    break;
                case empty: //születõ sejt megjelölése
                    if (counter==3)
                    {
                        map[i][j]=markedforborn;
                    }
                    break;
                default: break;
            }
            counter=0;
        }
    }

    for (j=0; j<y; j++) //megjelölt sejtek végleges állapotba helyezése
    {
        for (i=0; i<x; i++)
        {
            switch (map[i][j])
            {
                case markedfordeath: //elhalásra megjelölt sejt elpusztítása
                    map[i][j]=empty;
                    break;
                case markedforborn: //születésre megjelölt sejt létrehozása
                    map[i][j]=cell;
                    break;
                default: break;
            }
        }
    }
}
