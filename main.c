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

#include "conf.h"
#include "track.h"

void reverse(char s[])
{
    int c, i, j;
    for ( i = 0, j = strlen(s)-1; i < j; i++, j--){
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int n, char s[])
{
    int i, sign;
    if ((sign = n) < 0)
        n =-n;          
    i = 0;
    do { 
        s[i++] = n % 10 + '0';  
    } while ((n /= 10) > 0);  
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void daemonize(const char *cmd);

int init_socket(void)
{
    int sock, msgsock, rval;
    struct sockaddr_un server;
    char buf[1024];
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    
    if (sock < 0) {
        syslog(LOG_INFO, "sock error"); 
        exit(1);
    }

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, "/home/nikolay/C/Daemon/socket");
    if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) {
        syslog(LOG_INFO, "bind error"); 
        exit(1);
    } 

    int lis = listen(sock, 5);
    if (lis == -1)
        syslog(LOG_INFO, "los error"); 

    return sock;
}

int main(void)
{
    conf_init("conf.ini");
    char file[50];
    conf_take_string("First Group", "File path", file);
    daemonize("conf_daemon");


    fwd desc = add_to_watch("/home/nikolay/inotest.txt");
    
    

    int sock = init_socket();
    
    while(1){
        int msgsock = accept(sock, 0, 0);
        if (msgsock == -1)
            syslog(LOG_INFO, "mgssock error");

        file_size("/home/nikolay/inotest.txt", &desc);
        char sz[10];
        itoa(desc.size, sz);
        send(msgsock, sz, strlen(sz), 0);
        send(msgsock, "\n", 2, 0);
        close(msgsock);
    }


    inotify_rm_watch(desc.fd, desc.wd);
    return EXIT_SUCCESS;
}

void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    /*
    * Сбросить маску режима создания файла.
    */
    umask(0);
    /*
    * Получить максимально возможный номер дескриптора файла.
    */
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        perror("невозможно получить максимальный номер дескриптора");
    /*
    * Стать лидером нового сеанса, чтобы утратить управляющий терминал.
    */
    if ((pid = fork()) < 0)
        perror("ошибка вызова функции fork");
    else if (pid != 0) /* родительский процесс */
        exit(0);
    setsid();
    /*
    * Обеспечить невозможность обретения управляющего терминала в будущем.
    */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        perror("невозможно игнорировать сигнал SIGHUP");
    
    if ((pid = fork()) < 0)
        perror("ошибка вызова функции fork");
    else if (pid != 0) /* родительский процесс */
        exit(0);
    /*
    * Назначить корневой каталог текущим рабочим каталогом,
    * чтобы впоследствии можно было отмонтировать файловую систему.
    */
    if (chdir("/") < 0)
        perror("невозможно сделать текущим рабочим каталогом /");
    /*
    * Закрыть все открытые файловые дескрипторы.
    */
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);
    /*
    * Присоединить файловые дескрипторы 0, 1 и 2 к /dev/null.
    */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    /*
    * Инициализировать файл журнала.
    */
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "ошибочные файловые дескрипторы %d %d %d", fd0, fd1, fd2);
    exit(1);
    }
    syslog(LOG_INFO, "File tracking daemon up");
}