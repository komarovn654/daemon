#include "conf.h"

static GError *error = NULL;
static GKeyFile *key_file;

void conf_init(const char *conf_file_name)
{
    key_file = g_key_file_new (); 

    if (!(g_key_file_load_from_file(key_file, conf_file_name, G_KEY_FILE_NONE, &error))){
        if (!g_error_matches (error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
            g_warning ("Error loading key file: %s", error->message);
        exit(1);
    }   
}

void conf_take_string(const char *group_name, const char *key_name, char *value)
{
    g_autofree gchar *res = g_key_file_get_string(key_file, group_name, key_name, &error);
    if (res == NULL && !g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND)){
        g_warning ("Error finding key in key file: %s", error->message);
    exit(1);
    }

    strcpy(value, res);
}
 