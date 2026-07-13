#include "callbacks.h"
#include "capture.h"

/* screen capture on mouse down */
void on_mouse_down(void *data, Evas *evas_obj, Evas_Object *obj, void *event_info)
{
   App_Data *ad = data;
   if (ad->frozen) ad->frozen = EINA_FALSE; // on_mouse_up sets EINA_TRUE
   if (!ad->capture_timer) {
      ad->capture_timer = ecore_timer_add(1.0 / 30.0, update_capture, ad);
   }
}

/* on mouse up stop capturing */
void on_mouse_up(void *data, Evas *evas_obj, Evas_Object *obj, void *event_info)
{
   App_Data *ad = data;
   if (ad->capture_timer)
     {
        ecore_timer_del(ad->capture_timer);
        ad->capture_timer = NULL;
        ad->frozen = EINA_TRUE; // see "elicit.h"
     }
}

/* RGB Slider functions */
void on_r_slider_change(void *data, Evas_Object *obj, void *event_info)
{
    App_Data *ad = data;
    if (ad->updating) return;
    int r = (int)elm_slider_value_get(obj);
    int g = (int)elm_slider_value_get(ad->g_slider);
    int b = (int)elm_slider_value_get(ad->b_slider);
    update_color_widgets(ad, r, g, b);
}

void on_g_slider_change(void *data, Evas_Object *obj, void *event_info)
{
    App_Data *ad = data;
    if (ad->updating) return;
    int r = (int)elm_slider_value_get(ad->r_slider);
    int g = (int)elm_slider_value_get(obj);
    int b = (int)elm_slider_value_get(ad->b_slider);
    update_color_widgets(ad, r, g, b);
}

void on_b_slider_change(void *data, Evas_Object *obj, void *event_info)
{
    App_Data *ad = data;
    if (ad->updating) return;
    int r = (int)elm_slider_value_get(ad->r_slider);
    int g = (int)elm_slider_value_get(ad->g_slider);
    int b = (int)elm_slider_value_get(obj);
    update_color_widgets(ad, r, g, b);
}

/*  update rgb/hex values on rgb_sliders change and the sliders themselves */
void
update_color_widgets(App_Data *ad, int r, int g, int b)
{

   if (ad->updating) return;
   ad->updating = EINA_TRUE;

   // char buf[8]; // #rrggbb\0
   char buf[16]; // #rrggbbaa
   /* update rgb entries */
   snprintf(buf, sizeof(buf), "%d", r);
   elm_object_text_set(ad->r_entry, buf);
   snprintf(buf, sizeof(buf), "%d", g);
   elm_object_text_set(ad->g_entry, buf);
   snprintf(buf, sizeof(buf), "%d", b);
   elm_object_text_set(ad->b_entry, buf);
   /* update cc entry */
   snprintf(buf, sizeof(buf), "%d %d %d", r, g, b);
   elm_object_text_set(ad->cc_entry, buf);

   /* update hex entry */
   snprintf(buf, sizeof(buf), "#%02x%02x%02x", r, g, b);
   elm_object_text_set(ad->hex_entry, buf);

   /* update the sliders automatically */
   elm_slider_value_set(ad->r_slider, (double)r);
   elm_slider_value_set(ad->g_slider, (double)g);
   elm_slider_value_set(ad->b_slider, (double)b);

   evas_object_color_set(ad->swatch, r, g, b, 255);

   ad->updating = EINA_FALSE;
}

/* update rgb/hex entries on input */
void on_r_entry_change(void *data, Evas_Object *obj, void *event_info)
{
    App_Data *ad = data;
    if (ad->updating) return;
    const char *s = elm_object_text_get(obj);
    int r = s ? atoi(s) : 0;
    if (r < 0) r = 0;
    if (r > 255) r = 255;
    int g = (int)elm_slider_value_get(ad->g_slider);
    int b = (int)elm_slider_value_get(ad->b_slider);
    update_color_widgets(ad, r, g, b);
}

void on_g_entry_change(void *data, Evas_Object *obj, void *event_info)
{
    App_Data *ad = data;
    if (ad->updating) return;
    const char *s = elm_object_text_get(obj);
    int g = s ? atoi(s) : 0;
    if (g < 0) g = 0;
    if (g > 255) g = 255;
    int r = (int)elm_slider_value_get(ad->r_slider);
    int b = (int)elm_slider_value_get(ad->b_slider);
    update_color_widgets(ad, r, g, b);
}

void on_b_entry_change(void *data, Evas_Object *obj, void *event_info)
{
    App_Data *ad = data;
    if (ad->updating) return;
    const char *s = elm_object_text_get(obj);
    int b = s ? atoi(s) : 0;
    if (b < 0) b = 0;
    if (b > 255) b = 255;
    int r = (int)elm_slider_value_get(ad->r_slider);
    int g = (int)elm_slider_value_get(ad->g_slider);
    update_color_widgets(ad, r, g, b);
}

