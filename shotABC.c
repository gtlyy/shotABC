/*
This program is free software.
shot.c Copyright © 2021 By Lizhidi.
*/


char *version = "shotABC-2.1";

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <sys/time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#define	SCREEN_WIDTH  	640
#define	 SCREEN_HEIGHT 	480
#define 	FONTFILE			"FreeSans.ttf"
#define 	FONTSIZE			50
#define	NUM				5

TTF_Font 	*font = NULL;
SDL_Color color = { 255, 255, 255 };
char	letters[NUM];
SDL_Point	 pos[NUM];

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *background = NULL;
SDL_Texture *ls = NULL;
SDL_Texture *bao = NULL;

SDL_Surface	*icon=NULL;

Mix_Chunk	*Sbao = NULL;
Mix_Chunk	*Swrong = NULL;
Mix_Chunk	*Sother = NULL;
Mix_Chunk	*letter_sound = NULL;


void err_exit(char const *err)
{
	fprintf(stderr, "%s # %s\n", err, SDL_GetError() );
	exit(1);
}

/* 加载图片 */
SDL_Texture*  Load_Texture (char *filename, SDL_Renderer *ren)
{
	SDL_Texture *texture = IMG_LoadTexture (ren, filename);
	if (texture == NULL)		
		err_exit ("(EE) in LoadTexture()");
	return texture;
}

/* message to Texture 的转换 */
SDL_Texture* Render_Text (char *message, SDL_Renderer *ren)
{
	SDL_Surface *surf = TTF_RenderText_Blended (font, message, color); 
	
	if (surf == NULL)		
		err_exit ("(EE) in RenderText() 1 ");
	SDL_Texture *texture = SDL_CreateTextureFromSurface (ren, surf);
		if ( texture == NULL)		
		err_exit ("(EE) in RenderText() 2 ");
	SDL_FreeSurface(surf);
	return texture;
}

/* 随机设定字母 */
void Random_Letters()
{
	int i = 0 ;
	struct timeval		tv;
	for ( ; i<NUM; ++i) {	
		gettimeofday(&tv, NULL);			
		unsigned int	seed = (unsigned int)(tv.tv_usec + i);
		srand(seed);
		letters[i] = 'A' + rand() % 26 ;
		pos[i].x = rand() % (SCREEN_WIDTH - FONTSIZE);
		pos[i].y = rand() % (SCREEN_HEIGHT- FONTSIZE);
	}
}

/* 打印字母靶子 */
void Show_Letters() 
{
	int i = 0; 
        char c[]="a";
	SDL_Rect	rect;
	for ( ; i<NUM ; ++i) {
		c[0] = letters[i];
		c[1] = '\0';
		rect.x = pos[i].x;
		rect.y = pos[i].y;
		ls = Render_Text (c, renderer);
		SDL_QueryTexture(ls, NULL, NULL, &rect.w, &rect.h);
		SDL_RenderCopy (renderer, ls, NULL, &rect);
                //SDL_Delay(200);
	}
        SDL_RenderPresent(renderer);
}

/* 按对了，爆炸一下 */
void Show_Bao(SDL_Point p)
{
        //printf("In Show_Bao()\n");
	SDL_Rect	rect;
	SDL_QueryTexture(bao, NULL, NULL, &rect.w, &rect.h);
	rect.x = p.x ;
	rect.y = p.y ;
	SDL_SetTextureBlendMode (bao, SDL_TEXTUREMODULATE_NONE);  /* NO 混合 */
	SDL_RenderCopy (renderer, bao, NULL, &rect);
	SDL_RenderPresent(renderer);
}

