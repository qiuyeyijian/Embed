#include "includes.h"
#include "mcu.h"
#include "printf.h"

// 低音
#define Do0 5753
#define Re0 5126
#define Me0 4566
#define Fa0 4318
#define So0 3844
#define La0 3425
#define Si0 3050

// 中音
#define Do 2882
#define Re 2567
#define Me 2287
#define Fa 2159
#define So 1922
#define La 1713
#define Si 1525

// 高音
#define Do1 1441
#define Re1 1283
#define Me1 1143
#define Fa1 1079
#define So1 961
#define La1 856
#define Si1 763

#define LS (0)  // 低音
#define MS (1)  // 中音
#define HS (2)  // 高音

// // 一个节拍所用时间
#define bt0 600
#define bt1 780

void play(uint32_t tone, uint32_t beat);

void playStart();

void playHappyBirthday();

void playFarewell();

void playMyth();

void playTenderLines();