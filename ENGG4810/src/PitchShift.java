public class PitchShift extends SoftwareEffect {
	
	private int factor;
	
	public PitchShift( int factor ) {
		this.factor = factor;
	}
	
	private short[] down( short[] in, int num ) {
		short[] out = new short[in.length * num];
		
		for( int i = 0; i < in.length; ++i )
		{
			for ( int j = 0; j < num; j++ )
			{
				out[i*num + j] = in[i];
			}
		}
		
		return out;
	}
	
	private short[] up( short[] in, int num ) {
		short[] out = new short[in.length / num];
		
		for( int i = 0; i < out.length; ++i )
		{
			out[i] = in[i*num];
		}
		
		return out;
	}

	public byte[] apply(byte[] bytes) {
		
		// Get shorts
		short[] in = toShort(bytes);
		short[] out;
		if ( factor < 0 )
			out = down( in, -factor );
		else
			out = up( in, factor );
		
		// Convert back in place
		return fromShort( out );
		
	}

}
