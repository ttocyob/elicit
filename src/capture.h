#ifndef CAPTURE_H
#define CAPTURE_H

#include "elicit.h"

Eina_Bool capture_init(App_Data *ad);
Eina_Bool update_capture(void *ad);
void      capture_shutdown(App_Data *ad);

#endif /* CAPTURE_H */