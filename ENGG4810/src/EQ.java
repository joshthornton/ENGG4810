public class EQ extends SoftwareEffect {
	
	private class IIRCoefficients {
	    public double beta;
	    public double alpha;
	    public double gamma;

	    public IIRCoefficients(double beta, double alpha, double gamma) {
	        this.beta = beta;
	        this.alpha = alpha;
	        this.gamma = gamma;
	    }
	}
	private double[] gains;
	private final static int NUM_BANDS = 10;
	private IIRCoefficients coefficients[] = {
        /* 31 Hz*/
        new IIRCoefficients(9.9688176273e-01, 1.5591186337e-03, 1.9968622855e+00),
        /* 62 Hz*/
        new IIRCoefficients(9.9377323686e-01, 3.1133815717e-03, 1.9936954495e+00),
        /* 125 Hz*/
        new IIRCoefficients(9.8748575691e-01, 6.2571215431e-03, 1.9871705722e+00),
        /* 250 Hz*/
        new IIRCoefficients(9.7512812040e-01, 1.2435939802e-02, 1.9738753198e+00),
        /* 500 Hz*/
        new IIRCoefficients(9.5087485437e-01, 2.4562572817e-02, 1.9459267562e+00),
        /* 1k Hz*/
        new IIRCoefficients(9.0416308662e-01, 4.7918456688e-02, 1.8848691023e+00),
        /* 2k Hz*/
        new IIRCoefficients(8.1751373987e-01, 9.1243130064e-02, 1.7442229115e+00),
        /* 4k Hz*/
        new IIRCoefficients(6.6840529852e-01, 1.6579735074e-01, 1.4047189863e+00),
        /* 8k Hz*/
        new IIRCoefficients(4.4858358977e-01, 2.7570820511e-01, 6.0517475334e-01),
        /* 16k Hz*/
        new IIRCoefficients(2.4198119087e-01, 3.7900940457e-01, -8.0845085113e-01)
};
	
	public EQ( double[] gains ) {
		this.gains = gains;
	}

	public byte[] apply(byte[] bytes) {
		
		short[] in = toShort(bytes);
		short[] out = iir( in );
		return fromShort( out );
		
	}
	
	public short[] iir( short[] in ) {
        double[][] temp = new double[NUM_BANDS][3];
        double[] out = new double[in.length];

        out[0] = in[0];
        out[1] = in[1];
        for ( int i = 2; i < in.length; i++ ) {
            for ( int j = 0; j < NUM_BANDS; ++j ) {
            	temp[j][i%3] =	coefficients[j].alpha * ( in[i] - in[i-2] ) +
            				coefficients[j].gamma * temp[j][(i-1)%3] -
            				coefficients[j].beta * temp[j][(i-2)%3];
            	out[i] += temp[j][i%3] * gains[j];
            }
            
            // Normalise
            out[i] += ( in[i] * 0.2 );
            out[i] *= 5;
        }
        
        short[] shorts = new short[ out.length ];
        for( int i = 0; i < out.length; ++i )
        	shorts[i] = (short)out[i];
        
        return shorts;
        
    }
        

}
