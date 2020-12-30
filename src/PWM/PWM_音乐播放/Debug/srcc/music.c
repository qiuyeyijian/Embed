#include "music.h"

const uint32_t tones[8][3] = {{0, 0, 0},      {Do0, Do, Do1}, {Re0, Re, Re1},
                              {Me0, Me, Me1}, {Fa0, Fa, Fa1}, {So0, So, So1},
                              {La0, La, La1}, {Si0, Si, Si1}};

// const uint32_t myth[100][100] = {{6, MS, 4}, {}};

// 根据简谱生成{音调, 低/中/高, 节拍}
// const uint32_t hb[][2] = {{tones[5][0], 600}};

// // 低音 low sound
// const ls[7] = {0, Do0, Re0, Me0, Fa0, So0, La0, Si0};
// // 中音 medium sound
// const ms[7] = {0, Do, Re, Me, Fa, So, La, Si};
// // 高音 high sound
// const hs[7] = {0, Do1, Re1, Me1, Fa1, So1, La1, Si1};

void play(uint32_t tone, uint32_t beat) {
  gBeat = beat;
  pwm_init(PWM_USER, 3, tone, 50, PWM_EDGE, PWM_PLUS);
  while (gBeat > 0) {
    __asm("nop");
  }
}

void playStart() {
  play(tones[1][1], 600);
  play(tones[2][1], 600);
  play(tones[3][1], 600);
  play(tones[4][1], 600);
  play(tones[5][1], 600);
  play(tones[6][1], 600);
  play(tones[7][1], 600);
  play(tones[0][0], 600);
}

void playHappyBirthday() {
  // 1拍600毫秒
  play(So0, 600);
  play(La0, 600);
  play(So0, 600);
  play(Do, 600);
  play(Si0, 1200);
  play(So0, 600);

  play(La0, 600);
  play(So0, 600);
  play(Re, 600);

  play(Do, 1200);
  play(So0, 600);

  play(So, 600);
  play(Me, 600);
  play(Do, 600);

  play(Si0, 600);
  play(La0, 600);
  play(Fa, 600);

  play(Me, 600);
  play(Do, 600);
  play(Re, 600);

  play(Do, 1800);
}

void playMyth() {
  play(tones[1][1], 1800);
  play(tones[3][1], 300);
  play(tones[5][1], 300);

  play(tones[6][1], 1200);
  // play(tones[6][1], 300);
  play(tones[5][1], 600);
  play(tones[2][1], 300);
  play(tones[5][1], 300);

  play(tones[3][1], 300);
  play(tones[1][1], 600);
  // play(tones[1][1], 300);
  play(tones[6][0], 1200);
  // play(tones[6][0], 600);
  // play(tones[6][0], 300);
  play(tones[7][0], 300);

  play(tones[1][1], 600);
  play(tones[3][1], 300);
  play(tones[2][1], 900);
  // play(tones[2][1], 600);
  play(tones[5][1], 300);
  play(tones[3][1], 2400);

  // play(tones[3][1], 1800);
  // play(tones[3][1], 300);
  play(tones[5][1], 300);

  play(tones[6][1], 1200);
  // play(tones[6][1], 300);
  play(tones[5][1], 300);
  play(tones[2][1], 600);
  // play(tones[2][1], 300);
  play(tones[5][1], 300);

  play(tones[3][1], 300);
  play(tones[1][1], 600);
  // play(tones[1][1], 300);
  play(tones[6][0], 900);
  // play(tones[6][0], 600);
  play(tones[3][1], 300);
  play(tones[2][1], 300);

  play(tones[1][1], 600);
  play(tones[3][1], 300);
  play(tones[2][1], 900);
  // play(tones[2][1], 600);
  play(tones[3][1], 300);
  play(tones[7][0], 300);

  play(tones[6][0], 1800);
  play(tones[0][0], 300);
}

void playFarewell() {
  play(tones[5][1], 600);
  play(tones[3][1], 300);
  play(tones[5][1], 300);
  play(tones[1][2], 1200);

  play(tones[6][1], 600);
  play(tones[1][2], 600);
  play(tones[5][1], 1200);

  play(tones[5][1], 600);
  play(tones[1][1], 300);
  play(tones[2][1], 300);
  play(tones[3][1], 600);
  play(tones[2][1], 300);
  play(tones[1][1], 300);

  play(tones[2][1], 1200);
  // play(tones[0][0], 1200);

  play(tones[5][1], 600);
  play(tones[3][1], 300);
  play(tones[5][1], 300);
  play(tones[1][2], 900);
  play(tones[7][1], 300);

  play(tones[6][1], 600);
  play(tones[1][2], 600);
  play(tones[5][1], 1200);

  play(tones[5][1], 600);
  play(tones[2][1], 300);
  play(tones[3][1], 300);
  play(tones[4][1], 900);
  play(tones[7][0], 300);

  play(tones[1][1], 1200);
  // play(tones[0][0], 1200);

  play(tones[6][1], 600);
  play(tones[1][2], 600);
  play(tones[1][2], 1200);

  play(tones[7][1], 600);
  play(tones[6][1], 300);
  play(tones[7][1], 300);
  play(tones[1][2], 1200);

  play(tones[6][1], 300);
  play(tones[7][1], 300);
  play(tones[1][2], 300);
  play(tones[6][1], 600);
  play(tones[5][1], 300);
  play(tones[3][1], 300);
  play(tones[1][1], 300);

  play(tones[2][1], 1200);
  // play(tones[0][0], 1200);

  play(tones[5][1], 600);
  play(tones[3][1], 300);
  play(tones[5][1], 300);
  play(tones[1][2], 900);
  play(tones[7][1], 300);

  play(tones[6][1], 600);
  play(tones[1][2], 600);
  play(tones[5][1], 1200);

  play(tones[5][1], 600);
  play(tones[2][1], 300);
  play(tones[3][1], 300);
  play(tones[4][1], 900);
  play(tones[7][0], 300);

  play(tones[1][1], 1200);
  play(tones[0][0], 1200);
}

