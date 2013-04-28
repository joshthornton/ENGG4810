#define LPF 0
#define HPF 1
#define BPF 3
#define NOTCH 4

#define Fs 44100
#define NUM_COEFFS


float a[3];
float b[3];
 
/* Could be worth precomputing values for c1 but with 128 levels at 32 bits each
 * it'll be 512 bytes. Not sure if we can easily store them to flash or somewhere if they're
 * constant.
 *
 * Don't think it's worth precomputing alpha as there are 128^2 values for it.
 * sin(cut_off) might be worth computing if it's slow to do sin though. */

void low_pass_filter(int Fc, float Q, float coeffs[NUM_COEFFS])
{
    float cut_off = 2*3.14*Fc/Fs;
    float c1 = cos(cut_off);
    float alpha = sin(cut_off) / (2 * Q);

    coeffs[0] = 1 + alpha;
    coeffs[1] = -2 * c1;
    coeffs[2] = 1 - alpha; 
    coeffs[3] = (1 - c1)/2;
    coeffs[4] = 1 - c1;
    coeffs[5] = (1-c1)/2;
}

void high_pass_filter(int Fc, float Q, float coeffs[NUM_COEFFS])
{
    float cut_off = 2*3.14*Fc/Fs;
    float c1 = cos(cut_off);
    float alpha = sin(cut_off) / (2 * Q);

    coeffs[0] = 1 + alpha;
    coeffs[1] = -2 * c1;
    coeffs[2] = 1 - alpha;
    coeffs[3] = (1 + c1)/2;
    coeffs[4] = -1 - c1;
    coeffs[5] = (1 + c1)/2;
}

void bp_pass_filter(int Fc, float Q, float coeffs[NUM_COEFFS])
{
    float cut_off = 2*3.14*Fc/Fs;
    float c1 = cos(cut_off);
    float alpha = sin(cut_off) / (2 * Q);

    coeffs[0] = 1 + alpha;
    coeffs[1] = -2 * c1;
    coeffs[2] = 1 - alpha;
    coeffs[3] = alpha;
    coeffs[4] = 0;
    coeffs[5] = -alpha;
}

void notch_filter(int Fc, float Q, float coeffs[NUM_COEFFS])
{
    float cut_off = 2*3.14*Fc/Fs;
    float c1 = cos(cut_off);
    float alpha = sin(cut_off) / (2 * Q);
    
    coeffs[0] = 1 + alpha;
    coeffs[1] = -2 * c1;
    coeffs[2] = 1 - alpha;
    coeffs[3] = 1;
    coeffs[4] = -2*c1;
    coeffs[5] = 1 - alpha;
}


void generate_filter_coeffs(int type, int Fc, float Q)
{
    float coeffs[NUM_COEFFS]; //the array to put our a0-3 & b0-3 into
    
    switch(type)
    {
        case LPF:
            low_pass_filter(Fc, Q, coeffs[NUM_COEFFS]);
            break;
        
        case HPF:
            high_pass_filter(Fc, Q, coeffs[NUM_COEFFS]);
            break;
       
        case BPF:
            bp_pass_filter(Fc, Q, coeffs[NUM_COEFFS]);
            break;
        
        case NOTCH:
            notch_filter(Fc, Q, coeffs[NUM_COEFFS]);
            break;
            
        default:
    }
    
    a[0] = 1; 
    a[1] = coeffs[1]/coeffs[0];
    a[2] = coeffs[2]/coeffs[0];
    
    b[0] = coeffs[3]/coeffs[0];
    b[1] = coeffs[4]/coeffs[0];
    b[2] = coeffs[5]/coeffs[0];

}

// Not sure how we should implement this but this is the difference equation
void freq_filter()
{
    out[n] = b[0]*in[i] + b[1]*in[i-1] + b[2]*in[i-2] - a[1]*out[i-1] - a[2]*out[i-2];
    
    //potentially can use
    //void arm_iir_lattice_f32(const arm_iir_lattice_instance_f32 * S, float32_t * pSrc, float32_t * pDst, uint32_t blockSize )
    //void arm_iir_lattice_init_f32((	arm_iir_lattice_instance_f32 * 	S,uint16_t 	numStages,float32_t * 	pkCoeffs,float32_t * 	pvCoeffs,float32_t * 	pState, uint32_t 	blockSize 
    
    
    //or could do something like this
    float prevIn = 0;
    float prevIn2 = 0;
    
    float prevOut = 0;
    float prevOut2 = 0;

    float in = 0;
    for (i = 0; i < block_length; i++)
    {
        in = input[i];
        
        out[i] = b[0]*in + b[1]*prevIn + b[2]*prevIn2 - a[1]*prevOut - a[2]*prevOut2;
        
        prevIn2 = prevIn;
        prevIn = in;
        
        prevOut2 = prevOut;
        prevOuut = out[i]; 
    
    } 
}

void echo_filter(int a, int d)
{
    out[n] = in[n] + a * in[n-d];
}

void delay_filter(int a, int d)
{
    out[n] = i[n] + a * out[n-d]
}

//not sure if i've understood this one properly
void bitwise_ko(int and, int xor)
{
    out[n] = (in[n] & and) ^ xor;
}

void decimate_crush(int decimate, int crush)
{
    out[n] = linear interp
    
    out[n] = (out[n]/crush)*crush;

}
