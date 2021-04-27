#include <SDL.h>
#include <string>
#include <iostream>
#include <time.h>
#include <SDL_ttf.h>
#include <vector>
#include <sstream>
#include <SDL_mixer.h>
using namespace std;

const vector<int>chieucaoong = {40,60,80,100,120,140,160,180,210,230,240,270};

const int w_mh = 860,h_mh = 540;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Texture *background = NULL;
SDL_Texture *chimbaylen = NULL;
SDL_Texture *chimbayngang = NULL;
SDL_Texture *chim_roi = NULL;
SDL_Texture *vatcantren = NULL;
SDL_Texture *vatcanduoi = NULL;

Mix_Music *musicwait = NULL;
Mix_Music *music_birddie = NULL;
Mix_Music *ting = NULL;



SDL_Texture* loadTexture(const string &file)
{
	SDL_Texture *texture = NULL;
	SDL_Surface *loadedImage = SDL_LoadBMP(file.c_str());
    if(file=="img/chimroi.bmp" || file=="img/chimbay.bmp"|| file=="img/chimbayngang.bmp")
    {
        SDL_SetColorKey(loadedImage,SDL_TRUE,SDL_MapRGB( loadedImage->format, 193, 193,204 ));
    }
    if(file == "img/flap.bmp" || file == "img/gameover.bmp" || file == "img/score.bmp")
    {
        SDL_SetColorKey(loadedImage,SDL_TRUE,SDL_MapRGB( loadedImage->format, 0, 0,0 ));
    }
    if(file == "img/play.bmp" || file == "img/quit.bmp" || file == "img/playagain.bmp")
    {
        SDL_SetColorKey(loadedImage,SDL_TRUE,SDL_MapRGB( loadedImage->format, 255, 255,255 ));
    }
    texture = SDL_CreateTextureFromSurface(renderer, loadedImage);
    SDL_FreeSurface(loadedImage);
	return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h)
{

	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
    dst.w = w;
    dst.h = h;
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

struct chim
{
    const int x_chim =  140;
    const int w_chim = 64;
    const int h_chim = 48;
    int y_chim;
    bool baylen;
    void render_chim()
    {
        if(baylen==true)
        {

            renderTexture(chimbaylen,renderer,x_chim,y_chim,w_chim,h_chim);
        }
        else
        {

            renderTexture(chimbayngang,renderer,x_chim,y_chim,w_chim,h_chim);
        }
    }

    void render_chimchet()
        {
            renderTexture(chim_roi,renderer,x_chim,y_chim,w_chim,h_chim);
        }

    void dieukhienchim(SDL_Event &e, bool &play)
    {
         bool dieukhien=false;
         while(SDL_PollEvent(&e))
              {
                  if(e.type == SDL_QUIT)
                        {
                            play = false;
                            break;
                        }
                  else if(e.type==SDL_KEYDOWN)
                       {
                            if (e.key.keysym.sym == SDLK_ESCAPE)
                                 {
                                    play = false;
                                    break;
                                 }
                            if (e.key.keysym.sym == SDLK_UP)
                                {
                                    y_chim = y_chim-30;
                                    baylen = true;
                                    dieukhien = true;
                                    break;
                                }
                        }
              }
              if(!dieukhien)
              {
                    y_chim = y_chim+10;
                    baylen = false;
              }
      }

};

struct vatcan
{
    const int x_ong_bandau = 860;
    const int w_ong = 64;
    const int h_ong = 520;
    int x_ong;
    int chieucao_tren;
    int y_ongtren,y_ongduoi;

    void render_vatcan()
    {
        if(x_ong == x_ong_bandau)
        {
            int a = rand() % 12;
            chieucao_tren = chieucaoong[a];
        }
        y_ongtren = chieucao_tren - 520;
        y_ongduoi = chieucao_tren + 120;
        renderTexture(vatcantren,renderer,x_ong,y_ongtren,w_ong,h_ong);
        renderTexture(vatcanduoi,renderer,x_ong,y_ongduoi,w_ong,h_ong);
    }
    void dichuyenong()
    {
            if(x_ong <= 0)
                x_ong=x_ong_bandau;
            else
                x_ong = x_ong-10;
    }

};

struct diem
{
    int diem_so;
    string diem_chu;
    string diem_tostring(int diem)
    {
        stringstream ss;
        ss<< diem;
        string str = ss.str();
        return str;
    }

    void tangdiem(vatcan vc[],chim bird)
    {
        {
            for(int i=0;i<3;i++)
                {
                    if(vc[i].x_ong+vc[i].w_ong+6 == bird.x_chim)
                    {
                        diem_so ++;
                        Mix_PlayMusic(ting,1);
                        break;
                    }
                }
        }
    }
    void render_diem(int vitri)
    {
        diem_chu = diem_tostring(diem_so);
        TTF_Font* font = TTF_OpenFont("font/vnarial.ttf",60);
        SDL_Color color = { 243, 156, 18 };
        SDL_Surface* surface = TTF_RenderText_Solid(font, diem_chu.c_str(), color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if(vitri == 0)
            renderTexture(texture,renderer,400,0,50,60);
        else if(vitri == 1)
            renderTexture(texture,renderer,550,237,60,72);
    }

};

bool ktra_vacham(vatcan vc,chim bird)
{

    if(vc.x_ong+vc.w_ong >= bird.x_chim && vc.x_ong <= bird.x_chim+bird.w_chim)
    {
        if(bird.y_chim < vc.chieucao_tren || bird.y_chim+bird.h_chim > vc.y_ongduoi)
        {
            return true;
        }
    }
    return false;
}

void menu(bool &play,SDL_Event &e)
{
    SDL_RenderClear(renderer);

    renderTexture(background,renderer,0,0,w_mh,h_mh);

    SDL_Texture* flappy = loadTexture("img/flap.bmp");
    renderTexture(flappy,renderer,100,50,660,198);

    SDL_Texture* start = loadTexture("img/play.bmp");
    renderTexture(start,renderer,320,270,223,85);

    SDL_Texture* quit = loadTexture("img/quit.bmp");
    renderTexture(quit,renderer,320,370,223,85);

    Mix_PlayMusic(musicwait,-1);

    SDL_RenderPresent(renderer);
    while(SDL_WaitEvent(&e))
    {
         if(e.type == SDL_QUIT)
         {
             play = false;
             break;
         }
         else if(e.type == SDL_MOUSEBUTTONUP)
         {
             if(e.button.x >= 320 && e.button.x <= 543 && e.button.y>=270 && e.button.y <= 355)
             {
                 play = true;
                 break;
             }
             else if(e.button.x >= 320 && e.button.x <= 543 && e.button.y >= 370 && e.button.y <= 455)
             {
                 play = false;
                 break;
             }
         }
    }
    Mix_HaltMusic();
}

void render_chimchet(chim bird,vatcan vc[])
{
     while(true)
     {
         SDL_RenderClear(renderer);
         renderTexture(background,renderer,0,0,w_mh,h_mh);
         for(int i = 0; i < 3;i++)
         {
             vc[i].render_vatcan();
         }

         bird.render_chimchet();
         bird.y_chim = bird.y_chim+5;
         SDL_RenderPresent(renderer);
         SDL_Delay(10);
         if( bird.y_chim > 540 )
            return;
     }
}

void gameover(diem score,bool &play,SDL_Event &e)
{

    SDL_Texture* ketthuc = loadTexture("img/gameover.bmp");
    renderTexture(ketthuc,renderer,100,50,660,166);

    SDL_Texture* score_ = loadTexture("img/score.bmp");
    renderTexture(score_,renderer,180,180,430,170);

    score.render_diem(1);

    SDL_Texture* playagain = loadTexture("img/playagain.bmp");
    renderTexture(playagain,renderer,280,320,300,85);

    SDL_Texture* quit = loadTexture("img/quit.bmp");
    renderTexture(quit,renderer,320,415,223,85);

    Mix_PlayMusic(musicwait,-1);

    SDL_RenderPresent(renderer);

    while(SDL_WaitEvent(&e))
    {
        if(e.type == SDL_QUIT)
         {
             play = false;
             break;
         }
         else if(e.type == SDL_MOUSEBUTTONUP)
         {
             if(e.button.x >= 280 && e.button.x <= 580 && e.button.y >= 320 && e.button.y <= 405)
             {
                 play = true;
                 break;
             }
             else if(e.button.x >= 320 && e.button.x <= 543 && e.button.y >= 415 && e.button.y <= 500)
             {
                 play = false;
                 break;
             }
        }

    }
    Mix_HaltMusic();

}


int main( int argc, char* args[] )
{
    srand(time(NULL));


    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );

    window=SDL_CreateWindow("baitaplon_docongdong",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,w_mh,h_mh,SDL_WINDOW_SHOWN);
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    background = loadTexture("img/anhnen.bmp");
    chimbaylen = loadTexture("img/chimbay.bmp");
    chimbayngang = loadTexture("img/chimbayngang.bmp");
    chim_roi = loadTexture("img/chimroi.bmp");
    vatcantren = loadTexture("img/ong_tren.bmp");
    vatcanduoi = loadTexture("img/ong_duoi.bmp");

    musicwait = Mix_LoadMUS("sound/musicwait.mp3");
    music_birddie = Mix_LoadMUS("sound/musicbirddie.mp3");
    ting = Mix_LoadMUS("sound/ting.mp3");

    SDL_Event e;

    bool play = false;
    menu(play,e);

    while(play)
    {
        chim bird;
        bird.y_chim = 0;
        bird.baylen = false;

        vatcan vc[3];
        vc[0].x_ong = vc[0].x_ong_bandau;
        vc[1].x_ong = vc[1].x_ong_bandau;
        vc[2].x_ong = vc[2].x_ong_bandau;

        diem score;
        score.diem_so = 0;

        bool vc1_xhien = false;
        bool vc2_xhien = false;
        bool thua = false;


                while(true)
                {
                    SDL_RenderClear(renderer);
                    renderTexture(background, renderer,0,0,w_mh,h_mh);
                    bird.render_chim();
                    if(vc[0].x_ong == 570)
                    {
                        vc1_xhien=true;
                    }
                    if(vc[0].x_ong == 280)
                    {
                        vc2_xhien = true;
                    }
                    if(vc1_xhien == false)
                    {
                        vc[0].render_vatcan();
                    }
                    else if(vc1_xhien == true && vc2_xhien == false)
                    {
                         for(int i=0;i<2;i++)
                        {
                            vc[i].render_vatcan();
                        }
                    }
                    else if(vc2_xhien == true)
                    {
                        for(int i=0;i<3;i++)
                        {
                            vc[i].render_vatcan();
                        }
                    }
                    score.render_diem(0);
                    SDL_RenderPresent(renderer);
                    for(int i=0;i<3;i++)
                    {
                        if(ktra_vacham(vc[i],bird) == true)
                            {
                             Mix_PlayMusic(music_birddie,1);
                             thua = true;
                             break;
                            }
                    }
                    if(play == false || thua == true)
                      {
                          break;
                      }
                    score.tangdiem(vc,bird);
                    bird.dieukhienchim(e,play);
                    if(vc1_xhien == false)
                    {
                        vc[0].dichuyenong();
                    }
                    else if(vc1_xhien == true && vc2_xhien == false)
                    {
                        for(int i=0;i<2;i++)
                        {
                            vc[i].dichuyenong();
                        }
                    }
                    else
                    {
                        for(int i=0;i<3;i++)
                        {
                            vc[i].dichuyenong();
                        }
                    }
                    SDL_Delay(47);
                }

        if(thua)
            {
                render_chimchet(bird,vc);
                gameover(score,play,e);
            }
    }
    Mix_CloseAudio();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

