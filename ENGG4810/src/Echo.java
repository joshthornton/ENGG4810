public class Echo extends SoftwareEffect {
	
	private int ms;
	private double amount;
	
	public Echo( int ms, double amount ) {
		this.ms = ms;
		this.amount = amount;
	}

	public byte[] apply(byte[] bytes) {
		
		// Get shorts
		short[] in = toShort(bytes);
		
		// Add delay
		int d = msToFrame(ms);
		for( int i = d; i < in.length; ++i ) {
			int a = in[i];
			int b = (int)(in[i-d] * amount);
			if ( a + b > Short.MAX_VALUE ) {
				in[i] = Short.MAX_VALUE;
			} else if ( a + b < Short.MIN_VALUE ) {
				in[i] = Short.MIN_VALUE;
			} else {
				in[i] = (short)(in[i] + in[i-d] * amount);
			}
		}	
		
		// Convert back in place
		return fromShort( in );
		
	}
	
	private int msToFrame(double ms) {
		return (int)(ms/1000 * 44100);
	}

}