void playTenderLines() {
  play(tones[5][0], 1200);

  play(tones[3][1], 900);
  play(tones[2][1], 300);
  play(tones[3][1], 300);
  play(tones[1][1], 900);

  play(tones[2][1], 900);
  play(tones[1][1], 300);
  play(tones[2][1], 600);
  play(tones[5][1], 900);

  play(tones[1][1], 900);
  play(tones[6][0], 300);
  play(tones[1][1], 300);
  play(tones[6][0], 300);
  play(tones[5][0], 300);

  play(tones[2][1], 900);
  play(tones[3][1], 300);
  play(tones[6][0], 300);
  play(tones[5][0], 900);

  play(tones[4][1], 900);
  play(tones[3][1], 300);
  play(tones[4][1], 300);
  play(tones[1][1], 900);

  play(tones[2][1], 900);
  play(tones[1][1], 300);
  play(tones[2][1], 300);
  play(tones[5][1], 900);

  play(tones[1][1], 900);
  play(tones[6][0], 300);
  play(tones[1][1], 300);
  play(tones[6][0], 900);

  play(tones[3][1], 300);
  play(tones[2][1], 600);
  play(tones[1][1], 1500);

  play(tones[0][0], 300);
  play(tones[1][1], 600);
  play(tones[2][1], 300);

  play(tones[3][1], 1200);
  play(tones[0][0], 300);
  play(tones[3][1], 300);
  play(tones[2][1], 300);
  play(tones[1][1], 300);

  play(tones[7][0], 600);
  play(tones[6][1], 300);
  play(tones[5][1], 300);
  play(tones[0][0], 300);
  play(tones[5][1], 300);
  play(tones[6][1], 300);
  play(tones[7][1], 300);

  play(tones[1][2], 600);
  play(tones[3][1], 600);
  play(tones[0][0], 300);
  play(tones[1][2], 300);
  play(tones[7][1], 300);
  play(tones[1][2], 300);

  play(tones[7][1], 600);
  play(tones[3][1], 300);
  play(tones[5][1], 1500);
  play(tones[6][1], 300);

  play(tones[1][2], 300);
  play(tones[5][1], 600);
  play(tones[6][1], 300);
  play(tones[0][0], 300);
  play(tones[6][1], 300);
  play(tones[5][1], 300);
  play(tones[4][1], 300);

  play(tones[5][1], 300);
  play(tones[3][1], 600);
  play(tones[2][1], 300);
  play(tones[1][1], 900);
  play(tones[6][0], 150);
  play(tones[1][1], 150);

  play(tones[3][1], 300);
  play(tones[2][1], 600);
  play(tones[1][1], 300);
  play(tones[2][1], 300);
  play(tones[5][1], 600);
  play(tones[1][1], 300);

  play(tones[2][1], 2400);

  play(tones[5][1], 2400);

  play(tones[0][0], 300);
  play(tones[1][1], 600);
  play(tones[2][1], 300);

  play(tones[3][1], 600);
  play(tones[2][1], 300);
  play(tones[3][1], 300);
  play(tones[0][0], 300);
  play(tones[3][1], 300);
  play(tones[2][1], 300);
  play(tones[1][1], 300);

  play(tones[5][1], 600);
  play(tones[3][1], 300);
  play(tones[5][1], 300);
  play(tones[0][0], 300);
  play(tones[5][1], 300);
  play(tones[6][1], 300);
  play(tones[7][1], 300);

  play(tones[1][2], 1500);
  play(tones[7][1], 300);
  play(tones[6][1], 300);
  play(tones[7][1], 300);

  play(tones[6][1], 300);
  play(tones[3][1], 600);
  play(tones[5][1], 1200);
  play(tones[6][1], 300);

  play(tones[1][2], 300);
  play(tones[5][1], 600);
  play(tones[6][1], 300);
  play(tones[0][0], 300);
  play(tones[6][1], 300);
  play(tones[5][1], 300);
  play(tones[4][1], 300);

  play(tones[5][1], 300);
  play(tones[3][1], 600);
  play(tones[2][1], 300);
  play(tones[1][1], 900);
  play(tones[6][0], 150);
  play(tones[1][1], 150);

  play(tones[3][1], 300);
  play(tones[2][1], 600);
  play(tones[1][1], 300);
  play(tones[2][1], 300);
  play(tones[1][1], 600);
  play(tones[6][0], 300);

  play(tones[1][1], 1800);
  play(tones[0][0], 300);
  play(tones[6][0], 150);
  play(tones[1][1], 150);

  play(tones[3][1], 300);
  play(tones[2][1], 1500);
  play(tones[0][0], 300);
  play(tones[2][1], 150);
  play(tones[1][1], 150);
  play(tones[2][1], 300);
  play(tones[1][1], 1500);
  play(tones[0][0], 300);
  play(tones[6][0], 300);

  play(tones[1][1], 1800);
  play(tones[1][1], 1200);
}
