#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* surface;
SDL_Texture* texture;

SDL_Rect lpad;
SDL_Rect rpad;

TTF_Font* font;
char score[] = "0 0";
SDL_Color color;
SDL_Rect textrect;

int width = 800;
int height = 600;


struct Ball{
    int x;
    int y;
    int radius;
    int xmov;
    int ymov;
} ball;

bool right = true;

int lscore, rscore = 0;

void ballreset(){
    right = !right;
    
    ball.x = width/2;
    ball.y = height/2;
    ball.radius = 10;
    ball.xmov = -5;
    if(right) ball.xmov = 5;
    ball.ymov = 0;
}

void setobjects(){
    // paddles
    lpad = (SDL_Rect){.x = 80, .y = 600/2 - 50, .w = 20, .h = 100};
    rpad = (SDL_Rect){.x = 700 + 20, .y = 600/2 - 50, .w = 20, .h = 100};

    ballreset();

    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;

    textrect = (SDL_Rect){.x = 350, .y = 20, .w = 100, .h = 50};
}

void drawball(){
    int cx = ball.x;
    int cy = ball.y;
    int r = ball.radius;
    
    int x1, x2, y1, y2;
    x1 = cx - r;
    x2 = cx + r;
    y1 = cy - r;
    y2 = cy + r;

    for(int y = y1; y < y2; y++){
        for(int x = x1; x < x2; x++){
            int dx = x - cx;
            int dy = y - cy;
            
            if(dx * dx + dy * dy <= r*r){
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}

void draw(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &lpad);
    SDL_RenderFillRect(renderer, &rpad);

    drawball();

    char* scorep = &score[0];
    surface = TTF_RenderText_Solid(font, scorep, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &textrect);
    SDL_DestroyTexture(texture);
        
    SDL_RenderPresent(renderer);                                     
}

void updatescore(int lol){
    if(lol){
        score[2] = 48 + rscore;
    } else{
        score[0] = 48 + lscore;
    }
}

void ballmovement()
{
    // check for collision

    // lpad
    if(ball.x <= lpad.x + 20){
        if(ball.y >= lpad.y && ball.y <= lpad.y + 100){
            int ballbouncey = ball.y - lpad.y;
            if(ballbouncey <= 50){
                ball.ymov = -2;
            } else{
                ball.ymov = 2;
            }
            ball.xmov = 5;
        }
    }

    // rpad
    if(ball.x >= rpad.x){
        if(ball.y >= rpad.y && ball.y <= rpad.y + 100){
            int ballbouncey = ball.y - rpad.y;
            if(ballbouncey <= 50){
                ball.ymov = -2;
            } else{
                ball.ymov = 2;
            }
            ball.xmov = -5;
        }
    }

    // walls
    if(ball.y >= height || ball.y <= 0) ball.ymov = ball.ymov * -1;
    
    if(ball.x <= 0){
        rscore++;
        updatescore(1);
        ballreset();
    }
    if(ball.x >= width){
        lscore++;
        updatescore(0);
        ballreset();
    }

    
    // apply movement

    ball.x += ball.xmov;
    ball.y += ball.ymov;
}

void handlemovement(){
    // paddle movement
    const int movspeed = 5;
        
    const uint8_t* state = SDL_GetKeyboardState(NULL);
        
    if(state[SDL_SCANCODE_W]) {
        if(lpad.y - movspeed >= 0) lpad.y -= movspeed;
    }
    
    if(state[SDL_SCANCODE_S]) {
        if(lpad.y + movspeed <= 500) lpad.y += movspeed;
    }
    
    if(state[SDL_SCANCODE_UP]) {
        if(rpad.y - movspeed >= 0) rpad.y -= movspeed;
    }
    if(state[SDL_SCANCODE_DOWN]) {
        if(rpad.y + movspeed <= 500) rpad.y += movspeed;
    }

    ballmovement();
}

int main(){
    
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    
    setobjects();

    TTF_Init();
    font = TTF_OpenFont("./font/inconsolata.ttf", 1000);

    int framedelay = 1000 / 60;
    int run = 1;
    while(run){
        unsigned int initialtick = SDL_GetTicks();
        
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) run = 0;
        }
        
        handlemovement();
        draw();
        
        int frametime = SDL_GetTicks() - initialtick;
        if (frametime < framedelay) SDL_Delay(framedelay - frametime);
    }

    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
