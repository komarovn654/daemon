#ifndef CONF_H
#define CONF_H
#include <glib-2.0/glib.h>
#include <glib-2.0/glib/gprintf.h>

void conf_init(const char *conf_file_name);
void conf_take_string(const char *group_name, const char *key_name, char *value);

#endif //CONF_H