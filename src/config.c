#include "config.h"

#include <Eet.h>
#include <Eina.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>   /* mkdir */
#include <errno.h>

/* Eet descriptor */
static Eet_Data_Descriptor *_edd = NULL;

/* initialization function */
static void
_edd_init(void)
{
    if (_edd) return;

    Eet_Data_Descriptor_Class eddc;
    EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elicit_Config);
    _edd = eet_data_descriptor_stream_new(&eddc);

    //EET_DATA_DESCRIPTOR_ADD_BASIC(_edd, Elicit_Config, "x", x, EET_T_INT); // unused
    //EET_DATA_DESCRIPTOR_ADD_BASIC(_edd, Elicit_Config, "y", y, EET_T_INT); // unused
    EET_DATA_DESCRIPTOR_ADD_BASIC(_edd, Elicit_Config, "r", r, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(_edd, Elicit_Config, "g", g, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(_edd, Elicit_Config, "b", b, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(_edd, Elicit_Config, "zoom_factor", zoom_factor, EET_T_INT);
}

static char *
_elicit_config_path(void)
{
    const char *home = getenv("HOME");
    if (!home || !home[0]) return NULL;

    /* $XDG_CONFIG_HOME overrides ~/.config if set */
    const char *xdg = getenv("XDG_CONFIG_HOME");
    char *path = NULL;
    int n;

    if (xdg && xdg[0])
        n = asprintf(&path, "%s/elicit/elicit.eet", xdg);
    else
        n = asprintf(&path, "%s/.config/elicit/elicit.eet", home);

    if (n < 0) return NULL; 

    return path;   /* free()s */
}

static void
_elicit_config_dir(const char *filepath)
{
    char *tmp = strdup(filepath);
    if (!tmp) return;

    /* truncate '/' to get the directory */
    char *slash = strrchr(tmp, '/');
    if (slash) {
        *slash = '\0';
        if (mkdir(tmp, 0700) != 0 && errno != EEXIST)
            fprintf(stderr, "elicit: could not create config dir %s: %s\n",
                    tmp, strerror(errno));
    }
    free(tmp);
}

/* save configuration */
Eina_Bool
elicit_config_save(const Elicit_Config *cfg)
{
    if (!cfg) return EINA_FALSE;

    char *path = _elicit_config_path();
    if (!path) return EINA_FALSE;

    _elicit_config_dir(path);

    eet_init();
    _edd_init();

    Eet_File *ef = eet_open(path, EET_FILE_MODE_WRITE);
    free(path);

    if (!ef) {
        eet_shutdown();
        return EINA_FALSE;
    }

    int ok = eet_data_write(ef, _edd, "geometry", cfg, EINA_TRUE);
    eet_close(ef);
    eet_shutdown();

    return (ok > 0) ? EINA_TRUE : EINA_FALSE;
}

Elicit_Config *
elicit_config_load(void)
{
    Elicit_Config *cfg = calloc(1, sizeof(Elicit_Config));
    if (!cfg) return NULL;

    /* overwrite if file exists */
    //cfg->x = ELICIT_DEFAULT_X;
    //cfg->y = ELICIT_DEFAULT_Y;
    cfg->zoom_factor = ELICIT_DEFAULT_ZOOM;

    char *path = _elicit_config_path();
    if (!path) return cfg;

    eet_init();
    _edd_init();

    Eet_File *ef = eet_open(path, EET_FILE_MODE_READ);
    if (!ef) {
        /* */
        free(path);
        eet_shutdown();
        return cfg;
    }

    Elicit_Config *loaded = eet_data_read(ef, _edd, "geometry");
    eet_close(ef);
    free(path);
    eet_shutdown();

    if (loaded) {
        *cfg = *loaded;
        free(loaded);
    }

    return cfg;
}

void
elicit_config_shutdown(void)
{
   if (!_edd) return;
   eet_data_descriptor_free(_edd);
   _edd = NULL;
}
