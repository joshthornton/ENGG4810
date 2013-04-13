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
		byte b[] = new byte[16780];
		
		// First 3 x 4 bytes are effectOne, effectTwo, bpm
		System.arraycopy(int32ToByteArray( effectOne.getIndex() ), 0, b, 0, 4 );
		System.arraycopy(int32ToByteArray( effectTwo.getIndex() ), 0, b, 4, 4 );
		System.arraycopy(int32ToByteArray( bpm ), 0, b, 8, 4 );
		
		// Copy buttons
		for( int i = 0; i < buttons.length; ++i )
		{
			System.arraycopy( buttonToByteArray( buttons[i] ), 0, b, 12 + i * 1048, 1048 );
		}
		
		FileOutputStream fs = new FileOutputStream( f );
		fs.write( b );
		fs.close();
	}
	
	private static byte[] int32ToByteArray( int i )
	{
		byte[] b = new byte[4];
		
		// Stellaris is Big Endian
		b[3] = (byte)i;
		b[2] = (byte)(i<<8);
		b[1] = (byte)(i<<16);
		b[0] = (byte)(i<<24);
		
		return b;
	}
	
	private static byte[] buttonToByteArray( ButtonConfiguration btn )
	{
		byte[] b = new byte[1048];
		
		// First 3 * 4 bytes are mode, action, loopInterval
		System.arraycopy(int32ToByteArray( btn.mode.getIndex() ), 0, b, 0, 4 );
		System.arraycopy(int32ToByteArray( btn.action.getIndex() ), 0, b, 4, 4 );
		System.arraycopy(int32ToByteArray( btn.loop.getValue() ), 0, b, 8, 4 );
		
		return b;
	}
	
}
