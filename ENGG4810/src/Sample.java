import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioFormat.Encoding;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;

import org.tritonus.sampled.convert.SampleRateConversionProvider;


public class Sample {
	
	private AudioFormat format;
	private byte[] bytes;
	//private Clip clip;
	private SourceDataLine line;
	private static final int FRAME_SIZE = 2;
	private Thread lineThread;
	private Thread playheadThread;
	private int offset;
	private long oldFramePosition;
	private volatile boolean active;
	private boolean playing;
	private boolean paused;
	private int bytesLength;
	
	
	private Waveform canvas;
	public enum SampleType {
		WAV,
		MP3,
		DAT
	}
	
	public Sample( File file, Waveform canvas, SampleType type ) throws InvalidFileException {
		this.canvas = canvas;
		AudioFormat.Encoding encoding = AudioFormat.Encoding.PCM_SIGNED;
		float sampleRate = 44100;
		int sampleSizeInBits = 16;
		int channels = 1;
		int frameSize = FRAME_SIZE;
		float frameRate = sampleRate;
		boolean bigEndian = false;
		format = new AudioFormat(encoding, sampleRate, sampleSizeInBits, channels, frameSize, frameRate, bigEndian);
		
		if ( type == SampleType.WAV )
			loadWav( file );
		else if ( type == SampleType.MP3 )
			loadMP3( file );
		else if ( type == SampleType.DAT )
			loadDat( file );
		
	}
	
	private void loadDat( File file ) throws InvalidFileException
	{
		try {
			FileInputStream f = new FileInputStream( file );
			bytes = new byte[ f.available() ];
			f.read( bytes );
			load( bytes, 1 );
			f.close();
		} catch( Exception e ) {
			throw new InvalidFileException( e );
		}
	}
	
	private void loadWav( File file ) throws InvalidFileException {
		
		AudioInputStream stream;
		SampleRateConversionProvider.SampleRateConverterStream converter;
		try {
			stream = AudioSystem.getAudioInputStream( file );
			AudioFormat.Encoding encoding = AudioFormat.Encoding.PCM_SIGNED;
			float sampleRate = 44100;
			int sampleSizeInBits = 16;
			int channels = stream.getFormat().getChannels();
			int frameSize = FRAME_SIZE * channels;
			float frameRate = sampleRate;
			boolean bigEndian = false;
			long frameLength;
			
			if ( !stream.getFormat().getEncoding().equals( Encoding.PCM_SIGNED ) && !stream.getFormat().getEncoding().equals( Encoding.PCM_UNSIGNED )  )
			{
				throw new Exception( "Floating Point" );
			}
			
			if ( stream.getFormat().getSampleRate() != 44100 && !stream.getFormat().isBigEndian() )
			{
				AudioFormat intermediate = new AudioFormat(encoding, sampleRate, sampleSizeInBits, channels, frameSize, frameRate, bigEndian);
				converter = new SampleRateConversionProvider.SampleRateConverterStream( stream, intermediate );
				frameLength = converter.getFrameLength();
				frameSize = converter.getFrameSize();
				bytes = new byte[(int)(frameLength * frameSize)];
				converter.read( bytes );
				load( bytes, channels );
			} else {
				frameLength = stream.getFrameLength();
				frameSize = stream.getFormat().getFrameSize();
				bytes = new byte[(int)(frameLength * frameSize)];
				stream.read( bytes );
				load( bytes, channels );
			}
			
			
		} catch ( Exception e ) {
			
			try {
				
				// Try and parse
				FileInputStream s = new FileInputStream( file );
				s.skip( 20 );
				
				byte[] format = new byte[2];
				byte[] channels = new byte[2];
				byte[] sampleRate = new byte[4];
				byte[] frameSize = new byte[2];
				byte[] sampleSizeInBits = new byte[2];
				
				s.read( format );
				s.read( channels );
				s.read( sampleRate );
				s.skip( 4 );
				s.read( frameSize );
				s.read( sampleSizeInBits );
				
				int f = (format[1]<<8) | (format[0]);
				int c = (channels[1]<<8) | (channels[0]);
				//int sR = ((sampleRate[3]<<24)&0xFF000000) | ((sampleRate[2]<<16)&0x00FF0000) | ((sampleRate[1]<<8)&0x0000FF00) | ((sampleRate[0])&0x000000FF);
				int fS = (frameSize[1]<<8) | (frameSize[0]);
				int sS = (sampleSizeInBits[1]<<8) | (sampleSizeInBits[0]);
				
				// Find data
				byte[] temp = new byte[4];
				do {
					s.read( temp );
				} while ( !( ((char)temp[0]) == 'd' && ((char)temp[1]) == 'a' && ((char)temp[2]) == 't' && ((char)temp[3]) == 'a' ) );
				
				byte[] chunkSize = new byte[4];
				s.read( chunkSize );
				//int cS = ((chunkSize[3]<<24)&0xFF000000) | ((chunkSize[2]<<16)&0x00FF0000) | ((chunkSize[1]<<8)&0x0000FF00) | ((chunkSize[0])&0x000000FF);
				
				if ( f == 3 ) // Float
				{
					// Get file size
					int total = s.available();
					byte[] out = new byte[ total ];
					
					// Read in
					s.read( out );
					
					// Convert
					int framesPerSample = sS / fS;
					int numSamples = total / framesPerSample / c;
					int offset = 0;
					
					int[] littleMasks = { 0x00000000, 0x0000FF00, 0x00FFFF00, 0xFFFFFF00 };
					int[] bigMasks = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
					
					for( int i = 0; i < numSamples; i++ )
					{
						
						int little = (int)(0x00000000);
						int big = (int)(0x00000000);
						for ( int k = 0; k < c; k++ )
						{
							
							
							if ( k == 0 )
							{
								for( int j = 0; j < framesPerSample; j++ )
								{
									little = ( ((little<<8)&littleMasks[j])| ((out[i * framesPerSample * c + j]) & 0x000000FF ) );
									big = ( big | ((out[i * framesPerSample * c + j + offset] << ( j * 8 ) ) & bigMasks[ j ] ) );
								}
							}
						}
						
						// Convert int bytes to float
						//float flLittle = Float.intBitsToFloat( little );
						float flBig = Float.intBitsToFloat( big );
						short shrt = (short)(flBig * 32768);
						//short shrt = (short)(flLittle * 32768);
						out[i*2] = (byte)shrt;
						out[i*2 + 1] = (byte)(shrt>>8);
					}
					
					bytes = new byte[ numSamples * 2 ];
					System.arraycopy( out, 0, bytes, 0, bytes.length );
					load( bytes, 1 );
					s.close();
					
				}
				
				
				
			} catch ( Exception ex )
			{
				throw new InvalidFileException( e );
			}
		}
		
	}
	
