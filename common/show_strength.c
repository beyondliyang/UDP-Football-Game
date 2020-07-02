/************************************************************
    File Name : show_strength.c
    Author: Ginakira
    Mail: ginakira@outlook.com
    Github: https://github.com/Ginakira
    Created Time: 2020/06/30 19:54:35
************************************************************/

#include "head.h"

extern WINDOW *Write;
extern int sockfd;

void show_strength() {
    int mousex = 2, offset = 1;
    int maxx, maxy;
    getmaxyx(Write, maxy, maxx);
    for (int i = 2; i < maxx - 2; ++i) {
        if (i < maxx / 5 || i > maxx - maxx / 5) {
            wattron(Write, COLOR_PAIR(7));
        } else if ((i < (maxx / 5) * 2) || i > (maxx / 5) * 3) {
            wattron(Write, COLOR_PAIR(12));
        } else {
            wattron(Write, COLOR_PAIR(8));
        }
        mvwprintw(Write, 2, i, " ");
    }
    wattron(Write, COLOR_PAIR(3));
    make_nonblock(STDIN_FILENO);
    while (1) {
        int c = getchar();
        if (c != -1) {
            if (c == ' ' || c == 'k') {
                break;
            }
        }
        usleep(5000);
        if (mousex >= maxx - 2) {
            offset = -1;
        } else if (mousex <= 2) {
            offset = 1;
        }
        mvwprintw(Write, 1, mousex, " ");
        mvwprintw(Write, 3, mousex, " ");
        mousex += offset;
        mvwprintw(Write, 1, mousex, "|");
        mvwprintw(Write, 3, mousex, "|");
        mvwprintw(Write, 4, maxx, " ");
        wrefresh(Write);
    }
    int arr[5] = {1, 2, 3, 2, 1};

    struct FootBallMsg msg;
    msg.type = FT_CTL;
    msg.ctl.action = ACTION_KICK;
    msg.ctl.strength = arr[mousex / (maxx / 5)];
    send(sockfd, &msg, sizeof(msg), 0);
    return;
}