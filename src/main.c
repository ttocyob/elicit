#include "config.h"

#include <Elementary.h>
#include "elicit.h"
#include "callbacks.h"
#include "capture.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

static void
win_delete_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   App_Data *ad = data;
   Elicit_Config cfg = {0};

   //cfg.x = ad->win_x;
   //cfg.y = ad->win_y;
//fprintf(stderr, "SAVE: cfg.x=%d cfg.y=%d\n", cfg.x, cfg.y);

   cfg.r = (int)elm_slider_value_get(ad->r_slider);
   cfg.g = (int)elm_slider_value_get(ad->g_slider);
   cfg.b = (int)elm_slider_value_get(ad->b_slider);
   cfg.zoom_factor = ad->zoom;

   if (!elicit_config_save(&cfg))
      fprintf(stderr, "elicit: failed to save state\n");

   capture_shutdown(data);
    
   /* shutdown config subsystem */
   elicit_config_shutdown();
   free(ad);

   elm_exit();
}

static Eina_Bool
_config_changed_cb(void *data, int type EINA_UNUSED, void *event EINA_UNUSED)
{
    static double last_scale = 0.0;
    App_Data *ad = data;
    double scale = elm_config_scale_get();
    if (scale < 1.0 || scale > 2.0) scale = 1.0;
    if (scale == last_scale) return ECORE_CALLBACK_PASS_ON;
    last_scale = scale;

    evas_object_size_hint_min_set(ad->preview,
                                  ELM_SCALE_SIZE(212), ELM_SCALE_SIZE(200));
    evas_object_size_hint_min_set(ad->preview_shadow,
                                  ELM_SCALE_SIZE(212), ELM_SCALE_SIZE(200)); // re-enabled 
    evas_object_image_fill_set(ad->preview_shadow,
                               0, 0, ELM_SCALE_SIZE(212), ELM_SCALE_SIZE(200));

    evas_object_size_hint_min_set(ad->swatch,
                                  ELM_SCALE_SIZE(20), ELM_SCALE_SIZE(20));
    evas_object_size_hint_min_set(ad->cpicker_mask,
                                  ELM_SCALE_SIZE(20), ELM_SCALE_SIZE(20));
    evas_object_image_fill_set(ad->cpicker_mask,
                               0, 0, ELM_SCALE_SIZE(20), ELM_SCALE_SIZE(20));


    evas_smart_objects_calculate(evas_object_evas_get(ad->win));

    //evas_object_resize(ad->win, ELM_SCALE_SIZE(212), ELM_SCALE_SIZE(403));
    //evas_object_resize(ad->win, ELM_SCALE_SIZE(212), ELM_SCALE_SIZE(416));

Evas_Coord min_w, min_h;
evas_object_size_hint_min_get(ad->outer_table, &min_w, &min_h);
evas_object_resize(ad->win, min_w, min_h);
fprintf(stderr, "[SCALE_RESIZE] scale=%.2f min_w=%d min_h=%d\n", scale, (int)min_w, (int)min_h);

Evas_Coord ww, wh, pw, ph;
evas_object_geometry_get(ad->win, NULL, NULL, &ww, &wh);
evas_object_geometry_get(ad->preview, NULL, NULL, &pw, &ph);

    return ECORE_CALLBACK_PASS_ON;
}