/* 重刷屏幕，主要是重刷字体位置 */
void Clear_Screen()
{
        //int i=0;
        //SDL_Rect	rect;

        //SDL_SetTextureBlendMode (background, SDL_TEXTUREMODULATE_NONE);  /* NO 混合 */
/*
	for ( ; i<NUM; ++i) {
		rect.x = pos[i].x;
		rect.y = pos[i].y;
		rect.w = FONTSIZE * 2;
		rect.h = FONTSIZE * 2 ;
		SDL_RenderCopy (renderer, background, &rect, &rect);
                SDL_Delay(500);
	}
*/
        SDL_RenderCopy (renderer, background, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void Play_Letter(char c)
{
    //printf("In Play_Letter():%c\n", c);
    char sound_name[6] = "a.wav";
    sound_name[0]=tolower(c);
    //sound_name[5]='\0';
    if ( (letter_sound = Mix_LoadWAV (sound_name)) == NULL )
            fprintf (stderr, "(WW) in Play_Letter():%s\n", Mix_GetError() );

    Mix_PlayChannel (-1, letter_sound, 0);
}

/* 清场 */
void 	Close()
{	
	Mix_FreeChunk(Sbao);
	Mix_FreeChunk(Swrong);
	Mix_FreeChunk(Sother);
        Mix_FreeChunk(letter_sound);
	Mix_CloseAudio();
	Sbao = Swrong = Sother = NULL;
	
	TTF_CloseFont(font);
	TTF_Quit();
	font = NULL;
	
	SDL_FreeSurface(icon);
	SDL_DestroyTexture (ls);
	SDL_DestroyTexture (bao);
	SDL_DestroyTexture (background);
	icon = NULL;
	ls = bao = background = NULL;
	
	SDL_DestroyRenderer (renderer);
	SDL_DestroyWindow (window);
	renderer = NULL;
	window = NULL;
	
	SDL_Quit();
}

/* 打印版本号 */
void 	Print_Version()
{
	SDL_Rect		rect;
        rect.w = FONTSIZE * strlen(version) ;
	rect.h = FONTSIZE;
	rect.x = (SCREEN_WIDTH - rect.w) / 2;
	rect.y = (SCREEN_HEIGHT - rect.h) * 2 / 3 ;
        SDL_Texture *ver =  Render_Text (version, renderer);
	
	SDL_RenderClear (renderer);
	SDL_RenderCopy (renderer, ver, NULL, &rect);
	SDL_RenderPresent (renderer);
}

/* 初始化：窗口，渲染器，字体，图片，声效等 */
void	Init()
{	
	if ( SDL_Init(SDL_INIT_EVERYTHING) == -1 )
		err_exit ("(EE) SDL_Init()");
	if (TTF_Init() == -1)
		err_exit ("(EE) TTF_Init()");
	 
	/* Setup: window,renderer,icon */
	window = SDL_CreateWindow ("Best: ?", \
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, \
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) 
		err_exit ("(EE) SDL_CreateWindow()");
	if ( (icon = SDL_LoadBMP ("right.bmp")) == NULL)
		err_exit ("(EE) SDL_LoadBMP()");
	SDL_SetWindowIcon(window, icon);
	

	renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		printf("SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC is not ok.\n");
		renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_SOFTWARE); 
		if (renderer == NULL) {
			err_exit ("(EE) SDL_CreateRenderer");
		}
	}

/*
	renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_SOFTWARE); 
	if (renderer == NULL) {
		err_exit ("(EE) SDL_CreateRenderer");
	}
*/
	
    SDL_SetRenderDrawBlendMode (renderer, SDL_TEXTUREMODULATE_NONE);  /* NO 混合 */
	
	/* 加载图片 */
	background = Load_Texture ("background.bmp", renderer);
	bao = Load_Texture ("right.bmp", renderer);
	if ( (background == NULL)  || (bao == NULL) ) 
		err_exit("(EE) load_image()");
        SDL_SetTextureBlendMode (bao, SDL_TEXTUREMODULATE_NONE);  /* NO 混合 */
        SDL_SetTextureBlendMode (background, SDL_TEXTUREMODULATE_NONE);  /* NO 混合 */
		
	/* 加载字体  */
	font =  TTF_OpenFont (FONTFILE, FONTSIZE);
	if (font == NULL)
		err_exit ("(EE) TTF_OpenFont()");
		
	 /* 加载音乐 */
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) 
		fprintf( stderr, "(WW) Mix_OpenAudio():%s\n", Mix_GetError() );
	if ( (Sbao = Mix_LoadWAV ( "./Sbao.wav" )) == NULL ) 
		fprintf (stderr, "(WW) in Mix_LoadWAV():%s\n", Mix_GetError() );		
	if ( (Swrong = Mix_LoadWAV ( "./Swrong.wav" )) == NULL ) 
		fprintf (stderr, "(WW) in Mix_LoadWAV():%s\n", Mix_GetError() );				
	if ( (Sother = Mix_LoadWAV ( "./Sother.wav" )) == NULL ) 
		fprintf (stderr, "(WW) in Mix_LoadWAV():%s\n", Mix_GetError() );		
}

