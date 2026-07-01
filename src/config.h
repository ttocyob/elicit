#ifndef ELICIT_CONFIG_H
#define ELICIT_CONFIG_H

#include <Eina.h>

/* app state stored to ~/.config/elicit/elicit.eet */
typedef struct _Elicit_Config {
    //int  x, y;      // unused  
    int  r, g, b;     // rgb values
    int  zoom_factor; // spinner value

} Elicit_Config;

/* when no config file exists */
#define ELICIT_DEFAULT_X      0 // unused
#define ELICIT_DEFAULT_Y      0 // unused
#define ELICIT_DEFAULT_ZOOM   6

Elicit_Config *elicit_config_load(void);

Eina_Bool elicit_config_save(const Elicit_Config *cfg);

void elicit_config_shutdown(void);

#endif /* ELICIT_CONFIG_H */