EAPI_MAIN int
elm_main(int argc EINA_UNUSED, char **argv EINA_UNUSED)
{

   /* load saved state from .eet */
   Elicit_Config *cfg = elicit_config_load();
//fprintf(stderr, "LOAD: cfg->x=%d cfg->y=%d\n", cfg->x, cfg->y);

   /* declare ad */
   App_Data *ad = calloc(1, sizeof(App_Data));
   if (!ad) return -1;

   ad->win = elm_win_util_standard_add("elicit_elm", "Elicit");

   Evas_Object *win;
   Evas_Object *outer_table, *cpicker_table, *controls_table, *hex_table;
   Evas_Object *color_frame, *frame_box;
   Evas_Object *r_slider, *g_slider, *b_slider;
   Evas_Object *r_entry, *g_entry, *b_entry, *hex_entry, *cc_entry;
   Evas_Object *grid_toggle, *zoom_spinner;

   win = ad->win;

   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_smart_callback_add(win, "delete,request", win_delete_cb, ad);
   //evas_object_event_callback_add(win, EVAS_CALLBACK_MOVE, on_win_move, ad);

   /* table: row 0 = preview, row 2 = cpicker, row 3 = controls, row 4 = hex */
   outer_table = elm_table_add(win);
ad->outer_table = outer_table;
   elm_table_padding_set(outer_table, 0, 5); // increase padding between outer and cpicker table
evas_object_size_hint_weight_set(outer_table, EVAS_HINT_EXPAND, 0.0); // debug
   evas_object_size_hint_weight_set(outer_table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, outer_table);
   evas_object_show(outer_table);

   /* preview: expands in both directions */
   Evas_Object *preview = evas_object_image_add(evas_object_evas_get(win));
   evas_object_image_alpha_set(preview, EINA_FALSE);
   evas_object_image_colorspace_set(preview, EVAS_COLORSPACE_ARGB8888);

   /* set initial solid color */
   int init_w = 1, init_h = 1;
   evas_object_image_size_set(preview, init_w, init_h);
   evas_object_image_fill_set(preview, 0, 0, init_w, init_h);
   
   unsigned int *init_pixels = evas_object_image_data_get(preview, EINA_TRUE);
   if (init_pixels)
     {
        init_pixels[0] = 0xff000000 | ((cfg->r & 0xff) << 16) | ((cfg->g & 0xff) << 8) | (cfg->b & 0xff);
        evas_object_image_data_set(preview, init_pixels);
        evas_object_image_data_update_add(preview, 0, 0, init_w, init_h);
     }

   evas_object_size_hint_min_set(preview, ELM_SCALE_SIZE(212), ELM_SCALE_SIZE(200));
   //evas_object_size_hint_weight_set(preview, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//evas_object_size_hint_weight_set(preview, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(preview, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(outer_table, preview, 0, 0, 1, 1);
   evas_object_show(preview);
   ad->preview = preview;
   evas_object_event_callback_add(preview, EVAS_CALLBACK_MOUSE_DOWN, on_mouse_down, ad); // mouse down
   evas_object_event_callback_add(preview, EVAS_CALLBACK_MOUSE_UP, on_mouse_up, ad); // mouse up
   evas_object_event_callback_add(preview, EVAS_CALLBACK_RESIZE, on_preview_resize, ad); // resize

//

   /* dropshadow */
   Evas_Object *preview_shadow = evas_object_image_add(evas_object_evas_get(win)); // 
   /* make sure the dropshadow image does not capture mouse events */
   evas_object_pass_events_set(preview_shadow, EINA_TRUE);
   /* tell Evas to enable the alpha channel transparency */
   //evas_object_image_alpha_set(preview_shadow, EINA_TRUE);
   char shadow_path[PATH_MAX];
   snprintf(shadow_path, sizeof(shadow_path), ELICIT_DATADIR "/images/preview_shad.png");
   evas_object_image_file_set(preview_shadow, shadow_path, NULL); 
   /* set the transpareny manually on each RGBA channel */
   evas_object_color_set(preview_shadow, 92, 92, 92, 92); // Evas only handles premultiplied colors (0 <= R,G,B <= A <= 255)

   evas_object_image_border_set(preview_shadow, 1, 1, 0, 6);
   evas_object_image_border_scale_set(preview_shadow, 1.0);
   evas_object_image_fill_set(preview_shadow, 0, 0, ELM_SCALE_SIZE(212), ELM_SCALE_SIZE(200));
   
   // match the size hints and alignment of *swatch including same cell
   evas_object_size_hint_min_set(preview_shadow, ELM_SCALE_SIZE(212), ELM_SCALE_SIZE(200));
   //evas_object_size_hint_weight_set(preview_shadow, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//evas_object_size_hint_weight_set(preview_shadow, EVAS_HINT_EXPAND, 0.0); // debug
   evas_object_size_hint_align_set(preview_shadow, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(outer_table, preview_shadow, 0, 0, 1, 1);
   evas_object_event_callback_add(preview_shadow, EVAS_CALLBACK_RESIZE, on_preview_resize, ad); // resize
   
   evas_object_raise(preview_shadow); // raise the image 
   evas_object_show(preview_shadow);
   ad->preview_shadow = preview_shadow;

//

   /* table 1: color picker table */
   cpicker_table = elm_table_add(win);
   elm_table_padding_set(cpicker_table, 5, 0); // padding between widgets
   evas_object_size_hint_weight_set(cpicker_table, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(cpicker_table, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_padding_set(cpicker_table, 7, 6, 2, 0);
   elm_table_pack(outer_table, cpicker_table, 0, 1, 1, 1);
   evas_object_show(cpicker_table);

   /* column 0: color swatch */
   Evas_Object *swatch = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_color_set(swatch, 0, 0, 0, 255);
   evas_object_size_hint_min_set(swatch, ELM_SCALE_SIZE(20), ELM_SCALE_SIZE(20));
   evas_object_size_hint_weight_set(swatch, 0.0, 0.0);
   evas_object_size_hint_align_set(swatch, 0.0, 0.5); // (swatch, 0.0, 0.5) don't stretch me vertically
   elm_table_pack(cpicker_table, swatch, 0, 0, 1, 1);
   evas_object_show(swatch);
   ad->swatch = swatch;
   evas_object_event_callback_add(swatch, EVAS_CALLBACK_MOUSE_DOWN, on_swatch_down, ad); // mouse down
   evas_object_event_callback_add(swatch, EVAS_CALLBACK_MOUSE_UP, on_swatch_up, ad); // mouse up

   /* mask image over *swatch */
   Evas_Object *cpicker_mask = evas_object_image_add(evas_object_evas_get(win)); // 
   /* make sure the overlay image does not capture mouse events */
   evas_object_pass_events_set(cpicker_mask, EINA_TRUE);
   char mask_path[PATH_MAX];
   snprintf(mask_path, sizeof(mask_path), ELICIT_DATADIR "/images/cpicker_mask.png");
   evas_object_image_file_set(cpicker_mask, mask_path, NULL); 
   evas_object_image_border_set(cpicker_mask, 4, 4, 4, 4);
   evas_object_image_border_scale_set(cpicker_mask, 1.0);
   evas_object_image_fill_set(cpicker_mask, 0, 0, ELM_SCALE_SIZE(20), ELM_SCALE_SIZE(20));
   
   // match the size hints and alignment of *swatch including same cell
   evas_object_size_hint_min_set(cpicker_mask, ELM_SCALE_SIZE(20), ELM_SCALE_SIZE(20));
   evas_object_size_hint_weight_set(cpicker_mask, 0.0, 0.0);
   evas_object_size_hint_align_set(cpicker_mask, 0.0, 0.5);
   elm_table_pack(cpicker_table, cpicker_mask, 0, 0, 1, 1);
   
   // place the overlay image over *swatch
   evas_object_raise(cpicker_mask); 
   evas_object_show(cpicker_mask);
   ad->cpicker_mask   = cpicker_mask;

   /* column 1: toggle label */
   Evas_Object *toggle_label = elm_label_add(win);
   //elm_object_text_set(toggle_label, "<b>Grid</b>");
   elm_object_text_set(toggle_label, "Grid");
   evas_object_size_hint_weight_set(toggle_label, 0.0, 0.0);
   evas_object_size_hint_align_set(toggle_label, 0.0, EVAS_HINT_FILL);
   evas_object_size_hint_padding_set(toggle_label, 0, 0, 0, 0); // padding
   //elm_table_pack(cpicker_table, toggle_label, 0, 0, 1, 1);
   elm_table_pack(cpicker_table, toggle_label, 1, 0, 1, 1); // column 1
   //evas_object_hide(toggle_label); // hide the label
   evas_object_show(toggle_label); // show the label

   /* column 2: toggle */
   grid_toggle = elm_check_add(win);
   elm_object_style_set(grid_toggle, "toggle");
   evas_object_size_hint_weight_set(grid_toggle, 0.0, 0.0);
   evas_object_size_hint_align_set(grid_toggle, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(cpicker_table, grid_toggle, 2, 0, 1, 1); // column 2
   //evas_object_hide(grid_toggle); // hide the toggle
   evas_object_show(grid_toggle); // show the toggle

   /* column 3: zoom label */
   Evas_Object *zoom_label = elm_label_add(win);
   //elm_object_text_set(zoom_label, "<b>R</b>");
   elm_object_text_set(zoom_label, "Zoom");
   evas_object_size_hint_weight_set(zoom_label, 0.0, 0.0);
   evas_object_size_hint_align_set(zoom_label, 0.0, EVAS_HINT_FILL);
   evas_object_size_hint_padding_set(zoom_label, 0, 0, 0, 0); // padding
   elm_table_pack(cpicker_table, zoom_label, 3, 0, 1, 1); // column 3
   evas_object_show(zoom_label);

   /* column 4: spinner */
   zoom_spinner = elm_spinner_add(win);
   elm_spinner_min_max_set(zoom_spinner, 1, 10); // 16 is too high
   evas_object_size_hint_weight_set(zoom_spinner, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(zoom_spinner, EVAS_HINT_FILL, EVAS_HINT_FILL); // EVAS_HINT_FILL, 0.5
   elm_table_pack(cpicker_table, zoom_spinner, 4, 0, 1, 1); // column 4
   evas_object_smart_callback_add(zoom_spinner, "changed", on_zoom_change, ad); // smart callback change
   evas_object_show(zoom_spinner);

   /* elm_frame */
   color_frame = elm_frame_add(win);
   elm_object_text_set(color_frame, "Color Values");
   evas_object_size_hint_weight_set(color_frame, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(color_frame, EVAS_HINT_FILL, 0.0);
   evas_object_size_hint_padding_set(color_frame, 4, 4, 2, 4);
   elm_table_pack(outer_table, color_frame, 0, 2, 1, 1);
   evas_object_show(color_frame);

   /* elm_box inside elm_frame */
   frame_box = elm_box_add(win);
   //elm_box_padding_set(frame_box, 0, -3); // reduce the spacing between the tables (neg values)
   elm_box_horizontal_set(frame_box, EINA_FALSE);
   evas_object_size_hint_weight_set(frame_box, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(frame_box, EVAS_HINT_FILL, 0.0);
   elm_object_content_set(color_frame, frame_box);
   evas_object_show(frame_box);

   /* table 2: controls table */
   controls_table = elm_table_add(win);
   //elm_table_padding_set(controls_table, 0, -5); // reduce spacing between the rows (neg values)
   evas_object_size_hint_weight_set(controls_table, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(controls_table, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(frame_box, controls_table);
   evas_object_show(controls_table);

   /* r row  */
   Evas_Object *red_label = elm_label_add(win);
   //elm_object_text_set(red_label, "<b>R</b>");
   elm_object_text_set(red_label, "R");
   evas_object_size_hint_weight_set(red_label, 0.0, 0.0);
   evas_object_size_hint_align_set(red_label, 0.0, EVAS_HINT_FILL);
   evas_object_size_hint_padding_set(red_label, 3, 0, 0, 0);
   elm_table_pack(controls_table, red_label, 0, 0, 1, 1);
   evas_object_show(red_label);

   r_slider = elm_slider_add(win);
   elm_slider_min_max_set(r_slider, 0, 255); // set the min/max values
   elm_slider_value_set(r_slider, 0); // set default value
   elm_slider_indicator_show_set(r_slider, EINA_FALSE); // don't display the tooltip by default
   evas_object_size_hint_weight_set(r_slider, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(r_slider, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_padding_set(r_slider, 0, 0, 0, 0);
   elm_table_pack(controls_table, r_slider, 1, 0, 1, 1);
   evas_object_smart_callback_add(r_slider, "changed", on_r_slider_change, ad); // smart callback change
   evas_object_show(r_slider);

   r_entry = elm_entry_add(win);
   evas_object_smart_callback_add(r_entry, "activated", on_r_entry_change, ad); // active
   evas_object_smart_callback_add(r_entry, "unfocused", on_r_entry_change, ad); // unfocused
   elm_object_part_text_set(r_entry, "guide", "0"); // is the guide really necessary?
   elm_object_text_set(r_entry, "0");
   elm_entry_single_line_set(r_entry, EINA_TRUE);
   elm_entry_scrollable_set(r_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(r_entry, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(r_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   //evas_object_size_hint_min_set(r_entry, 64, 0);
   //evas_object_size_hint_max_set(r_entry, 64, -1);
   //evas_object_size_hint_padding_set(r_entry, 64, 0, 0, 0);
   evas_object_smart_callback_add(r_entry, "changed,user", on_r_entry_change, ad); // smart callback change
   elm_table_pack(controls_table, r_entry, 2, 0, 1, 1);
   evas_object_show(r_entry);

   /* g row */
   Evas_Object *green_label = elm_label_add(win);
   elm_object_text_set(green_label, "G");
   evas_object_size_hint_weight_set(green_label, 0.0, 0.0);
   evas_object_size_hint_align_set(green_label, 0.0, EVAS_HINT_FILL);
   evas_object_size_hint_padding_set(green_label, 3, 0, 0, 0);
   elm_table_pack(controls_table, green_label, 0, 1, 1, 1);
   evas_object_show(green_label);

   g_slider = elm_slider_add(win);
   elm_slider_min_max_set(g_slider, 0, 255); // set the min/max values
   elm_slider_value_set(g_slider, 0); // set default value
   elm_slider_indicator_show_set(g_slider, EINA_FALSE); // don't display the tooltip by default
   evas_object_size_hint_weight_set(g_slider, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(g_slider, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_padding_set(g_slider, 0, 0, 0, 0);
   elm_table_pack(controls_table, g_slider, 1, 1, 1, 1);
   evas_object_smart_callback_add(g_slider, "changed", on_g_slider_change, ad); // smart callback change
   evas_object_show(g_slider);

   g_entry = elm_entry_add(win);
   evas_object_smart_callback_add(g_entry, "activated", on_g_entry_change, ad); // active
   evas_object_smart_callback_add(g_entry, "unfocused", on_g_entry_change, ad); // unfocused
   elm_object_part_text_set(g_entry, "guide", "0"); // is the guide really necessary?
   elm_object_text_set(g_entry, "0");
   elm_entry_single_line_set(g_entry, EINA_TRUE);
   elm_entry_scrollable_set(g_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(g_entry, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(g_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   //evas_object_size_hint_min_set(g_entry, 64, 0);
   //evas_object_size_hint_max_set(g_entry, 64, -1);
   //evas_object_size_hint_padding_set(g_entry, 64, 0, 0, 0);
   evas_object_smart_callback_add(g_entry, "changed,user", on_g_entry_change, ad); // smart callback change
   elm_table_pack(controls_table, g_entry, 2, 1, 1, 1);
   evas_object_show(g_entry);

   /* b row */
   Evas_Object *blue_label = elm_label_add(win);
   elm_object_text_set(blue_label, "B");
   evas_object_size_hint_weight_set(blue_label, 0.0, 0.0);
   evas_object_size_hint_align_set(blue_label, 0.0, EVAS_HINT_FILL);
   //evas_object_size_hint_min_set(entry, 48, 0);
   evas_object_size_hint_padding_set(blue_label, 3, 0, 0, 0);
   elm_table_pack(controls_table, blue_label, 0, 2, 1, 1);
   evas_object_show(blue_label);

   b_slider = elm_slider_add(win);
   elm_slider_min_max_set(b_slider, 0, 255); // set the min/max values
   elm_slider_value_set(b_slider, 0); // set default value
   elm_slider_indicator_show_set(b_slider, EINA_FALSE); // don't display the tooltip by default
   evas_object_size_hint_weight_set(b_slider, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(b_slider, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_padding_set(b_slider, 0, 0, 0, 0);
   elm_table_pack(controls_table, b_slider, 1, 2, 1, 1);
   evas_object_smart_callback_add(b_slider, "changed", on_b_slider_change, ad); // smart callback change
   evas_object_show(b_slider);

   b_entry = elm_entry_add(win);
   evas_object_smart_callback_add(b_entry, "activated", on_b_entry_change, ad); // active
   evas_object_smart_callback_add(b_entry, "unfocused", on_b_entry_change, ad); // unfocused
   elm_object_part_text_set(b_entry, "guide", "0"); // is the guide really necessary?
   elm_object_text_set(b_entry, "0");
   elm_entry_single_line_set(b_entry, EINA_TRUE);
   elm_entry_scrollable_set(b_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(b_entry, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(b_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   //evas_object_size_hint_min_set(b_entry, 64, 0);
   //evas_object_size_hint_max_set(b_entry, 64, -1);
   //evas_object_size_hint_padding_set(b_entry, 64, 0, 0, 0);
   evas_object_smart_callback_add(b_entry, "changed,user", on_b_entry_change, ad); // smart callback change
   elm_table_pack(controls_table, b_entry, 2, 2, 1, 1);
   evas_object_show(b_entry);

   /* table 3: hex table */
   hex_table = elm_table_add(win);
   elm_table_padding_set(hex_table, 0, 0);
   evas_object_size_hint_weight_set(hex_table, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(hex_table, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(frame_box, hex_table);
   evas_object_show(hex_table);

   Evas_Object *hex_label = elm_label_add(win);
   elm_object_text_set(hex_label, "Hex");
   evas_object_size_hint_weight_set(hex_label, 0.0, 0.0);
   evas_object_size_hint_align_set(hex_label, 0.0, EVAS_HINT_FILL);
   //evas_object_size_hint_min_set(hex_label, 48, 0);
   evas_object_size_hint_padding_set(hex_label, 3, 0, 0, 0);
   elm_table_pack(hex_table, hex_label, 0, 0, 1, 1);
   evas_object_show(hex_label);

/* hex minimum size control: https://www.enlightenment.org/develop/legacy/samples/elm_min_size_control */
Evas_Object *hex_rec = evas_object_rectangle_add(evas_object_evas_get(win));
evas_object_size_hint_weight_set(hex_rec, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
evas_object_size_hint_align_set(hex_rec, EVAS_HINT_FILL, EVAS_HINT_FILL);
evas_object_size_hint_min_set(hex_rec, ELM_SCALE_SIZE(10), 0);
elm_table_pack(hex_table, hex_rec, 1, 0, 1, 1); /* same cell as hex_entry */
/* do not display the rectangle */

   hex_entry = elm_entry_add(win);
   evas_object_smart_callback_add(hex_entry, "activated", on_hex_entry_change, ad); // active
   evas_object_smart_callback_add(hex_entry, "unfocused", on_hex_entry_change, ad); // unfocused
   // elm_object_part_text_set(hex_entry, "guide", "#hex"); // no need for a guide here due to update_color_widgets callback
   elm_object_text_set(hex_entry, "#000000");
   elm_entry_single_line_set(hex_entry, EINA_TRUE);
   elm_entry_scrollable_set(hex_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(hex_entry, 1.4, 0.0);
   evas_object_size_hint_align_set(hex_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   //evas_object_size_hint_min_set(hex_entry, 48, 0);
   //evas_object_size_hint_max_set(hex_entry, 48, 0);
   evas_object_size_hint_padding_set(hex_entry, 3, 0, 0, 0);
   elm_table_pack(hex_table, hex_entry, 1, 0, 1, 1);
   evas_object_show(hex_entry);

//

   Evas_Object *cc_label = elm_label_add(win);
   elm_object_text_set(cc_label, "cc");
   evas_object_size_hint_weight_set(cc_label, 0.0, 0.0);
   evas_object_size_hint_align_set(cc_label, 0.0, EVAS_HINT_FILL);
   //evas_object_size_hint_min_set(cc_label, 48, 0);
   evas_object_size_hint_padding_set(cc_label, 3, 0, 0, 0);
   elm_table_pack(hex_table, cc_label, 2, 0, 1, 1);
   evas_object_show(cc_label);

   cc_entry = elm_entry_add(win);
   //evas_object_smart_callback_add(cc_entry, "activated", on_cc_entry_change, ad); // active
   //evas_object_smart_callback_add(cc_entry, "unfocused", on_cc_entry_change, ad); // unfocused
   // elm_object_part_text_set(cc_entry, "guide", "#ffffff #ffffff #ffffff "); // no need for a guide here due to update_color_widgets callback
   elm_object_text_set(cc_entry, "# # #");
   elm_entry_single_line_set(cc_entry, EINA_TRUE);
   elm_entry_scrollable_set(cc_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(cc_entry, 3.6, 0.0);
   evas_object_size_hint_align_set(cc_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   //evas_object_size_hint_min_set(cc_entry, 48, 0);
   evas_object_size_hint_padding_set(cc_entry, 3, 0, 0, 0);
   elm_table_pack(hex_table, cc_entry, 3, 0, 1, 1);
   evas_object_show(cc_entry);

//

   /* store the widget pointers into "ad" */
   ad->r_slider     = r_slider;
   ad->g_slider     = g_slider;
   ad->b_slider     = b_slider;
   ad->r_entry      = r_entry;
   ad->g_entry      = g_entry;
   ad->b_entry      = b_entry;
   ad->hex_entry    = hex_entry;
   ad->zoom_spinner = zoom_spinner;
   ad->grid_toggle  = grid_toggle;
   ad->cc_entry     = cc_entry;

   update_color_widgets(ad, cfg->r, cfg->g, cfg->b);
   elm_spinner_value_set(ad->zoom_spinner, cfg->zoom_factor);
   ad->zoom = cfg->zoom_factor;
   //ad->win_x = cfg->x;
   //ad->win_y = cfg->y;

   capture_init(ad);

  // evas_object_move(ad->win, cfg->x, cfg->y);
//fprintf(stderr, "MOVE: moved to cfg->x=%d cfg->y=%d\n", cfg->x, cfg->y);
   //evas_object_resize(ad->win, ELM_SCALE_SIZE(212), ELM_SCALE_SIZE(416));

evas_smart_objects_calculate(evas_object_evas_get(ad->win));
Evas_Coord min_w, min_h;
evas_object_size_hint_min_get(ad->outer_table, &min_w, &min_h);
evas_object_resize(ad->win, min_w, min_h);
fprintf(stderr, "[INIT_RESIZE] min_w=%d min_h=%d\n", (int)min_w, (int)min_h);


   evas_object_show(ad->win);

//int show_x, show_y;
//evas_object_geometry_get(ad->win, &show_x, &show_y, NULL, NULL);
//fprintf(stderr, "SHOW: geometry now x=%d y=%d\n", show_x, show_y);

   ecore_event_handler_add(ELM_EVENT_CONFIG_ALL_CHANGED, _config_changed_cb, ad);

   elm_run();

   elicit_config_shutdown(); /* free eet */

   free(cfg);

   return 0;
}
ELM_MAIN()