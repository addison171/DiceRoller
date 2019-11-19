/*
 * Pi Project 2
 * Addison Kuykendall & Jonathan Zhang
 * 5/15/19
 * A program that uses a Raspberry Pi sense hat to
 * display four dies which are "rolled" when the Pi's
 * accelerometer detects that the Pi has been shaken.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sense/sense.h>
#include <linux/input.h>

int r = 255;
int g = 0;
int b = 0;
int color = 0;

int run = 1; /*Global loop control variable*/
/*interruptHandler will be called when a SIGINT event occurs (i.e. ctrl-c)*/
void interruptHandler() {
  run = 0;
}

void one(pi_framebuffer_t* fb, int quadrant) {
  if(quadrant == 0) {
    fb->bitmap->pixel[1][1] = getColor(r,g,b);
  }
  else if(quadrant == 1) {
    fb->bitmap->pixel[1][6] = getColor(r,g,b);
  }
  else if(quadrant == 2) {
    fb->bitmap->pixel[6][1] = getColor(r,g,b);
  }
  else if(quadrant == 3) {
    fb->bitmap->pixel[6][6] = getColor(r,g,b);
  }
}

void two(pi_framebuffer_t* fb, int quadrant) {
 if(quadrant == 0) {
    fb->bitmap->pixel[0][2] = getColor(r,g,b);
    fb->bitmap->pixel[2][0] = getColor(r,g,b);
  }
  else if(quadrant == 1) {
    fb->bitmap->pixel[0][7] = getColor(r,g,b);
    fb->bitmap->pixel[2][5] = getColor(r,g,b);
  }
  else if(quadrant == 2) {
    fb->bitmap->pixel[5][2] = getColor(r,g,b);
    fb->bitmap->pixel[7][0] = getColor(r,g,b);
  }
  else if(quadrant == 3) {
    fb->bitmap->pixel[5][7] = getColor(r,g,b);
    fb->bitmap->pixel[7][5] = getColor(r,g,b);
  }
}

void three(pi_framebuffer_t* fb, int quadrant) {
  one(fb, quadrant);
  two(fb, quadrant);
}

void four(pi_framebuffer_t* fb, int quadrant) {
 if(quadrant == 0) {
    fb->bitmap->pixel[0][0] = getColor(r,g,b);
    fb->bitmap->pixel[2][0] = getColor(r,g,b);
    fb->bitmap->pixel[2][2] = getColor(r,g,b);
    fb->bitmap->pixel[0][2] = getColor(r,g,b); 
  }
  else if(quadrant == 1) {
    fb->bitmap->pixel[0][5] = getColor(r,g,b);
    fb->bitmap->pixel[0][7] = getColor(r,g,b);
    fb->bitmap->pixel[2][5] = getColor(r,g,b);
    fb->bitmap->pixel[2][7] = getColor(r,g,b);
  }
  else if(quadrant == 2) {
    fb->bitmap->pixel[5][0] = getColor(r,g,b);
    fb->bitmap->pixel[7][0] = getColor(r,g,b);
    fb->bitmap->pixel[5][2] = getColor(r,g,b);
    fb->bitmap->pixel[7][2] = getColor(r,g,b);
  }
  else if(quadrant == 3) {
    fb->bitmap->pixel[5][5] = getColor(r,g,b);
    fb->bitmap->pixel[5][7] = getColor(r,g,b);
    fb->bitmap->pixel[7][5] = getColor(r,g,b);
    fb->bitmap->pixel[7][7] = getColor(r,g,b);
  }
}

void five(pi_framebuffer_t* fb, int quadrant) {
  four(fb, quadrant);
  one(fb, quadrant);
}

void six(pi_framebuffer_t* fb, int quadrant) {
  four(fb, quadrant);
  if(quadrant == 0) {
    fb->bitmap->pixel[0][1] = getColor(r,g,b);
    fb->bitmap->pixel[2][1] = getColor(r,g,b);
  }
  else if(quadrant == 2) {
    fb->bitmap->pixel[5][1] = getColor(r,g,b);
    fb->bitmap->pixel[7][1] = getColor(r,g,b);
  }
  else if(quadrant == 1) {
    fb->bitmap->pixel[0][6] = getColor(r,g,b);
    fb->bitmap->pixel[2][6] = getColor(r,g,b);
  }
  else if(quadrant == 3) {
    fb->bitmap->pixel[5][6] = getColor(r,g,b);
    fb->bitmap->pixel[7][6] = getColor(r,g,b);
  }
}

void diceDisplay(pi_framebuffer_t* fb, int dice[4]) {
  for(int i = 0; i < 4; i++) {
    if(dice[i] == 1) {
      one(fb, i);
    }
    else if(dice[i] == 2) {
      two(fb, i);
    }
    else if(dice[i] == 3) {
      three(fb, i);
    }
    else if(dice[i] == 4) {
      four(fb, i);
    }
    else if(dice[i] == 5) {
      five(fb, i);
    }
    else if(dice[i] == 6) {
      six(fb, i);
    }
  }
}

void changeColor(unsigned int code) {
  if(code == KEY_ENTER) {
    color++;
    
    if(color == 3) {
      color = 0;
    }
    
    if(color == 0) {
      r = 255;
      g = 0;
      b = 0;
    }
    else if(color == 1) {
      r = 0;
      g = 255;
      b = 0;
    }
    else {
      r = 0;
      g = 0;
      b = 255;
    }
  }
}

void main() {
  int dice[4];
  int rolled = 0;
  pi_framebuffer_t* fb = getFBDevice();
  clearBitmap(fb->bitmap,0);

  pi_i2c_t* accel = geti2cDevice();
  configureAccelGyro(accel);
  coordinate_t data;

  pi_joystick_t* joystick = getJoystickDevice();
  
  signal(SIGINT,interruptHandler);
  while(run) {
    pollJoystick(joystick, changeColor, 0);
    if(rolled == 1) {
      diceDisplay(fb, dice);
    }
    getAccelData(accel, &data);
    if(data.x > 2 || data.y > 2 || data.z > 2) {
      rolled = 1;
      clearBitmap(fb->bitmap,0);
      srand(time(0));
      for(int i = 0; i < 4; i++) {
        dice[i] = rand() % 6 + 1;
      }
      diceDisplay(fb, dice);
    }
    data.x = 0;
    data.y = 0;
    data.z = 0;
  }
  printf("\nExiting\n");
  clearBitmap(fb->bitmap,0);
  freeFrameBuffer(fb);
  freei2cDevice(accel);
  freeJoystick(joystick);
}
