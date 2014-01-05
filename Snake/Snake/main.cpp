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
int map_size = 25;
int freq = 10;
int snake_lenght = 4;

struct snake_chain
{
	unsigned int x;
	unsigned int y;
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

	display = al_create_display(50+ROZMIAR_KLOCKA*map_size+200, 50+ROZMIAR_KLOCKA*map_size);
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

	timer = al_create_timer(1.0 / freq);
	if(!timer) 
	{
		al_destroy_display(display);
		al_destroy_event_queue(event_queue);
		al_destroy_font(font);
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize timer!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	al_init_primitives_addon();
	al_install_keyboard();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	return 1;
}

int create_map()
{
	for(int i=0; i<map_size; i++)
		for(int j=0; j<map_size; j++)
			playground[i][j] = 0;

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
	char snake_ass_removal_delay = 0;
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
	else
	{
		switch(direction)
		{
		case 1: //up
			if(snake_head->y == 0) return 0; //border were hit = game over
			snake_head->y = snake_head->y-1; break;
		case 2: //left
			if(snake_head->x == 0) return 0;
			snake_head->x = snake_head->x-1; break;
		case 3: //down
			if(snake_head->y == map_size-1) return 0;
			snake_head->y = snake_head->y+1; break;
		case 4: //right
			if(snake_head->x == map_size-1) return 0;
			snake_head->x = snake_head->x+1; break;
		//default: 
		}
	}
	fprintf(f, "new head: x=%d y=%d\n", snake_head->x, snake_head->y);

	if(playground[snake_head->y][snake_head->x] == 1)	//snake meets itself
	{
		al_draw_filled_rectangle(20+snake_head->x*ROZMIAR_KLOCKA, 20+snake_head->y*ROZMIAR_KLOCKA, 20+snake_head->x*ROZMIAR_KLOCKA+ROZMIAR_KLOCKA-1, 20+snake_head->y*ROZMIAR_KLOCKA+ROZMIAR_KLOCKA-1, al_map_rgb(255, 200, 200));
		al_flip_display();
		return 0; //game over
	}
	else if(playground[snake_head->y][snake_head->x] == 2) //food eaten
	{
		playground[snake_head->y][snake_head->x] = 1;// now snake here
		int x, y;
		do {		// generate new food where snake is not now
			x = rand() % map_size;
			y = rand() % map_size;
		} while(playground[y][x] > 0 || x==0 || x==map_size-1 || y==0 || y==map_size-1);
		playground[y][x] = 2;
		snake_ass_removal_delay = 1;
		fprintf(f, "new food: x=%d y=%d\n", x, y);
		snake_lenght++;
	}
	playground[snake_head->y][snake_head->x] = 1; //snake here

	if(!snake_ass_removal_delay)
	{
	//removing ass
	playground[snake_ass->y][snake_ass->x] = 0; //snake not here
	fprintf(f, "assR: x=%d y=%d\nn", snake_ass->x, snake_ass->y);
	snake_ass = snake_ass->next;
	free(snake_ass->previous);
	snake_ass->previous = NULL;
	}
	return 1; //game continues
}

void draw_game()
{
#define WHITE al_map_rgb(255, 255, 255)
#define RED al_map_rgb(255, 100, 100)
#define GREEN al_map_rgb(50, 255, 50)
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_rectangle(18, 18, 19+2+map_size*ROZMIAR_KLOCKA+1, 19+2+map_size*ROZMIAR_KLOCKA+1, (borders_exist?RED:WHITE), (borders_exist?2:1));
	al_draw_filled_rectangle(19, 19, 19+2+map_size*ROZMIAR_KLOCKA, 19+2+map_size*ROZMIAR_KLOCKA, al_map_rgb(50, 50, 50));
		
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
	al_draw_textf(font, al_map_rgb(255, 255, 255), 50+ROZMIAR_KLOCKA*map_size+30, 50, 0, "Lenght: %2d", snake_lenght);
	al_draw_textf(font, al_map_rgb(255, 255, 255), 50+ROZMIAR_KLOCKA*map_size+30, 70, 0, "SPACE = pause", snake_lenght);

	al_flip_display();
}

int main()
{
	f = fopen("debug.txt", "w");
	if(initialize_allegro() == -1)
		return 0;

 
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();
	srand(time(NULL));

	al_clear_to_color(al_map_rgb(0,0,50));
#define TEXT_X_POS(x) 50+x*20
	al_draw_text(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(1), 0, "SNAKE v0.5 by Baftek");
	al_draw_textf(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(3), 0, "Speed: %dHz", freq);
	al_draw_textf(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(4), 0, "Map size: %d", map_size);
	al_draw_text(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(5), 0, "Borders doesn't exist");
	al_draw_text(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(7), 0, "Press ANY KEY to begin");
	al_flip_display();
	while(1)
	{
		al_wait_for_event(event_queue, &ev);
		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
			return 0;  //exit program
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
			break;
	}

	al_clear_to_color(al_map_rgb(0,0,50));
	al_draw_text(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(2), 0, "Choose difficulty (press choice number key):");
	al_draw_text(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(3), 0, "1. easy");
	al_draw_text(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(4), 0, "2. easy with borders");
	al_draw_text(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(5), 0, "3. medium");
	al_draw_text(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(6), 0, "4. medium with borders");
	al_draw_text(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(7), 0, "5. hard");
	al_draw_text(font, al_map_rgb(255, 255, 255), 50, TEXT_X_POS(8), 0, "6. hard with borders");
	al_flip_display();

	while(1)
	{
		al_wait_for_event(event_queue, &ev);
		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
			return 0;  //exit program
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_1:
			case ALLEGRO_KEY_PAD_1: freq = 5; break;
			case ALLEGRO_KEY_2:
			case ALLEGRO_KEY_PAD_2: freq = 5; borders_exist = 1; break;
			case ALLEGRO_KEY_3:
			case ALLEGRO_KEY_PAD_3: freq = 13; break;
			case ALLEGRO_KEY_4:
			case ALLEGRO_KEY_PAD_4: freq = 13; borders_exist = 1; break;
			case ALLEGRO_KEY_5:
			case ALLEGRO_KEY_PAD_5: freq = 25; break;
			case ALLEGRO_KEY_6:
			case ALLEGRO_KEY_PAD_6: freq = 25; borders_exist = 1; break;
			default: freq = 13; 
			}
			break;
		}
	}
	al_set_timer_speed(timer, 1.0 / freq);

	//creating gamemap dyn. array
	playground = (char**)calloc(map_size, sizeof(char*));
	for(int i=0; i<map_size; i++)
	{
		playground[i] = (char*)calloc(map_size, sizeof(char));
	}

