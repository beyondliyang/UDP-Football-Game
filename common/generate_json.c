/************************************************************
    File Name : generate_json.c
    Author: Ginakira
    Mail: ginakira@outlook.com
    Github: https://github.com/Ginakira
    Created Time: 2020/07/05 15:10:25
************************************************************/
#include <cjson/cJSON.h>

#include "datatype.h"
#include "game.h"
#include "head.h"
#include "thread_pool.h"

extern struct User *rteam, *bteam;
extern struct Bpoint ball;
extern struct BallStatus ball_status;

void* generate_court_json(void* arg);
void* generate_score_json(void* arg);
cJSON* generate_player_json(struct User* user);

// 发送球场画面json
void send_court_json() {
    pthread_t tid;
    pthread_create(&tid, NULL, generate_court_json, NULL);
    return;
}

// 发送得分信息json
void send_score_json() {
    pthread_t tid;
    pthread_create(&tid, NULL, generate_score_json, NULL);
    return;
}

// 生成球场画面json
void* generate_court_json(void* arg) {
    pthread_t tid = pthread_self();
    pthread_detach(tid);
    // 声明最终发送的json对象
    cJSON* court_json = cJSON_CreateObject();
    if (court_json == NULL) return NULL;

    // 红队数组
    cJSON* red = cJSON_AddArrayToObject(court_json, "red");
    if (red == NULL) return NULL;
    for (int i = 0; i < MAX; ++i) {
        if (!rteam[i].online) continue;
        cJSON* player = generate_player_json(&rteam[i]);
        if (player == NULL) return NULL;
        cJSON_AddItemToArray(red, player);
    }

    // 蓝队数组
    cJSON* blue = cJSON_AddArrayToObject(court_json, "blue");
    if (blue == NULL) return NULL;
    for (int i = 0; i < MAX; ++i) {
        if (!bteam[i].online) continue;
        cJSON* player = generate_player_json(&bteam[i]);
        if (player == NULL) return NULL;
        cJSON_AddItemToArray(red, player);
    }

    // 球
    cJSON* ball_obj = cJSON_AddObjectToObject(court_json, "ball");
    if (ball_obj == NULL) return NULL;
    // 是否正被带球
    cJSON* is_carry =
        cJSON_AddNumberToObject(ball_obj, "is_carry", ball_status.is_carry);
    if (is_carry == NULL) return NULL;
    // 哪队带球
    cJSON* who = cJSON_AddNumberToObject(ball_obj, "who", ball_status.who);
    if (who == NULL) return NULL;
    // 带球者
    cJSON* name = cJSON_AddStringToObject(ball_obj, "name", ball_status.name);
    if (name == NULL) return NULL;
    // 球坐标
    cJSON* x = cJSON_AddNumberToObject(ball_obj, "x", ball.x);
    cJSON* y = cJSON_AddNumberToObject(ball_obj, "y", ball.y);
    if (x == NULL || y == NULL) return NULL;

    // 广播给客户端
    struct FootBallMsg msg;
    bzero(&msg, sizeof(msg));
    msg.type = FT_GAME;
    strcpy(msg.msg, cJSON_Print(court_json));
    DBG(L_BLUE "Sended court json:%s" NONE "\n", msg.msg);

    send_all(msg);
    return NULL;
}

// 生成得分信息json
void* generate_score_json(void* arg) {
    pthread_t tid = pthread_self();
    pthread_detach(tid);
    return NULL;
}

// 工具函数 - 传入一个User 生成一个cjson对象并返回
cJSON* generate_player_json(struct User* user) {
    cJSON* player = cJSON_CreateObject();
    if (player == NULL) return NULL;

    // 玩家名
    cJSON* name = cJSON_AddStringToObject(player, "name", user->name);
    if (name == NULL) return NULL;
    // x坐标
    cJSON* x = cJSON_AddNumberToObject(player, "x", user->loc.x);
    if (x == NULL) return NULL;
    // y坐标
    cJSON* y = cJSON_AddNumberToObject(player, "y", user->loc.y);
    if (y == NULL) return NULL;

    return player;
}