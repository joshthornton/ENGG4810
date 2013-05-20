import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class Configuration {

	public static final int NUM_BUTTONS = 16;
	
	public ButtonConfiguration[] buttons = new ButtonConfiguration[16];
	public MPCEffect effectOne = MPCEffect.NO_EFFECT;
	public MPCEffect effectTwo = MPCEffect.NO_EFFECT;
	public int bpm = 120;
	
	public void writeStructConfig( File f ) throws IOException
	{
		byte b[] = new byte[524];
		
		// First 3 x 4 bytes are effectOne, effectTwo, bpm
		System.arraycopy(int32ToByteArray( effectOne.getIndex() ), 0, b, 0, 4 );
		System.arraycopy(int32ToByteArray( effectTwo.getIndex() ), 0, b, 4, 4 );
		System.arraycopy(int32ToByteArray( (44100 * 60) / ( bpm * 2) ), 0, b, 8, 4 );
		
		// Copy buttons
		for( int i = 0; i < buttons.length; ++i )
		{
			System.arraycopy( buttonToByteArray( buttons[i], i ), 0, b, 12 + i * 32, 32 );
		}
		
		FileOutputStream fs = new FileOutputStream( f );
		fs.write( b );
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