restart:
	create_map();
	al_start_timer(timer);
	while(1)
	{
		draw_game();
		//al_flush_event_queue(event_queue);
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
			else
			{	//pause
				al_clear_to_color(al_map_rgb(0,0,0));
				al_draw_text(font, al_map_rgb(255, 255, 255), 100, 100, 0, "PAUSE            press SPACE to return");
				al_flip_display();
				al_stop_timer(timer);
				al_wait_for_event(event_queue, &ev);
				while(!(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_SPACE || ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
					al_wait_for_event(event_queue, &ev);
				if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
					return 0;
				else
					al_start_timer(timer);
			}
			al_set_timer_count(timer, 0);
		}
		else if(ev.type == ALLEGRO_EVENT_TIMER)
		{
			if(!change_snake_position())
				break;
		}
		fprintf(f, "dir: %d\n", direction);
	}
	//game over
	//al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_text(font, al_map_rgb(255, 100, 100), 50+ROZMIAR_KLOCKA*map_size+30, 150, 0, "GAME OVER");
	al_draw_text(font, al_map_rgb(255, 255, 255), 50+ROZMIAR_KLOCKA*map_size+30, 170, 0, "Once again?");
	al_draw_text(font, al_map_rgb(255, 255, 255), 50+ROZMIAR_KLOCKA*map_size+30, 190, 0, "press any key");
	al_draw_text(font, al_map_rgb(255, 255, 255), 50+ROZMIAR_KLOCKA*map_size+30, 230, 0, "Want to quit?");
	al_draw_text(font, al_map_rgb(255, 255, 255), 50+ROZMIAR_KLOCKA*map_size+30, 250, 0, "press Q or ESC");
	al_flip_display();
	while(!(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN)))
		al_wait_for_event(event_queue, &ev);
	switch(ev.keyboard.keycode)
	{
		case ALLEGRO_KEY_Q:
		case ALLEGRO_KEY_ESCAPE: break;
		default: goto restart;
	}
	fclose(f);
	return 0;
}