#ifndef ELICIT_H
#define ELICIT_H

#include <X11/Xlib.h>
#include <Elementary.h>

typedef struct _App_Data {
    Evas_Object *win;
    Evas_Object *preview;
    Evas_Object *swatch;
    Evas_Object *r_slider, *g_slider, *b_slider;
    Evas_Object *r_entry,  *g_entry,  *b_entry;
    Evas_Object *hex_entry;
    Evas_Object *zoom_spinner;
    Evas_Object *grid_toggle;
    Display     *xdisplay;
    int          zoom;
    int          win_x, win_y;   
    Ecore_Timer *capture_timer;
    Eina_Bool updating;
    Eina_Bool frozen; // weird struct name
} App_Data;

#endif