/************************************************************
    File Name : server.c
    Author: Ginakira
    Mail: ginakira@outlook.com
    Github: https://github.com/Ginakira
    Created Time: 2020/06/02 18:13:42
************************************************************/

#include "../common/color.h"
#include "../common/common.h"
#include "../common/head.h"
#include "../common/udp_server.h"
#include "../game.h"

char *conf = "./server.conf";

struct User *rteam;
struct User *bteam;
int data_port;

pthread_t draw_t;
// struct Map court;

int main(int argc, char **argv) {
    int opt, port = 0, listener, epoll_fd;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-p port]\n", argv[0]);
                exit(1);
        }
    }
    argc -= (optind - 1);
    argv += (optind - 1);

    if (argc > 1) {
        fprintf(stderr, "Usage: %s [-p port]\n", argv[0]);
        exit(1);
    }

    if (!port) port = atoi(get_value(conf, "PORT"));
    data_port = atoi(get_value(conf, "DATAPORT"));

    court.width = atoi(get_value(conf, "COLS"));
    court.height = atoi(get_value(conf, "LINES"));
    court.start.x = 1;
    court.start.y = 1;

    rteam = (struct User *)calloc(MAX, sizeof(struct User));
    bteam = (struct User *)calloc(MAX, sizeof(struct User));

    if ((listener = socket_create_udp(port)) < 0) {
        perror("socket_create_udp");
        exit(1);
    }

    DBG(GREEN "INFO" NONE " : Server start on Port %d\n", port);

    pthread_create(&draw_t, NULL, draw, NULL);

    epoll_fd = epoll_create(MAX * 2);

    if (epoll_fd < 0) {
        perror("epoll_create");
        exit(1);
    }

    struct epoll_event ev, events[MAX * 2];

    ev.events = EPOLLIN;
    ev.data.fd = listener;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listener, &ev);
    struct LogData lg;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    while (1) {
        w_gotoxy_puts(Message, 1, 1, "Waiting for connect...");
        int nfds = epoll_wait(epoll_fd, events, MAX * 2, -1);

        for (int i = 0; i < nfds; ++i) {
            char info[1024];
            recvfrom(events[i].data.fd, (void *)&lg, sizeof(lg), 0,
                     (struct sockaddr *)&client, &len);
            recvfrom(listener, (void *)&lg, sizeof(lg), 0,
                     (struct sockaddr *)&client, &len);
            sprintf(info, "Login: %s:%d\n", inet_ntoa(client.sin_addr),
                    ntohs(client.sin_port));
            w_gotoxy_puts(Message, 1, 2, info);
        }
    }
    return 0;
}