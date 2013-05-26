public class Delay extends SoftwareEffect {
	
	private int ms;
	private double amount;
	
	public Delay( int ms, double amount ) {
		this.ms = ms;
		this.amount = amount;
	}

	public byte[] apply(byte[] bytes, int bytesLength) {
		
		// Get shorts
		short[] in = toShort(bytes, bytesLength);
		short[] out = in.clone();
		
		// Add delay
		int d = msToFrame(ms);
		for( int i = d; i < in.length; ++i ) {
			int a = in[i];
			int b = (int)(in[i-d] * amount);
			if ( a + b > Short.MAX_VALUE ) {
				out[i] = Short.MAX_VALUE;
			} else if ( a + b < Short.MIN_VALUE ) {
				out[i] = Short.MIN_VALUE;
			} else {
				out[i] = (short)(in[i] + in[i-d] * amount);
			}
		}	
		
		// Convert back in place
		return fromShort( out );
		
	}
	
	private int msToFrame(double ms) {
		return (int)(ms/1000 * 44100);
	}

}
