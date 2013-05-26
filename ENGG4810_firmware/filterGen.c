#include "config.h"
#include <math.h>
#include "arm_math.h"

#define Fs 44100

void low_pass_filter(int Fc, float Q, float coeffs[6])
{
    float cut_off = 2*3.14*Fc/Fs;
    float c1 = arm_cos_f32(cut_off);
    float alpha = arm_sin_f32(cut_off) / (2 * Q);

    coeffs[0] = 1 + alpha;
    coeffs[1] = -2 * c1;
    coeffs[2] = 1 - alpha;
    coeffs[3] = (1 - c1)/2;
    coeffs[4] = 1 - c1;
    coeffs[5] = (1-c1)/2;
}

void high_pass_filter(int Fc, float Q, float coeffs[6])
{
    float cut_off = 2*3.14*Fc/Fs;
    float c1 = arm_cos_f32(cut_off);
    float alpha = arm_sin_f32(cut_off) / (2 * Q);

    coeffs[0] = 1 + alpha;
    coeffs[1] = -2 * c1;
    coeffs[2] = 1 - alpha;
    coeffs[3] = (1 + c1)/2;
    coeffs[4] = -1 - c1;
    coeffs[5] = (1 + c1)/2;
}

void bp_pass_filter(int Fc, float Q, float coeffs[6])
{
    float cut_off = 2*3.14*Fc/Fs;
    float c1 = arm_cos_f32(cut_off);
    float alpha = arm_sin_f32(cut_off) / (2 * Q);

    coeffs[0] = 1 + alpha;
    coeffs[1] = -2 * c1;
    coeffs[2] = 1 - alpha;
    coeffs[3] = alpha;
    coeffs[4] = 0;
    coeffs[5] = -alpha;
}

void notch_filter(int Fc, float Q, float coeffs[6])
{
    float cut_off = 2*3.14*Fc/Fs;
    float c1 = arm_cos_f32(cut_off);
    float alpha = arm_sin_f32(cut_off) / (2 * Q);

    coeffs[0] = 1 + alpha;
    coeffs[1] = -2 * c1;
    coeffs[2] = 1 - alpha;
    coeffs[3] = 1;
    coeffs[4] = -2*c1;
    coeffs[5] = 1 - alpha;
}


void generate_filter_coeffs(int type, int Fc, float Q, float a[3], float b[3])
{
    float coeffs[6]; //the array to put our a0-3 & b0-3 into

    switch(type)
    {
        case EFFECT_LOWPASS:
            low_pass_filter(Fc, Q, coeffs);
            break;

        case EFFECT_HIGHPASS:
            high_pass_filter(Fc, Q, coeffs);
            break;

        case EFFECT_BANDPASS:
            bp_pass_filter(Fc, Q, coeffs);
            break;

        case EFFECT_NOTCH:
            notch_filter(Fc, Q, coeffs);
            break;

        default:
        	return;
        	break;
    }

    a[0] = 1;
    a[1] = coeffs[1]/coeffs[0];
    a[2] = coeffs[2]/coeffs[0];

    b[0] = coeffs[3]/coeffs[0];
    b[1] = coeffs[4]/coeffs[0];
    b[2] = coeffs[5]/coeffs[0];

}
