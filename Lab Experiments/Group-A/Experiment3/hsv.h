/*
 * hsv.h
 *
 *  Created on: Feb 10, 2016
 *      Author: jaseem
 */

#ifndef HSV_H_
#define HSV_H_

typedef struct {
    double r;       // percent
    double g;       // percent
    double b;       // percent
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsv;



rgb hsv2rgb(hsv in);



#endif /* HSV_H_ */
