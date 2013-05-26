import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class Configuration {

	public static final int NUM_BUTTONS = 16;
	
	public ButtonConfiguration[] buttons = new ButtonConfiguration[16];
	public MPCEffect effectOne = MPCEffect.NO_EFFECT;
	public MPCEffect effectTwo = MPCEffect.NO_EFFECT;
	public int bpm = 120;
	
	final static public int[] BOARD_MAPPING = { 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12 };
	//final static private int[] BOARD_MAPPING = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	
	public void readStructure( File file ) throws IOException
	{
		FileInputStream f;
		try {
			f = new FileInputStream( file );
			
			byte[] temp = new byte[4];
			
			// Read effects
			f.read( temp );
			effectOne = MPCEffect.indexToEnum( byteArrayToInt32( temp, 0 ) );
			f.read( temp );
			effectTwo = MPCEffect.indexToEnum( byteArrayToInt32( temp, 0 ) );
			
			// Read BPM
			f.read( temp );
			bpm = (int)(( 44100 * 60 ) / ( byteArrayToInt32( temp, 0 ) * 2 ) + 0.5);
			
			// Read Buttons
			byte[] bTemp = new byte[32];
			for ( int i = 0; i < 16; i++ )
			{
				f.read( bTemp );
				buttons[ BOARD_MAPPING[i] ] = byteArrayToButton( bTemp );
			}
				
			
			
		} catch ( Exception e )
		{
			throw new IOException( e.getMessage() );
		}
		
		
	}
	
	public void writeStructConfig( File f ) throws IOException
	{
		byte b[] = new byte[524];
		
		int effectOneIndex = effectOne.getIndex();
		int effectTwoIndex = effectTwo.getIndex();
		
		if ( effectOneIndex == MPCEffect.NO_EFFECT.getIndex() && effectTwoIndex != MPCEffect.NO_EFFECT.getIndex() )
		{
			effectOneIndex = effectTwoIndex;
			effectTwoIndex = MPCEffect.NO_EFFECT.getIndex();
		}
		
		if ( effectOne == MPCEffect.NO_EFFECT && effectTwo == MPCEffect.NO_EFFECT )
		{
			effectOneIndex = (1<<8);
			effectTwoIndex = (1<<8);
		}
		
		// First 3 x 4 bytes are effectOne, effectTwo, bpm
		System.arraycopy(int32ToByteArray( effectOneIndex ), 0, b, 0, 4 );
		System.arraycopy(int32ToByteArray( effectTwoIndex ), 0, b, 4, 4 );
		System.arraycopy(int32ToByteArray( (44100 * 60) / ( bpm * 2) ), 0, b, 8, 4 );
		
		// Copy buttons
		for( int i = 0; i < buttons.length; ++i )
			System.arraycopy( buttonToByteArray( buttons[ BOARD_MAPPING[i] ], i ), 0, b, 12 + i * 32, 32 );
		
		FileOutputStream fs = new FileOutputStream( f );
		fs.write( b );
		fs.flush();
		fs.getFD().sync();
		fs.close();
	}
	
	private static byte[] int32ToByteArray( int i )
	{
		byte[] b = new byte[4];
		
		// Stellaris is Little Endian
		b[0] = (byte)i;
		b[1] = (byte)(i>>8);
		b[2] = (byte)(i>>16);
		b[3] = (byte)(i>>24);
		
		return b;
	}
	
	private static int byteArrayToInt32( byte[] arr, int offset )
	{
		
		int i = ( ( arr[0 + offset] << 0  ) & 0xFF ) |
				( ( arr[1 + offset] << 8  ) & 0xFF00 ) |
				( ( arr[2 + offset] << 16 ) & 0xFF0000 ) |
				( ( arr[3 + offset] << 24 ) & 0xFF000000 );
		
		return i;
	}
	
	private ButtonConfiguration byteArrayToButton( byte[] arr )
	{
		ButtonConfiguration b = new ButtonConfiguration();
		
		b.action = Action.enumFromIndex( byteArrayToInt32( arr, 8 ) );
		b.loop = LoopInterval.enumFromIndex( byteArrayToInt32( arr, 12 ) );
		b.mode = Mode.enumFromIndex(byteArrayToInt32( arr, 4 ) );
		
		return b;
	}
	
	private byte[] buttonToByteArray( ButtonConfiguration btn, int index )
	{
		byte[] b = new byte[32];
		
		// First 3 * 4 bytes are mode, action, loopInterval
		System.arraycopy(int32ToByteArray( index ), 0, b, 0, 4 );
		System.arraycopy(int32ToByteArray( btn.mode.getIndex() ), 0, b, 4, 4 );
		System.arraycopy(int32ToByteArray( btn.action.getIndex() ), 0, b, 8, 4 );
		System.arraycopy(int32ToByteArray( btn.loop.getValue() ), 0, b, 12, 4 );
		System.arraycopy(int32ToByteArray( 0xFFFFFFFF ), 0, b, 16, 4 );
		System.arraycopy(int32ToByteArray( 0 ), 0, b, 20, 4 );
		System.arraycopy(int32ToByteArray( (int)((((double)44100) * 60) / bpm / btn.loop.getValue())  ), 0, b, 24, 4 );
		
		return b;
	}
	
}
