#ifndef TRACK_H
#define TRACK_H
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/inotify.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib/gprintf.h>
#include <syslog.h>

struct _fwd{
    int fd;
    int wd;
    __off_t size;
};

typedef struct _fwd fwd;

fwd add_to_watch(const char *tracking_file);
int file_size(const char *tracking_file, struct _fwd *file_struct);
int take_size(const char *tracking_file);

#endif //TRACK_H