public class BitCrusher extends SoftwareEffect {
	
	private int numBits;
	
	public BitCrusher( int numBits ) {
		this.numBits = numBits;
	}

	public byte[] apply(byte[] bytes) {
		
		// Get shorts
		short[] in = toShort(bytes);
		
		// Add clipping
		for( int i = 0; i < in.length; ++i )
			in[i] = (short)((((short)(in[i] << 2) >> 2)&0x7FFF)|(0x8000&in[i]));
		
		// Add bit crushing
		for( int i = 0; i < in.length; ++i )
			in[i] = (short)(((short)in[i] >> (numBits-1)) << (numBits-1));
		
		// Convert back in place
		return fromShort( in );
		
	}

}