	private void loadMP3( File file ) {
		AudioInputStream in, din;
		try {
			in = AudioSystem.getAudioInputStream(file);
			din = null;
			AudioFormat baseFormat = in.getFormat();
			AudioFormat decodedFormat = new AudioFormat(AudioFormat.Encoding.PCM_SIGNED,
				baseFormat.getSampleRate(),
				16,
				baseFormat.getChannels(),
				baseFormat.getChannels() * 2,
				baseFormat.getSampleRate(),
				false
			);
			din = AudioSystem.getAudioInputStream(decodedFormat, in);
			File f = File.createTempFile("temp", ".wav" );
			AudioSystem.write(din, AudioFileFormat.Type.WAVE, f);
			loadWav( f );
			
		} catch ( Exception e ) {
			e.printStackTrace();
			throw new InvalidFileException( e );
		}
	}
	
	private void load( byte[] bytes, int numChannels ) throws LineUnavailableException
	{
		// Delete extra channels
		bytesLength = bytes.length / numChannels;
		for( int i = 0; i < bytesLength; ) {
			for( int frame = 0; frame < FRAME_SIZE; frame++ ) {
				bytes[i + frame] = bytes[(i*numChannels)+frame];
			}
			i += FRAME_SIZE;
		}
		setup();
	}
	
	public byte[] getByteArray() {
		return bytes;
	}
	
	public AudioFormat getFormat() {
		return format;
	}
	
	private void updateCanvas() {
		canvas.load( bytes, bytesLength );
		canvas.repaint();
	}
	
	public void playPause() {
		if ( paused ) {
			line.start();
			paused = false;
			System.out.println( "play" );
		} else if ( active ) {
			pause();
		} else {
			play();
			System.out.println( "pause" );
		}
	}
	
	public void playhead() {
		canvas.setPlayhead( ((double)(line.getFramePosition() - oldFramePosition)) / (bytesLength / 2) );
		canvas.repaint();
	}
	
