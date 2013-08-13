#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
	
#include "mini-printf.h"


#define MY_UUID { 0x14, 0x2C, 0x16, 0x52, 0xB3, 0x1E, 0x45, 0x6C, 0x9C, 0x94, 0xA9, 0xD4, 0x3E, 0xD0, 0x41, 0xA6 }
PBL_APP_INFO(MY_UUID,
             "Holiday-ish", "Matt Donders",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
TextLayer timeLayer;

// Declare a variable for storing time string
static char timeText[] = "00:00ish";

/**
 *  Function to set the time and date features on TextLayers
**/
void setTime(PblTm* t) {
	
  int ish_hours = t->tm_hour;
  int ish_minutes = t->tm_min;
  
  if ((ish_minutes > 15) && (ish_minutes < 45)) {
	ish_minutes = 30;
	mini_snprintf(timeText, sizeof(timeText), "%d:%dish", ish_hours, ish_minutes);
  }
  else {
	ish_minutes = 0;
	mini_snprintf(timeText, sizeof(timeText), "%d:%d0ish", ish_hours, ish_minutes);
  }

	// Set the TextLayer text
	text_layer_set_text(&timeLayer, timeText);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  // Initialize Window
  window_init(&window, "ISH Window");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);
	
  // Initialize Text Layer
  text_layer_init(&timeLayer, GRect(0, 70, 144, 168));
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_text_color(&timeLayer, GColorWhite);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
  text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);
	
  // Add child (timeLayer) to our window
  layer_add_child(&window.layer, &timeLayer.layer);
	
  // Set initial time so display isn't blank
  PblTm time;
  get_time(&time);
  setTime(&time);
}


/**
 *  Handle function every minute
**/
  
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;
	
  PblTm time; // Stores time info
  get_time(&time); // Fill with current time
	
  setTime(t->tick_time); // Change time every minute
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
	  
	.tick_info = {
		.tick_handler = &handle_minute_tick,
		.tick_units = MINUTE_UNIT
	}
  };
  app_event_loop(params, &handlers);
}