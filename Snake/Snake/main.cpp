#include <stdio.h>
#include <stdio.h>
#include <conio.h>
#include <allegro5\allegro5.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_native_dialog.h>

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_FONT *font = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_EVENT ev;
FILE *f;

#define TEXT_SIZE 17
#define ROZMIAR_KLOCKA 13
char direction = 4;
char **playground;
struct snake_chain *snake_head;
struct snake_chain *snake_ass;
char borders_exist = 0;
int map_size = 20;

struct snake_chain
{
	char x;
	char y;
	struct snake_chain *previous;
	struct snake_chain *next;
};

int initialize_allegro()
{	
	if(!al_init()) 
	{
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize allegro!", 
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	display = al_create_display(600, 300);
	if(!display) 
	{
		al_show_native_message_box(display, "Error", "Error", "failed to create display!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	event_queue = al_create_event_queue();
	if(!event_queue) 
	{
		al_show_native_message_box(display, "Error", "Error", "failed to create event_queue!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		return -1;
	}
	al_flush_event_queue(event_queue);

	al_init_font_addon();
	al_init_ttf_addon();

	font = al_load_ttf_font("C:\\Windows\\Fonts\\cour.ttf", TEXT_SIZE, 0);
	if(!font)
	{
		al_destroy_event_queue(event_queue);
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize font!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		return -1;
	}

	timer = al_create_timer(1.0 / 1);
	if(!timer) 
	{
		al_destroy_display(display);
		al_destroy_event_queue(event_queue);
		al_destroy_font(font);
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize timer!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	al_install_keyboard();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	return 1;
}

int create_map()
{
	fprintf(f, "createmap()\n");
#define START_LEN 3 //+1
	direction = 4;
	snake_head = (struct snake_chain*)malloc(sizeof(struct snake_chain));
	snake_head->previous = NULL;
	snake_head->x = map_size / 2 - 3;
	snake_head->y = map_size / 2;
	snake_ass = snake_head;
	playground[snake_head->y][snake_head->x] = 1;
	fprintf(f, "1. x=%d y=%d \n", snake_head->x, snake_head->y);
	for(int i=0; i<START_LEN; i++)
	{
		snake_head->next = (struct snake_chain*)malloc(sizeof(struct snake_chain));
		snake_head->next->previous = snake_head;
		snake_head = snake_head->next;
		snake_head->x = map_size / 2 - 2 + i;
		snake_head->y = map_size / 2;
		playground[snake_head->y][snake_head->x] = 1;
		fprintf(f, "%d. x=%d y=%d \n", i, snake_head->x, snake_head->y);
	}
	snake_head->next = NULL;

	int x, y; //generating food
	do {		// generate new food where snake is not lying
		do { x = rand() % map_size; } while(x == map_size /2);
			 y = rand() % map_size;
	} while(playground[y][x] > 0 || x==0 || x==map_size-1 || y==0 || y==map_size-1);
	playground[y][x] = 2; // food here
	fprintf(f, "food: x=%d y=%d\n\n", x, y);
	return 1;
}


int change_snake_position()
{
	fprintf(f, "change()\n");
	//moving head
	snake_head->next = (struct snake_chain*)malloc(sizeof(struct snake_chain));
	snake_head->next->previous = snake_head;
	snake_head->next->next = NULL;
	snake_head->next->x = snake_head->x;
	snake_head->next->y = snake_head->y;
	snake_head = snake_head->next;
	if(!borders_exist)
		switch(direction)
		{
		case 1: //up
			snake_head->y = (snake_head->y == 0 ? map_size-1 : snake_head->y-1); break;
		case 2: //left
			snake_head->x = (snake_head->x == 0 ? map_size-1 : snake_head->x-1); break;
		case 3: //down
			snake_head->y = (snake_head->y == map_size-1 ? 0 : snake_head->y+1); break;
		case 4: //right
			snake_head->x = (snake_head->x == map_size-1 ? 0 : snake_head->x+1); break;
		//default: 
		}
	fprintf(f, "new head: x=%d y=%d\n", snake_head->x, snake_head->y);

	if(playground[snake_head->y][snake_head->x] == 1) //snake meets itself
		return 0; //game over
	else if(playground[snake_head->y][snake_head->x] == 2) //food eaten
	{
		playground[snake_head->y][snake_head->x] = 1;
		int x, y;
		do {		// generate new food where snake is not now
			x = rand() % map_size;
			y = rand() % map_size;
		} while(playground[y][x] > 0 || x==0 || x==map_size-1 || y==0 || y==map_size-1);
		playground[y][x] = 2;
		fprintf(f, "new food: x=%d y=%d\n", x, y);
	}
	playground[snake_head->y][snake_head->x] = 1; //snake here

	//removing ass
	playground[snake_ass->y][snake_ass->x] = 0; //snake not here
	fprintf(f, "assR: x=%d y=%d\nn", snake_ass->x, snake_ass->y);
	snake_ass = snake_ass->next;
	free(snake_ass->previous);
	snake_ass->previous = NULL;
	return 1; //game continues
}

void draw_game()
{
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_rectangle(19, 19, 19+2+map_size*ROZMIAR_KLOCKA, 19+2+map_size*ROZMIAR_KLOCKA, al_map_rgb(255, 255, 255), 1);
		
	for(int j=0; j<map_size; j++)
	{
		for(int k=0; k<map_size; k++)
		{
			if(playground[j][k] == 1) //snake
				al_draw_filled_rectangle(20+k*ROZMIAR_KLOCKA, 20+j*ROZMIAR_KLOCKA, 20+k*ROZMIAR_KLOCKA+ROZMIAR_KLOCKA-1, 20+j*ROZMIAR_KLOCKA+ROZMIAR_KLOCKA-1, al_map_rgb(255, 255, 255));
			else if(playground[j][k] == 2) //food
				al_draw_filled_circle(20+k*ROZMIAR_KLOCKA+ROZMIAR_KLOCKA/2.0, 20+j*ROZMIAR_KLOCKA+ROZMIAR_KLOCKA/2.0, ROZMIAR_KLOCKA/3.0, al_map_rgb(255, 255, 50));
			else // nothing
				al_draw_filled_rectangle(20+k*ROZMIAR_KLOCKA, 20+j*ROZMIAR_KLOCKA, 20+k*ROZMIAR_KLOCKA+ROZMIAR_KLOCKA-1, 20+j*ROZMIAR_KLOCKA+ROZMIAR_KLOCKA-1, al_map_rgb(30, 30, 30));
		} 
	}
	al_flip_display();
}

int main()
{
	f = fopen("debug.txt", "w");
	if(initialize_allegro() == -1)
		return 0;

	//creating gamemap dyn. array
	playground = (char**)calloc(map_size+1, sizeof(char));
	for(int i=0; i<map_size; i++)
	{
		playground[i] = (char*)calloc(1, sizeof(char));
	}
 
	al_init_primitives_addon();
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();
	srand(time(NULL));

	create_map();
	al_start_timer(timer);
	while(1)
	{
		draw_game();
		al_flush_event_queue(event_queue);
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
			return 0;  //exit program
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			
			if(ev.keyboard.keycode != ALLEGRO_KEY_SPACE)
				switch(ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_DOWN:
					if(direction != 1)
					direction = 3; break;
				case ALLEGRO_KEY_UP:
					if(direction != 3)
					direction = 1; break;
				case ALLEGRO_KEY_LEFT:
					if(direction != 4)
					direction = 2; break;
				case ALLEGRO_KEY_RIGHT:
					if(direction != 2)
					direction = 4; break;
				}
		}
			//al_flip_display();
		if(ev.type = ALLEGRO_EVENT_TIMER)
		{
			if(!change_snake_position())
				break;
		}
		fprintf(f, "dir: %d\n", direction);
	}
	//game over
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_text(font, al_map_rgb(255, 255, 255), 50, 50, 0, "GAME OVER");
	while(!(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE))))
		al_wait_for_event(event_queue, &ev);
	fclose(f);
	return 0;
}