// hex
void on_hex_entry_change(void *data, Evas_Object *obj, void *event_info)
{
    App_Data *ad = data;
    if (ad->updating) return;
    const char *s = elm_object_text_get(obj);
    if (!s) return;
    const char *p = (s[0] == '#') ? s + 1 : s;
    char expanded[7];
    if (strlen(p) == 3)
      {
         expanded[0] = p[0]; expanded[1] = p[0];
         expanded[2] = p[1]; expanded[3] = p[1];
         expanded[4] = p[2]; expanded[5] = p[2];
         expanded[6] = '\0';
         p = expanded;
      }
    else if (strlen(p) < 6) return;
    int r, g, b;
    if (sscanf(p, "%02x%02x%02x", &r, &g, &b) != 3) return;
    update_color_widgets(ad, r, g, b);
}

// spinner
void on_zoom_change(void *data, Evas_Object *obj, void *event_info)
{
   App_Data *ad = data;
   ad->zoom = (int)elm_spinner_value_get(obj);

   if (ad->zoom < 5)
     {
        elm_check_state_set(ad->grid_toggle, EINA_FALSE);
        elm_object_disabled_set(ad->grid_toggle, EINA_TRUE);
     }
   else
     {
        elm_object_disabled_set(ad->grid_toggle, EINA_FALSE);
     }
}

/* shared by the grid toggle and update_capture() -- shows/hides/sizes
 * the grid overlay based on toggle state + zoom, independent of whether
 * a capture is currently running */
void
_grid_overlay_refresh(App_Data *ad)
{
   Evas_Coord box_w, box_h;
   evas_object_geometry_get(ad->preview, NULL, NULL, &box_w, &box_h);

   if (elm_check_state_get(ad->grid_toggle) && ad->zoom >= 5 && box_w > 0 && box_h > 0)
     {
        Evas_Coord cell = ELM_SCALE_SIZE(ad->zoom);
        evas_object_image_fill_set(ad->grid_overlay, 0, 0, cell, cell);
        evas_object_resize(ad->grid_overlay, box_w, box_h);
        evas_object_show(ad->grid_overlay);
     }
   else
     {
        evas_object_hide(ad->grid_overlay);
     }
}

/* grid toggle change -- shows/hides immediately, does not wait for a capture tick */
void
on_grid_toggle_change(void *data, Evas_Object *obj, void *event_info)
{
   App_Data *ad = data;
   _grid_overlay_refresh(ad);
}

// color picker
void on_swatch_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   App_Data *ad = data;
   if (!ad->frozen) return;
   evas_event_callback_add(e, EVAS_CALLBACK_MOUSE_MOVE, on_preview_move, ad);
}

void on_swatch_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   //App_Data *ad = data;
   evas_event_callback_del(e, EVAS_CALLBACK_MOUSE_MOVE, on_preview_move);
}

void on_preview_move(void *data, Evas *e, void *event_info)
{
   App_Data *ad = data;
   Evas_Event_Mouse_Move *ev = event_info;
   int px, py, gw, gh;
   evas_object_geometry_get(ad->preview, &px, &py, &gw, &gh);
   int mx = ev->cur.canvas.x - px;
   int my = ev->cur.canvas.y - py;
   if (mx < 0 || my < 0 || mx >= gw || my >= gh) return;
   int out_w, out_h;
   evas_object_image_size_get(ad->preview, &out_w, &out_h);
   if (out_w <= 0 || out_h <= 0) return;
   /* snap to zoom cell centre */
   int zoom = ad->zoom;
   int bx = (mx * out_w / gw / zoom) * zoom + zoom / 2;
   int by = (my * out_h / gh / zoom) * zoom + zoom / 2;
   if (bx >= out_w) bx = out_w - 1;
   if (by >= out_h) by = out_h - 1;
   unsigned int *pixels = evas_object_image_data_get(ad->preview, EINA_FALSE);
   if (!pixels) return;
   unsigned int colour = pixels[by * out_w + bx];
   update_color_widgets(ad,
                        (colour >> 16) & 0xff,
                        (colour >>  8) & 0xff,
                        (colour      ) & 0xff);
}

/* unused */

/*
void
on_win_move(void *data, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   App_Data *ad = data;
   static int call_count = 0;
   call_count++;
   evas_object_geometry_get(obj, &ad->win_x, &ad->win_y, NULL, NULL);
   fprintf(stderr, "MOVE_CB #%d: win_x=%d win_y=%d\n", call_count, ad->win_x, ad->win_y);
}
*/

/* stretch the capture on resize */
void
on_preview_resize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   int w, h;
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);
   if (w <= 0 || h <= 0) return;
   evas_object_image_fill_set(obj, 0, 0, w, h);

   int iw, ih;
   evas_object_image_size_get(obj, &iw, &ih);
   fprintf(stderr, "[PREVIEW_RESIZE] geom=%dx%d image_size=%dx%d\n", w, h, iw, ih);
}
