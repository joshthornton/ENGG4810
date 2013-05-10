public class EQ extends SoftwareEffect {
	
	private double[] gains;
	private final static int NUM_BANDS = 10;
	private final static double qualityFactor = 1/Math.sqrt(2);
	private final static int[] bands = { 31, 62, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
	private static double[][] alpha;
	private static double[][] beta;
	
	public EQ( double[] gains ) {
		this.gains = gains;
		
		alpha = new double[NUM_BANDS][3];
		beta = new double[NUM_BANDS][3];
		
		// calculate coefficients
		
		
	}

	public byte[] apply(byte[] bytes) {		
		short[] in = toShort(bytes);
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
            						alpha[band][0] * out[band][(i-1)%3] -
            						alpha[band][1] * out[band][(i-2)%3];
            	combined[i] += out[band][i%3] * gains[band];
            }
            
            // Normalise
            combined[i] += ( in[i] * 0.25 );
            combined[i] *= 4;
        }
        
        short[] shorts = new short[ combined.length ];
        for( int i = 0; i < combined.length; ++i )
        	shorts[i] = (short)combined[i];
        
        return shorts;
        
    }
        

}
