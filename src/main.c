#include <pebble.h>

static Window* window;
static TextLayer* text_layer;
static DictationSession* dictation_session;

static void dictation_session_handler(DictationSession* session, DictationSessionStatus status, char* transcription, void* context) {
  DictionaryIterator *iterator;
  app_message_outbox_begin(&iterator);
  dict_write_cstring(iterator, 0, transcription);
  app_message_outbox_send();
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "SELECT to begin");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void* context) {
  dictation_session_start(dictation_session);
}

static void click_config_provider(void* context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void init(void) {
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Window
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  
  // Dictation Session
  dictation_session = dictation_session_create(0, dictation_session_handler, NULL);
  dictation_session_enable_confirmation(dictation_session, true);
  
  window_set_click_config_provider(window, click_config_provider);
}

static void deinit(void) {
  window_destroy(window);
  dictation_session_stop(dictation_session);
  dictation_session_destroy(dictation_session);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}