int main(int argc, char *argv[])
{
	int i=0, quit=0, key=0, press=0, show=0, right=0;
	unsigned int	start_ticks=0, best=0, t=0; 		/* Uint32  */
	char	title[50];
	SDL_Event	event;
	
	/* 初始化 */
	Init();

	/* 打印版本号 */
	Print_Version();
	SDL_Delay (3000);
	
	/* 清屏、铺背景 */ 
	SDL_RenderClear (renderer);
	SDL_RenderCopy (renderer, background, NULL, NULL);
	SDL_RenderPresent(renderer);
	
	/*---------------一切就绪,准备进入主循环----------------*/
	show=1;
	SDL_Delay(1000);
	while( ! quit ) {
		SDL_Delay(200);		
		if (show) {
                        while ( SDL_PollEvent( &event ) )  {
                            /* 清理开场前的按键事件 */
				;
                        }
                        //开始：
			Random_Letters();
			Show_Letters();
			start_ticks = SDL_GetTicks();  /* 计时开始 */
			show=0;
		}
		/* 捕捉事件 */
		while ( SDL_PollEvent( &event ) ) {
                        if ( event.type == SDL_QUIT )  {
				quit = 1;
                                continue;
                        } else if ( event.type == SDL_KEYDOWN ) {
					key = event.key.keysym.sym ; 
                                        //Bug fix:
                                        //printf("key=%c\n", key);

					if ( key>=SDLK_a  && key <=SDLK_z ) {
						++press;
						for(i=0; i<NUM; ++i) 
							if( toupper(key) == letters[i] ) {
								++right;
                                                                //printf("To play:%c\n", letters[i]);
                                                                Play_Letter(letters[i]);
                                                                //printf("To bao:%c\n", letters[i]);
                                                                Show_Bao ( pos[i]);
                                                                //你无法想象，没有下面这一句，Linux版本运行出错！！！
                                                                //你无法想象，就下面这一句，把花屏的Bug都fix了！！！
                                                                // SDL_Delay(200);
								letters[i] = '\0';
								break;
							}
						if (i==NUM)   /* 是字母，但按不对 */
							Mix_PlayChannel (-1, Swrong, 0);
					}else   
						Mix_PlayChannel (-1, Sother, 0);
                        } else if ( event.type == SDL_KEYUP) {
                                        continue;
                        } else if ( event.type == SDL_TEXTINPUT) {
                                        continue;
                        } else {
                                        //按下一个键，应该就是上面这些事件，没有其他了。
                                        continue;
                        }

			/* 按了NUM个字母了 */
			if (press == NUM)  {
				if (right==NUM) {
					t= SDL_GetTicks() - start_ticks;
					if (t<best || best==0) 
						best = t;
					sprintf(title, "Best:%u",  best);
					SDL_SetWindowTitle (window, title) ;
				}
				press = 0;
				show = 1 ;
				right=0;
                                SDL_Delay(200);
				Clear_Screen();
				break;
			}
		} /* 捕捉事件结束 */
	}
	
	//清场
	Close();
	
	return 0;
}
