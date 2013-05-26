
public abstract class SoftwareEffect {
	
	public abstract byte[] apply( byte[] bytes, int bytesLength );
	
	protected short[] toShort( byte[] bytes, int bytesLength ) {
		short[] shorts = new short[bytesLength / 2];
		for( int i = 0; i < shorts.length; ++i )
		{
			short lower = (short)(((short)bytes[i*2])&0x00FF);
			short higher = (short)((((short)bytes[i*2+1])<<8)&0xFF00);
			shorts[i] = (short)(lower|higher);
		}
		return shorts;
	}
	
	protected byte[] fromShort( short[] shorts ) {
		byte[] bytes = new byte[shorts.length * 2];
		for ( int i = 0; i < shorts.length; ++i )
		{
			bytes[i*2] = (byte)(shorts[i]);
			bytes[i*2+1] = (byte)(shorts[i]>>8);
		}
		return bytes;
	}
	
}
