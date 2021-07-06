#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_ttf.h>
#include "engine.h"
#include "draw.h"

static void menubox(SDL_Surface* screen, int y, menubutton buttontype, menubutton* menukor, char* text)
{
    SDL_Color white = {255, 255, 255};
    rectangleRGBA(screen, 50, y, 253, y+44, 255, 255, 255, 255); //körvonal
    SDL_Rect hova = { 0, 0, 0, 0 };
    TTF_Font *lfont;
    TTF_Init();
    SDL_Surface *felirat;
    lfont = TTF_OpenFont("LiberationSerif-Regular.ttf", 22);
    if (!lfont) {
        fprintf(stderr, "Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(2);
    }
    felirat = TTF_RenderUTF8_Blended(lfont, text, white);
    hova.x = (303-felirat->w)/2;
    hova.y = y+11;
    SDL_BlitSurface(felirat, NULL, screen, &hova);
    SDL_FreeSurface(felirat);
    TTF_CloseFont(lfont);
    int i;
    for (i=y; i<=y+44; i++)
    {
        menukor[i]=buttontype;
    }
}

static void drawmenu(SDL_Surface* screen, menubutton* menukor)
{
    boxRGBA(screen, 0, 0, 302, 619, 0, 0, 0, 255); //clrscr
    menubox(screen,44,meret,menukor,"Meret valtoztatas");
    menubox(screen,134,random,menukor,"Random elhelyezes");
    menubox(screen,179,reset,menukor,"Teljes torles");
    menubox(screen,224,load,menukor,"Allas betoltese");
    menubox(screen,269,save,menukor,"Allas mentese");
    menubox(screen,359,manualturn,menukor,"Leptetes");
    menubox(screen,404,autoturn,menukor,"Automatikus leptetes");
    menubox(screen,449,speed,menukor,"Sebesseg beallitasa");
    menubox(screen,539,quit,menukor,"Kilepes");
}

static Uint32 idozit(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}

static int input_text(char* dest, int x, int y, int sz, int m, SDL_Color hatter, SDL_Color szin, TTF_Font *font, SDL_Surface *screen) //kilopva infoc-ről
{
    SDL_Rect forras = { 0, 0, sz, m}, cel = { x, y, sz, m };
    SDL_Surface *lfelirat;
    SDL_Event levent;
    int hossz, kilep, enter;

    hossz = 0;
    dest[hossz] = 0x0000;   /* lezaro 0 */
    SDL_EnableUNICODE(1);
    enter = 0;
    kilep = 0;
    while (!kilep && !enter)
    {
        /* szoveg kirajzolasa */
        boxRGBA(screen, x, y, x+sz-1, y+m-1, hatter.r, hatter.g, hatter.b, 255);
        lfelirat = TTF_RenderText_Blended(font, dest, szin);
        SDL_BlitSurface(lfelirat, &forras, screen, &cel);
        SDL_FreeSurface(lfelirat);
        rectangleRGBA(screen, x, y, x+sz-1, y+m-1, 0, 255, 0, 255);
        /* updaterect: mint az sdl_flip, de csak a kepernyo egy darabjat */
        SDL_UpdateRect(screen, x, y, sz, m);

        SDL_WaitEvent(&levent);
        switch (levent.type)
        {
            case SDL_KEYDOWN:
                switch (levent.key.keysym.unicode)
                {
                    case 0x0000:
                        /* nincs neki megfelelo karakter (pl. shift gomb) */
                        break;
                    case '\r':
                    case '\n':
                        /* enter: bevitel vege */
                        enter = 1;
                        break;
                    case '\b':
                        /* backspace: torles visszafele, ha van karakter */
                        if (hossz>0)
                            dest[--hossz] = 0x0000;
                        break;
                    default:
                        /* karakter: tombbe vele, plusz lezaro nulla */
                        dest[hossz++] = levent.key.keysym.unicode;
                        dest[hossz] = 0x0000;
                        break;
                }
                break;
            case SDL_QUIT:
                /* visszatesszuk a sorba ezt az eventet, mert
                 * sok mindent nem tudunk vele kezdeni */
                SDL_PushEvent(&levent);
                kilep = 1;
                break;
        }
    }

    /* 1 jelzi a helyes beolvasast; = ha enter miatt allt meg a ciklus */
    return enter;
}

void draw(tile** map, int xorigin, int yorigin)
{
    SDL_Color feher = {255, 255, 255}, fekete = { 0, 0, 0 };
    int x=xorigin;
    int y=yorigin;
    SDL_Event event;
    SDL_Event insideevent;
    SDL_TimerID id;
    SDL_Surface *screen;
    TTF_Font *font;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_EnableKeyRepeat(500, 30);
    SDL_WM_SetCaption("Eletjatek szimulacio", "Eletjatek szimulacio");
    screen = SDL_SetVideoMode(924,620,0,SDL_ANYFORMAT);
    if (!screen)
    {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }

    menubutton menukor[620];
    menubutton almenukor[620];
    int i, j;
    int quitprogram=0;
    int quitmenu=0;
    int timerstate=0;
    int tick=200;
    char* filename;

    while (!quitprogram)
    {
        drawmenu(screen,menukor);
        boxRGBA(screen, 303, 0, 923, 619, 0, 0, 0, 255); //clrscr
        for (i=303; i<=923; i+=620/x) //függõleges vonalazás
        {
            lineRGBA(screen, i, 0, i, 618, 255, 255, 255, 255);
        }
        for (i=(620/y)-2; i<=619; i+=620/y) //vízszintes vonalazás
        {
            lineRGBA(screen, 303, i, 923, i, 255, 255, 255, 255);
        }
        for (j=0; j<y; j++)
        {
            for (i=0; i<x; i++)
            {
                if (map[i][j]==cell)
                {
                    boxRGBA(screen, 304+(i*(620/x)), (j)*(620/y)-1, 304+((i+1)*(620/x))-2, ((j+1)*(620/y))-3, 100, 100, 255, 255);
                }
            }
        }

        SDL_Flip(screen);
        SDL_WaitEvent(&event);
        switch (event.type)
        {
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    if (event.button.x>302) //a mapra kattintott
                    {
                        if (map[(event.button.x-304)/(620/x)][(event.button.y)/(620/y)]==empty)
                        {
                            map[(event.button.x-304)/(620/x)][(event.button.y)/(620/y)]=cell;
                        }
                        else
                        {
                            map[(event.button.x-304)/(620/x)][(event.button.y)/(620/y)]=empty;
                        }
                    }
                    else //a menü részre kattintott
                    {
                        if ((event.button.x>=50) && (event.button.x<=253))
                        {
                            switch (menukor[event.button.y])
                            {
                                case meret: //kész
                                    boxRGBA(screen, 0, 0, 302, 619, 0, 0, 0, 255); //clrscr
                                    menubox(screen,44,x10,almenukor,"10x10");
                                    menubox(screen,89,x20,almenukor,"20x20");
                                    menubox(screen,134,x31,almenukor,"31x31");
                                    menubox(screen,179,x62,almenukor,"62x62");
                                    menubox(screen,224,x124,almenukor,"124x124");
                                    menubox(screen,269,x155,almenukor,"155x155");
                                    SDL_Flip(screen);
                                    while (!quitmenu)
                                    {
                                        SDL_WaitEvent(&insideevent);
                                        switch (insideevent.type)
                                        {
                                            case SDL_MOUSEBUTTONDOWN:
                                                if ((insideevent.button.x>=50) && (insideevent.button.x<=253) && (insideevent.button.button==SDL_BUTTON_LEFT))
                                                {
                                                    switch (almenukor[insideevent.button.y])
                                                    {
                                                        case x10:
                                                            free(map[0]);
                                                            free(map);
                                                            createmap(&map,10,10);
                                                            x=10;
                                                            y=10;
                                                            break;
                                                        case x20:
                                                            free(map[0]);
                                                            free(map);
                                                            createmap(&map,20,20);
                                                            x=20;
                                                            y=20;
                                                            break;
                                                        case x31:
                                                            free(map[0]);
                                                            free(map);
                                                            createmap(&map,31,31);
                                                            x=31;
                                                            y=31;
                                                            break;
                                                        case x62:
                                                            free(map[0]);
                                                            free(map);
                                                            createmap(&map,62,62);
                                                            x=62;
                                                            y=62;
                                                            break;
                                                        case x124:
                                                            free(map[0]);
                                                            free(map);
                                                            createmap(&map,124,124);
                                                            x=124;
                                                            y=124;
                                                            break;
                                                        case x155:
                                                            free(map[0]);
                                                            free(map);
                                                            createmap(&map,155,155);
                                                            x=155;
                                                            y=155;
                                                            break;
                                                        default: break;
                                                    }
                                                    quitmenu=1;
                                                }
                                                break;
                                            case SDL_QUIT:
                                                quitmenu=1;
                                                quitprogram=1;
                                                break;
                                        }
                                    }
                                    quitmenu=0;
                                    for (i=0; i<620; i++)
                                    {
                                        almenukor[i]=defaultstate;
                                    }
                                    break;
                                case random: //kész
                                    randomgenerate(map,x,y);
                                    break;
                                case reset: //kész
                                    resetmap(map,x,y);
                                    break;
                                case load:

                                    free(map[0]);
                                    free(map);
                                    loadfile(map,"test.dat",&x,&y);
                                    break;
                                case save:
                                    boxRGBA(screen, 0, 0, 302, 619, 0, 0, 0, 255); //clrscr
                                    SDL_Flip(screen);
                                    TTF_Init();
                                    font = TTF_OpenFont("LiberationSerif-Regular.ttf", 32);
                                    if (!font)
                                    {
                                        fprintf(stderr, "Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
                                        exit(2);
                                    }
                                    input_text(filename,50,44,203,44,fekete,feher,font,screen);
                                    TTF_CloseFont(font);
                                  //  filename="test.dat";
                                    savefile(map,x,y,filename);
                                    break;
                                case manualturn: //kész
                                    turn(map,x,y);
                                    break;
                                case autoturn: //kész
                                    if (!timerstate) //elindít
                                    {
                                        id = SDL_AddTimer(tick, idozit, NULL);
                                        timerstate=1;
                                    }
                                    else //megállít
                                    {
                                        SDL_RemoveTimer(id);
                                        timerstate=0;
                                    }
                                    break;
                                case speed: //kész
                                    boxRGBA(screen, 0, 0, 302, 619, 0, 0, 0, 255); //clrscr
                                    menubox(screen,44,s001,almenukor,"0.01 s");
                                    menubox(screen,89,s005,almenukor,"0.05 s");
                                    menubox(screen,134,s01,almenukor,"0.1 s");
                                    menubox(screen,179,s02,almenukor,"0.2 s");
                                    menubox(screen,224,s05,almenukor,"0.5 s");
                                    menubox(screen,269,s1,almenukor,"1 s");
                                    SDL_Flip(screen);
                                    while (!quitmenu)
                                    {
                                        SDL_WaitEvent(&insideevent);
                                        switch (insideevent.type)
                                        {
                                            case SDL_MOUSEBUTTONDOWN:
                                                if ((insideevent.button.x>=50) && (insideevent.button.x<=253) && (insideevent.button.button==SDL_BUTTON_LEFT))
                                                {
                                                    switch (almenukor[insideevent.button.y])
                                                    {
                                                        case s001:
                                                            tick=10;
                                                            break;
                                                        case s005:
                                                            tick=50;
                                                            break;
                                                        case s01:
                                                            tick=100;
                                                            break;
                                                        case s02:
                                                            tick=200;
                                                            break;
                                                        case s05:
                                                            tick=500;
                                                            break;
                                                        case s1:
                                                            tick=1000;
                                                            break;
                                                        default: break;
                                                    }
                                                    quitmenu=1;
                                                }
                                                break;
                                            case SDL_QUIT:
                                                quitmenu=1;
                                                quitprogram=1;
                                                break;
                                        }
                                    }
                                    quitmenu=0;
                                    for (i=0; i<620; i++)
                                    {
                                        almenukor[i]=defaultstate;
                                    }
                                    break;
                                case quit: //kész
                                    quitprogram=1;
                                    break;
                                default: break;
                            }
                        }
                    }
                }
                break;
            case SDL_USEREVENT:
                turn(map,x,y);
                break;
            case SDL_QUIT:
                quitprogram=1;
                break;
        }
    }

    SDL_Quit();
}
