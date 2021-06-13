#include "track.h"

int take_size(const char *tracking_file)
{
    struct stat buf;
    
    if (stat(tracking_file, &buf)){
        syslog(LOG_ERR, "stat error");
        return -1;
    }
    
    return buf.st_size;
}

fwd add_to_watch(const char *tracking_file)
{
    fwd res;
    res.fd = inotify_init1(IN_NONBLOCK);
    if (res.fd == -1){
        syslog(LOG_ERR, "inotify_init error");
        return res;
    }
    
    res.wd = inotify_add_watch(res.fd, tracking_file, IN_MODIFY);
    if (res.wd == -1){
        syslog(LOG_ERR, "inotify_add_watch error");
        return res;
    }
   
    if ((res.size  = take_size(tracking_file)) == -1)
        syslog(LOG_ERR, "stat error");
    
    return res;
}

int file_size(const char *tracking_file, struct _fwd *file_struct)
{
    struct inotify_event *event;

    ssize_t len;
    char buf[BUFSIZ];
    char *p;

    
    len = read(file_struct->fd, buf, BUFSIZ);
    p = buf;
    event = (struct inotify_event *)p;
    if (event->mask && IN_MODIFY){
        if ((file_struct->size  = take_size(tracking_file)) == -1){
            syslog(LOG_ERR, "stat error");
            return -1;
        }
    }

    event->mask = 0;
    return file_struct->size;
}
