#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
	
#include "rockshot.h"
#include "config.h"

//UUID: 142C1652-B31E-456C-9C94-A9D43ED041A6
#define MY_UUID { 0x14, 0x2C, 0x16, 0x52, 0xB3, 0x1E, 0x45, 0x6C, 0x9C, 0x94, 0xA9, 0xD4, 0x3E, 0xD0, 0x41, 0xA6 }
PBL_APP_INFO(MY_UUID,
             "Holiday-ish", "Matt Donders",
             1, 0, /* App version */
             RESOURCE_ID_APPICON,
             APP_INFO_WATCH_FACE);

#if INVERT_COLORS
#define COLOR_FOREGROUND GColorBlack
#define COLOR_BACKGROUND GColorWhite
#else
#define COLOR_FOREGROUND GColorWhite
#define COLOR_BACKGROUND GColorBlack
#endif

Window window;
TextLayer timeLayer;
#if TWO_LINE
TextLayer ishLayer;
#endif

// Declare a variable for storing time string
static char timeText[] = "00:00ish";

/**
 *  Function to set the time and date features on TextLayers
**/
void setTime(PblTm* t) {
	
  int ish_hours = t->tm_hour;
  int ish_minutes = t->tm_min;

  // Adjust hours for 12-hour time if needed.
  if ((!clock_is_24h_style()) && (ish_hours > 12)) {
	ish_hours = ish_hours - 12;
  }
	
  // Convert minutes (15-45) to text  
  if ((ish_minutes > 15) && (ish_minutes < 45)) {
	ish_minutes = 30;
	
	#if TWO_LINE
		snprintf(timeText, sizeof(timeText), "%d:%d", ish_hours, ish_minutes);
	#else
		snprintf(timeText, sizeof(timeText), "%d:%dish", ish_hours, ish_minutes);
	#endif
  }
	
  else {
	if (ish_minutes >= 45) {
	  ish_hours++;  
	}
	  
	ish_minutes = 0;
	
	#if TWO_LINE
		snprintf(timeText, sizeof(timeText), "%d:%d0", ish_hours, ish_minutes);
	#else
		snprintf(timeText, sizeof(timeText), "%d:%d0ish", ish_hours, ish_minutes);
	#endif
  }

	// Set the TextLayer text
	text_layer_set_text(&timeLayer, timeText);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;
	
  // ROCKSHOT SETUP
  rockshot_init(ctx); 
	
  resource_init_current_app(&APP_RESOURCES);

  // Initialize Window
  window_init(&window, "ISH Window");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, COLOR_BACKGROUND);
	
#if TWO_LINE
  // Initialize Text Layer
  text_layer_init(&timeLayer, GRect(0, 38, 144, 168));
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_text_color(&timeLayer, COLOR_FOREGROUND);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);

  // Initialize ISH Layer
  text_layer_init(&ishLayer, GRect(0, 86, 144, 168));
  text_layer_set_background_color(&ishLayer, GColorClear);
  text_layer_set_text_color(&ishLayer, COLOR_FOREGROUND);
  text_layer_set_font(&ishLayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(&ishLayer, GTextAlignmentCenter);
  text_layer_set_text(&ishLayer, "ish");
    
  // Add child (timeLayer) to our window
  layer_add_child(&window.layer, &timeLayer.layer);
  layer_add_child(&window.layer, &ishLayer.layer);
	
#else
  // Initialize Text Layer
  text_layer_init(&timeLayer, GRect(0, 69, 144, 168));
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_text_color(&timeLayer, COLOR_FOREGROUND);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);
	
  // Add child (timeLayer) to our window
  layer_add_child(&window.layer, &timeLayer.layer);
#endif	

	
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
	},
	 			
	.messaging_info = {
		.buffer_sizes = {
			.inbound = 124,
			.outbound = 256,
		}
	}
  };
	
  rockshot_main(&handlers);
  app_event_loop(params, &handlers);
}
