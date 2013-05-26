public class Decimator extends SoftwareEffect {
	
	private int numSamples;
	
	public Decimator( int numSamples ) {
		this.numSamples = numSamples;
	}

	public byte[] apply(byte[] bytes, int bytesLength ) {
		
		// Get shorts
		short[] in = toShort(bytes, bytesLength);
		short[] out = new short[in.length];
		
		for( int i = 0; i < out.length - numSamples; i+=numSamples )
		{
			double gradient = (in[i+numSamples] - in[i])/numSamples;
			for ( int j = 0; j < numSamples; j++ )
			{
				out[i+j] = (short)(in[i] + gradient * numSamples );
			}
		}
		
		// Convert back in place
		return fromShort( out );
		
	}

}
