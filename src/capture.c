#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "callbacks.h"
#include "capture.h"

Eina_Bool
capture_init(App_Data *ad)
{
   ad->xdisplay = XOpenDisplay(NULL);
   if (!ad->xdisplay)
     {
        fprintf(stderr, "elicit: cannot open X display\n");
        return EINA_FALSE;
     }
   ad->capture_timer = NULL;
   return EINA_TRUE;
}

void
capture_shutdown(App_Data *ad)
{
   if (ad->capture_timer)
     {
        ecore_timer_del(ad->capture_timer);
        ad->capture_timer = NULL;
     }
   if (ad->xdisplay)
     {
        XCloseDisplay(ad->xdisplay);
        ad->xdisplay = NULL;
     }
}

Eina_Bool
update_capture(void *data)
{
   App_Data *ad = data;
   Display  *dpy = ad->xdisplay;
   int       rx, ry;
   unsigned int mask;
   int       cx, cy;

   Window root_win = DefaultRootWindow(dpy);
   Window returned_root, returned_child;
   XQueryPointer(dpy, root_win, &returned_root, &returned_child,
              &rx, &ry, &rx, &ry, &mask);
   cx = rx;
   cy = ry;

   int zoom = ad->zoom;

   /* size the capture to whatever the preview box currently is */
   int box_w, box_h;
   evas_object_geometry_get(ad->preview, NULL, NULL, &box_w, &box_h);
   if (box_w <= 0 || box_h <= 0) return ECORE_CALLBACK_RENEW;

   /* real screen pixels needed so the scaled-up result covers the box with no gap (round up) */
   int raw_w = (box_w + zoom - 1) / zoom;
   int raw_h = (box_h + zoom - 1) / zoom;

   int screen_w = DisplayWidth(dpy, DefaultScreen(dpy));
   int screen_h = DisplayHeight(dpy, DefaultScreen(dpy));

   /* a raw region can never be larger than the screen itself */
   if (raw_w > screen_w) raw_w = screen_w;
   if (raw_h > screen_h) raw_h = screen_h;

   int x = cx - raw_w / 2;
   int y = cy - raw_h / 2;
   if (x < 0) x = 0;
   if (y < 0) y = 0;
   if (x + raw_w > screen_w) x = screen_w - raw_w;
   if (y + raw_h > screen_h) y = screen_h - raw_h;

   XImage *img = XGetImage(dpy, root_win, x, y, raw_w, raw_h, AllPlanes, ZPixmap);
   if (!img) return ECORE_CALLBACK_RENEW;

   evas_object_image_size_set(ad->preview, box_w, box_h);
   evas_object_image_fill_set(ad->preview, 0, 0, box_w, box_h);

   unsigned int *pixels = evas_object_image_data_get(ad->preview, EINA_TRUE);
   if (!pixels)
     {
        XDestroyImage(img);
        return ECORE_CALLBACK_RENEW;
     }

   /* nearest-neighbour scale the raw grab up to exactly fill the box */
   for (int py = 0; py < box_h; py++)
     {
        int sy = py / zoom;
        if (sy >= raw_h) sy = raw_h - 1;
        for (int px = 0; px < box_w; px++)
          {
             int sx = px / zoom;
             if (sx >= raw_w) sx = raw_w - 1;
             unsigned long pixel = XGetPixel(img, sx, sy);
             int r = (pixel >> 16) & 0xff;
             int g = (pixel >>  8) & 0xff;
             int b = (pixel      ) & 0xff;
             pixels[py * box_w + px] = (0xff << 24) | (r << 16) | (g << 8) | b;
          }
     }

   unsigned int centre = pixels[(box_h / 2) * box_w + (box_w / 2)];
   update_color_widgets(ad,
                        (centre >> 16) & 0xff,
                        (centre >>  8) & 0xff,
                        (centre      ) & 0xff);

   evas_object_image_data_set(ad->preview, pixels);
   evas_object_image_data_update_add(ad->preview, 0, 0, box_w, box_h);

   XDestroyImage(img);

//

   /* grid overlay is only useful on a zoom factor above 4 */
   if (ad->grid_toggle && elm_check_state_get(ad->grid_toggle) && zoom >= 5)
     {
        Evas_Coord cell = ELM_SCALE_SIZE(zoom);
        evas_object_image_fill_set(ad->grid_overlay, 0, 0, cell, cell);
        evas_object_resize(ad->grid_overlay, box_w, box_h);
        evas_object_show(ad->grid_overlay);
     }
   else
     {
        _grid_overlay_refresh(ad);
     }

//

   return ECORE_CALLBACK_RENEW;
}