// gcc main.c -IC:/SDL3/include -IC:/SDL3_ttf/include -LC:/SDL3/lib -LC:/SDL3_ttf/lib -lSDL3 -lSDL3_ttf -o juego.exe
#include <SDL3/SDL.h>
#include <stdio.h>
#include <SDL3_ttf/SDL_ttf.h>

#define WINDOW_WIDTH 800 
#define WINDOW_HEIGHT 600
#define PLAYER_SIZE 50 
#define SPEED 5
#define BULLET_SIZE 10
#define BULLET_SPEED 30


int main(void)
{
    printf("Antes SDL_Init\n");

    if (SDL_Init(SDL_INIT_VIDEO) != true) {
        printf("SDL_Init FALLÓ: %s\n", SDL_GetError());
        return false;
    }

    printf("SDL_Init OK\n");

    if (!TTF_Init())
    {
    printf("TTF_Init falló: %s\n", SDL_GetError());
    return 1;
    }
    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 32);

    if (!font)
    {
        printf("No se pudo cargar la fuente: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "TEST",
        800,
        600,
        0
    );

    if (!window) {
        printf("Window error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        printf("Renderer error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_FRect player = { 
    WINDOW_WIDTH / 2.0f, 
    WINDOW_HEIGHT / 2.0f, 
    (float)PLAYER_SIZE, 
    (float)PLAYER_SIZE };
    


bool running = true;
bool paused = false;
int opcionMenu = 0;

const int TOTAL_OPCIONES = 2;

SDL_Event event; 

#define MAX_BULLETS 100

typedef struct
{
    SDL_FRect rect;
    bool active;
} Bullet;

Bullet bullets[MAX_BULLETS] = {0};    

int fireCooldown = 0;


while (running) { 

// Eventos 
while (SDL_PollEvent(&event)) 
{ if (event.type == SDL_EVENT_QUIT) 
{ running = false; 
} 
   if (event.type == SDL_EVENT_KEY_DOWN &&
    event.key.scancode == SDL_SCANCODE_Z)
    {
    
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            bullets[i].active = true;



            bullets[i].rect.x = player.x + PLAYER_SIZE/2 - BULLET_SIZE/2;
            bullets[i].rect.y = player.y;

            bullets[i].rect.w = BULLET_SIZE;
            bullets[i].rect.h = BULLET_SIZE;

            break;
            }
        }
    }
    if (event.type == SDL_EVENT_KEY_DOWN)
{
    printf("Tecla: %d\n", event.key.scancode);
}
    if (event.type == SDL_EVENT_KEY_DOWN &&
    event.key.scancode == SDL_SCANCODE_ESCAPE)
{
    paused = !paused;    // Cambia entre pausa y juego
}
if (paused && event.type == SDL_EVENT_KEY_DOWN)
{
    switch (event.key.scancode)
    {
        case SDL_SCANCODE_UP:

            if (opcionMenu > 0)
                opcionMenu--;

            break;

        case SDL_SCANCODE_DOWN:

            if (opcionMenu < TOTAL_OPCIONES - 1)
                opcionMenu++;

            break;

        case SDL_SCANCODE_Z:

            if (opcionMenu == 0)
            {
                paused = false;
            }
            else if (opcionMenu == 1)
            {
                running = false;
            }

            break;
    }
}
} 
// Input continuo 

//mov personaje

const bool *keys = SDL_GetKeyboardState(NULL);

if (!paused)
{
float velocidad = SPEED;

if (keys[SDL_SCANCODE_LCTRL])
    velocidad *= 2.0f;

if (keys[SDL_SCANCODE_LSHIFT])
    velocidad *= 0.5f;

if (keys[SDL_SCANCODE_UP])
    player.y -= velocidad;

if (keys[SDL_SCANCODE_DOWN])
    player.y += velocidad;

if (keys[SDL_SCANCODE_LEFT])
    player.x -= velocidad;

if (keys[SDL_SCANCODE_RIGHT])
    player.x += velocidad;

// balas


if (fireCooldown > 0)
    fireCooldown--;

for (int i = 0; i < MAX_BULLETS; i++)
{
    if (bullets[i].active)
    {
        bullets[i].rect.y -= BULLET_SPEED;

        if (bullets[i].rect.y < -BULLET_SIZE)
        {
            bullets[i].active = false;
        }
    }
}

if (keys[SDL_SCANCODE_Z] && fireCooldown == 0)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            bullets[i].active = true;

            bullets[i].rect.x = player.x + PLAYER_SIZE/2 - BULLET_SIZE/2;
            bullets[i].rect.y = player.y;

            bullets[i].rect.w = BULLET_SIZE;
            bullets[i].rect.h = BULLET_SIZE;

            fireCooldown = 10; // velocidad de disparo
            break;
        }
    }
}
}
// cerrar rapido + menu
const char *opciones[] =
{
    "Continuar",
    "Salir"
};

const int numOpciones = 2;




// Limitar pantalla 
if (player.x < 0) player.x = 0; 
if (player.y < 0) player.y = 0; 
if (player.x > WINDOW_WIDTH - PLAYER_SIZE) 
player.x = WINDOW_WIDTH - PLAYER_SIZE; 
if (player.y > WINDOW_HEIGHT - PLAYER_SIZE) 
player.y = WINDOW_HEIGHT - PLAYER_SIZE; 
// Render 
SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
SDL_RenderClear(renderer);

SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
SDL_RenderFillRect(renderer, &player);

SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

for (int i = 0; i < MAX_BULLETS; i++)
{
    if (bullets[i].active)
        SDL_RenderFillRect(renderer, &bullets[i].rect);
}

if (paused)
{
    SDL_FRect fondo = {180,120,440,300};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_RenderFillRect(renderer, &fondo);

    //resaltar colores menu
    // colores menu
    SDL_Color blanco = {255, 255, 255, 255};

for (int i = 0; i < numOpciones; i++)
{
    SDL_Surface *surface =
        TTF_RenderText_Blended(font, opciones[i], 0, blanco);

    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FRect dst = {
        (WINDOW_WIDTH - surface->w) / 2.0f,
        220 + i * 60,
        (float)surface->w,
        (float)surface->h
    };

    SDL_RenderTexture(renderer, texture, NULL, &dst);

    /* Subrayado azul */
    if (i == opcionMenu)
    {
        SDL_SetRenderDrawColor(renderer, 0, 120, 255, 255);

        SDL_FRect linea = {
            dst.x,
            dst.y + dst.h + 4,
            dst.w,
            3
        };

        SDL_RenderFillRect(renderer, &linea);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}
}



SDL_RenderPresent(renderer);

SDL_Delay(16); // ~60 FPS 
}
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Delay(2000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}