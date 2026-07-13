#ifndef CAPTURE_H
#define CAPTURE_H

#include "elicit.h"

Eina_Bool capture_init(App_Data *ad);
Eina_Bool update_capture(void *ad);
void      capture_shutdown(App_Data *ad);
void _grid_overlay_refresh(App_Data *ad);
void on_grid_toggle_change(void *data, Evas_Object *obj, void *event_info);

#endif /* CAPTURE_H */