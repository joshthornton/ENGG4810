public class BitCrusher extends SoftwareEffect {
	
	private int num;
	
	public BitCrusher( int num ) {
		this.num = num;
	}

	public byte[] apply(byte[] bytes, int bytesLength ) {
		
		// Get shorts
		short[] in = toShort(bytes, bytesLength);
		
		// Add bit crushing
		for( int i = 0; i < in.length; ++i )
			in[i] = (short)( (in[i] / num) * num );
		
		// Convert back in place
		return fromShort( in );
		
	}

}
