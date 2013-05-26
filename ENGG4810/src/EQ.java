public class EQ extends SoftwareEffect {
	
	private double[] gains;
	private final static int NUM_BANDS = 10;
	private final static int SAMPLE_RATE = 44100;
	private final static double qualityFactor = 1/Math.sqrt(2);
	private final static double[] bands = { 31, 62, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
	private double[][] alpha;
	private double[][] beta;
	
	public EQ( double[] gains ) {
		this.gains = gains;
		
		alpha = new double[NUM_BANDS][3];
		beta = new double[NUM_BANDS][3];
		
		// calculate coefficients
		for ( int b = 0; b < NUM_BANDS; ++b )
		{
			double cutoff = 2 * Math.PI * ( bands[b] / SAMPLE_RATE);
			double c = Math.cos( cutoff );
			double a = Math.sin( cutoff ) / (2 * qualityFactor );
			
			alpha[b][0] = 1;
			alpha[b][1] = ( -2 * c ) / ( 1 + a );
			alpha[b][2] = ( 1 - a ) / ( 1 + a );
			
			beta[b][0] = ( a ) / ( 1 + a );
			beta[b][1] = ( 0 ) / ( 1 + a );
			beta[b][2] = ( -a ) / ( 1 + a );
		}
		
	}

	public byte[] apply(byte[] bytes, int bytesLength) {		
		short[] in = toShort(bytes, bytesLength);
		short[] out = iir( in );
		return fromShort( out );
	}
	
	public short[] iir( short[] in ) {
        double[][] out = new double[NUM_BANDS][3];
        double[] combined = new double[in.length];

        combined[0] = in[0];
        combined[1] = in[1];
        for ( int i = 2; i < in.length; i++ ) {
            for ( int band = 0; band < NUM_BANDS; ++band ) {
            	out[band][i%3] =	beta[band][0] * in[i] +
            						beta[band][1] * in[i-1] +
            						beta[band][2] * in[i-2] -
            						alpha[band][1] * out[band][(i-1)%3] -
            						alpha[band][2] * out[band][(i-2)%3];
            	combined[i] += out[band][i%3] * gains[band] /*/ 2*/;
            }
            
            // Normalise
            //combined[i] += ( in[i] * 0.5 );
            //combined[i] *= 2;
        }
        
        short[] shorts = new short[ combined.length ];
        for( int i = 0; i < combined.length; ++i )
        {
        	if ( combined[i] > Short.MAX_VALUE ) {
        		shorts[i] = Short.MAX_VALUE;
        	} else if ( combined[i] < Short.MIN_VALUE ) {
        		shorts[i] = Short.MIN_VALUE;
        	} else {
        		shorts[i] = (short)combined[i];
        	}
        }
        
        return shorts;
        
    }
        

}
