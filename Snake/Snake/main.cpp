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
#define TEXT_SIZE 17
char direction;

int main(int argc, char *argv[])
{
	if(!al_init()) 
	{
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize allegro!", 
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	display = al_create_display(800, 600);
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

	timer = al_create_timer(1.0 / 2);
	if(!timer) 
	{
		al_destroy_display(display);
		al_destroy_event_queue(event_queue);
		al_destroy_font(font);
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	al_install_keyboard();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	ALLEGRO_EVENT ev;
 
	al_init_primitives_addon();
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();

	while(1)
	{
		al_flush_event_queue(event_queue);
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (ev.type == ALLEGRO_EVENT_KEY_DOWN && (ev.keyboard.keycode == ALLEGRO_KEY_Q || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)))
			break;  //exit program
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			al_clear_to_color(al_map_rgb(0,0,0));
			if(ev.keyboard.keycode != ALLEGRO_KEY_SPACE)
			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_DOWN:
				direction = 3; break;
			case ALLEGRO_KEY_UP:
				direction = 1; break;
			case ALLEGRO_KEY_LEFT:
				direction = 2; break;
			case ALLEGRO_KEY_RIGHT:
				direction = 4; break;
			}
			al_flip_display();
		}
		else //żadnego przerwania
			continue;
		//cośtam
	}

	return 0;
}