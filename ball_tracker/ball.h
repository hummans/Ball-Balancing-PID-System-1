
#ifndef BALL_H
#define BALL_H

typedef struct Ball {
    uint16_t    x[8];
    uint16_t    y[8];
    uint16_t    dx;
    uint16_t    dy;
    uint16_t    fx;
    uint16_t    fy;
    float       phi;
    float       v;
}Ball;

#endif
