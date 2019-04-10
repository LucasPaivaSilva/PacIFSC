using namespace std;
#include <iostream>
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "objects.h"

//VARIÁVEIS GLOBAIS//=========================//
const int WIDTH = 1024;
const int HEIGHT = 768;
enum KEYS{UP, DOWN, LEFT, RIGHT, SPACE};
bool keys[5] = {false, false, false, false, false};
const int FPS = 60;


//PROTÓTIPOS DE FUNÇÃO//====================//
void InitPlayer(PlayerData &player1);
void DrawPlayer(PlayerData &player1);
void MovePlayerLeft(PlayerData &player1);
void MovePlayerUp(PlayerData &player1);
void MovePlayerDown(PlayerData &player1);
void MovePlayerRight(PlayerData &player1);
void DrawGhost(GhostData &GhostTest, PlayerData &player1);
void InitGhost(GhostData &GhostTest);

//MAIN VOID//===============================//

int main(void)
{
	//VARIÁVEIS PRIMITIVAS
	bool done = false;
	bool redraw = true;

	//VARIÁVEIS DE OBJETO(PLAYER+FANTASMA)
	PlayerData player1;
	GhostData GhostTest;

	//Allegro variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_SAMPLE *sample_beginning=NULL;
	ALLEGRO_SAMPLE *sample_chomp=NULL;

	//Initialization Functions
	if(!al_init())
		return -1;

        if(!al_install_audio()){
      return -1;
   }

      if(!al_init_acodec_addon()){
      return -1;
   }

   if (!al_reserve_samples(1)){
      return -1;
   }

    sample_beginning = al_load_sample( "music/pacman_beginning.wav" );
    sample_chomp = al_load_sample( "music/pacman_chomp.wav" );


	display = al_create_display(WIDTH, HEIGHT);

	if(!display)
		return -1;


	//Allegro Module Init
	al_init_primitives_addon();
	al_install_keyboard();

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	//Game Init
 	InitPlayer(player1);
 	InitGhost(GhostTest);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_start_timer(timer);
	al_play_sample(sample_beginning , 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

	while(!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}
		else if(ev.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = true;
			if(keys[UP])
				MovePlayerUp(player1);
			if(keys[DOWN])
				MovePlayerDown(player1);
			if(keys[LEFT])
				MovePlayerLeft(player1);
			if(keys[RIGHT])
				MovePlayerRight(player1);
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;
				case ALLEGRO_KEY_UP:
					keys[UP] = true;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = true;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = true;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = true;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = true;
					break;

			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_UP:
					keys[UP] = false;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = false;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = false;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = false;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = false;
					break;

			}
		}


		if(redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;

			DrawPlayer(player1);

			al_flip_display();
			al_clear_to_color(al_map_rgb(0,0,0));
		}
	}

	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_sample(sample_chomp);

	return 0;
}
//FUNCOES DO FANTASMA (GHOST)

void InitGhost(GhostData &GhostTest)
{
	GhostTest.x = 200;
	GhostTest.y = 200;
	GhostTest.alive = true;
	GhostTest.vulnerable = false;
	GhostTest.speed = 5;
	GhostTest.boundx = 6;
	GhostTest.boundy = 7;
}

void DrawGhost(GhostData &GhostTest, PlayerData &player1)
{
    if(player1.y>GhostTest.y){
        GhostTest.y = GhostTest.y + GhostTest.speed;
    }
    else if(player1.y<GhostTest.y){
        GhostTest.y = GhostTest.y - GhostTest.speed;
    }
    else{
        GhostTest.y = GhostTest.y;
    }
    if(player1.x>GhostTest.x){
        GhostTest.x = GhostTest.x + GhostTest.speed;
    }
    else if(player1.x<GhostTest.x){
        GhostTest.x = GhostTest.x - GhostTest.speed;
    }
    else{
        GhostTest.x = GhostTest.x;
    }
    al_draw_filled_circle(GhostTest.x, GhostTest.y, 10, al_map_rgb(239, 26, 26));

}

//FUNÇOES DO JOGADOR(PACMAN)

void InitPlayer(PlayerData &player1)
{
	player1.x = 20;
	player1.y = HEIGHT / 2;
	player1.ID = PLAYER;
	player1.lives = 3;
	player1.speed = 7;
	player1.boundx = 6;
	player1.boundy = 7;
	player1.score = 0;
}
void DrawPlayer(PlayerData &player1)
{
    al_draw_filled_circle(player1.x, player1.y, 10, al_map_rgb(242, 234, 24));
}
void MovePlayerLeft(PlayerData &player1)
{
	player1.x -= player1.speed;
	if(player1.x < 10)
		player1.x = 10;

}
void MovePlayerUp(PlayerData &player1)
{
	player1.y -= player1.speed;
	if(player1.y < 10)
		player1.y = 10;
}
void MovePlayerDown(PlayerData &player1)
{
	player1.y += player1.speed;
	if(player1.y > HEIGHT-10)
		player1.y = HEIGHT-10;

}
void MovePlayerRight(PlayerData &player1)
{
	player1.x += player1.speed;
	if(player1.x > WIDTH-10)
		player1.x = WIDTH-10;
}