	// opens line
	// updates canvas
	private void setup()
	{
		stopThreads();
		DataLine.Info info = new DataLine.Info(SourceDataLine.class, format);
		try {
			line = (SourceDataLine)AudioSystem.getLine( info );
		} catch (LineUnavailableException e) {
			e.printStackTrace();
		}
		try {
			line.open( format );
		} catch (LineUnavailableException e) {
			e.printStackTrace();
		}
		offset = 0;
		oldFramePosition = 0;
		updateCanvas();
	}
	
	
	// Starts line
	public void play() {
		
		if ( playing && !active ) {
			oldFramePosition = line.getFramePosition();
			offset = 0;
		}
		
		stopThreads();
		
		playing = true;
		paused = false;
		active = true;
		line.flush();
		line.start();
		
		System.out.println( "offset: " + offset );
		
		playheadThread = new Thread(new Runnable() {
			public void run() {
				
				while( active )
				{
					playhead();
				}
				playhead();
			}
		});
		playheadThread.setDaemon(true);
		playheadThread.start();
		
		lineThread = new Thread(new Runnable() {
			public void run() {
				line.write(bytes, offset, bytesLength - offset );
				line.drain();
				active = false;
			}
		});
        lineThread.setDaemon(true);
        lineThread.start();
	}
	
	public void pause() {
		paused = true;
		line.stop();
		offset = line.getFramePosition() * 2;
	}
	
	public void back() {
		paused = false;
		line.stop();
		line.flush();
		oldFramePosition = line.getFramePosition();
		offset = 0;
		if ( playing )
			play();
		playhead();
	}
	
	public void setPosition( double pos )
	{
		line.stop();
		line.flush();
		playing = active && !paused;
		paused = false;
		offset = (int)((bytesLength / 2 ) * pos) * 2;
		oldFramePosition = line.getFramePosition() - ( offset / 2 );
		playhead();
		if ( playing )
			play();
	}
	
	private void stopThreads() {
		if (lineThread != null) {
			try {
				lineThread.interrupt();
			    lineThread.join();
			    lineThread = null;
			} catch(InterruptedException wakeAndContinue) {
			}
		}
		if (playheadThread != null) {
			try {
				playheadThread.interrupt();
				playheadThread.join();
				playheadThread = null;
			} catch(InterruptedException wakeAndContinue) {
			}
		}
	}
	
	public void applyEffect( SoftwareEffect effect ) {
		if ( bytesLength >= 2 * 10 * 44100 )
		{
			throw new RuntimeException( "Please trim track to 10 seconds or less before editing." );
		}
		bytes = effect.apply( bytes, bytesLength );
		bytesLength = bytes.length;
		setup();
	}
	
	public void cutLeft() {
		int start = (int)( line.getFramePosition() - oldFramePosition );
		int end = bytesLength / 2;
		trim( start, end );
		setup();
	}
	
	public void cutRight() {
		int start = 0;
		int end = (int)( line.getFramePosition() - oldFramePosition );
		trim( start, end );
		setup();
	}
	
	private void trim( int start, int end ) {
		start *= FRAME_SIZE;
		end *= FRAME_SIZE;
		bytesLength = end - start;
		for ( int i = 0; i < end - start; i++ )
		{
			bytes[i] = bytes[i + start];
		}
	}
	
	public void writeData( File f ) throws IOException
	{
		/*for ( int i = 0; i < bytesLength; i += 2 )
		{
			short lower = (short)(((short)bytes[i])&0x00FF);
			short higher = (short)((((short)bytes[i+1])<<8)&0xFF00);
			short both = (short)(lower|higher);
			both = (short)(both);
			bytes[i] = (byte)(both);
			bytes[i+1] = (byte)(both>>8);
		}*/
		
		FileOutputStream s = new FileOutputStream( f );
		s.write(bytes, 0, Math.min( bytesLength, 4  * 44100 ) );
		s.flush();
		s.getFD().sync();
		s.close();
		
		/*for ( int i = 0; i < bytesLength; i += 2 )
		{
			short higher = (short)(((short)bytes[i])&0x00FF);
			short lower = (short)((((short)bytes[i+1])<<8)&0xFF00);
			short both = (short)(lower|higher);
			both = (short)(both);
			bytes[i] = (byte)(both);
			bytes[i+1] = (byte)(both>>8);
		}*/
	}
}
