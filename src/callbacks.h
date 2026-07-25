#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <Ecore.h>
#include "elicit.h"

/* capture X11 */
void on_mouse_down(void *data, Evas *evas_obj, Evas_Object *obj, void *event_info); // initiate capture
void on_mouse_up(void *data, Evas *evas_obj, Evas_Object *obj, void *event_info); // stop capture

/* RGB slider callbacks */
void on_r_slider_change(void *data, Evas_Object *obj, void *event_info);
void on_g_slider_change(void *data, Evas_Object *obj, void *event_info);
void on_b_slider_change(void *data, Evas_Object *obj, void *event_info);

/* rgb/hex value updates callbacks */
void on_r_entry_change(void *data, Evas_Object *obj, void *event_info);
void on_g_entry_change(void *data, Evas_Object *obj, void *event_info);
void on_b_entry_change(void *data, Evas_Object *obj, void *event_info);
void on_hex_entry_change(void *data, Evas_Object *obj, void *event_info);

/* rgb/hex value updates callback */
void update_color_widgets(App_Data *ad, int r, int g, int b, Evas_Object *source);

/* spinner callback */
void on_zoom_change(void *data, Evas_Object *obj, void *event_info);

void _grid_overlay_refresh(App_Data *ad);
void _grid_toggle_sync(App_Data *ad);

/* color picker */
void on_swatch_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
void on_swatch_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
void on_preview_move(void *data, Evas *e, void *event_info);

/* unused */
/*
void on_win_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
*/

/* frozen image after resize */
void on_preview_resize(void *data, Evas *e, Evas_Object *obj, void *event_info);

#endif /* CALLBACKS_H */