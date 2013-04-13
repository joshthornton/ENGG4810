import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
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
	private static final String TEMP_FILE = "src/temp/temp.wav";
	private Thread lineThread;
	private Thread playheadThread;
	private int offset;
	private long oldFramePosition;
	private volatile boolean active;
	private boolean playing;
	
	private Waveform canvas;
	public enum SampleType {
		WAV,
		MP3
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
			AudioFormat intermediate = new AudioFormat(encoding, sampleRate, sampleSizeInBits, channels, frameSize, frameRate, bigEndian);
			
			converter = new SampleRateConversionProvider.SampleRateConverterStream( stream, intermediate );
			
			long frameLength = converter.getFrameLength();
			frameSize = converter.getFrameSize();
			bytes = new byte[(int)(frameLength * frameSize)];
			converter.read( bytes );
			load( bytes, (int)frameLength, channels );
			
		} catch ( Exception e ) {
			throw new InvalidFileException( e );
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
			File f = new File(TEMP_FILE);
			AudioSystem.write(din, AudioFileFormat.Type.WAVE, f);
			loadWav( f );
			
		} catch ( Exception e ) {
			e.printStackTrace();
			throw new InvalidFileException( e );
		}
	}
	
	private void load( byte[] bytes, int frameLength, int numChannels ) throws LineUnavailableException
	{
		// Delete extra channels
		byte[] temp = new byte[bytes.length / numChannels];
		for( int i = 0; i < temp.length; ) {
			for( int frame = 0; frame < FRAME_SIZE; frame++ ) {
				temp[i + frame] = bytes[(i*numChannels)+frame];
			}
			i += FRAME_SIZE;
		}
		this.bytes = temp;
		setup();
	}
	
	public int[] getSampleArray() {
		
		int[] samples = new int[bytes.length / FRAME_SIZE];
		int sampleIndex = 0;
		for( int i = 0; i < bytes.length;) {
			int sample = 0;
			for ( int frame = 0; frame < FRAME_SIZE; ++frame ) {
				int b = bytes[i++] & 0x000000FF;
				b = b << (8 * frame); 
				sample = sample | b;
			}
			samples[sampleIndex++] = sample;
		}
		return samples;
	}
	
	public byte[] getByteArray() {
		return bytes;
	}
	
	public AudioFormat getFormat() {
		return format;
	}
	
	private void updateCanvas() {
		canvas.load(getSampleArray());
		canvas.repaint();
	}
	
	public void playPause() {
		if ( line == null )
			play();
		if ( line.isActive() ) {
			pause();
		} else {
			play();
		}
	}
	
	public void playhead() {
		canvas.setPlayhead( ((double)(line.getFramePosition() - oldFramePosition)) / (bytes.length / 2) );
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
		
		stopThreads();
		
		try {
			FileWriter f = new FileWriter(new File( "src/temp/test.data" ));
			short[] s = toShort(bytes);
			for( int i = 0; i < 8*(1<<10); ++i )
				f.write( String.format("0x%x, ", s[i] ) );
			f.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		if ( playing && line != null ) {
			offset = 0;
			oldFramePosition = line.getLongFramePosition();
		}
		
		playing = true;
		active = true;
		line.start();
		
		lineThread = new Thread(new Runnable() {
			public void run() {
				int temp = line.write(bytes, offset, bytes.length - offset );
				line.drain();
				active = false;
			}
		});
        lineThread.setDaemon(true);
        lineThread.start();
        
		playheadThread = new Thread(new Runnable() {
			public void run() {
				
				while( active )
				{
					playhead();
				}
			}
		});
		playheadThread.setDaemon(true);
		playheadThread.start();
	}
	
	public void pause() {
		playing = false;
		line.stop();
		line.flush();
		offset = line.getFramePosition() * 2;
	}
	
	public void back() {
		line.stop();
		line.flush();
		oldFramePosition = line.getFramePosition();
		offset = 0;
		if ( playing )
			play();
		playhead();
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
		bytes = effect.apply( bytes );
		setup();
	}
	
	public void cutLeft() {
		int start = line.getFramePosition();
		int end = bytes.length / 2;
		trim( start, end );
		setup();
	}
	
	public void cutRight() {
		int start = 0;
		int end = line.getFramePosition();
		trim( start, end );
		updateCanvas();
	}
	
	private void trim( int start, int end ) {
		start *= FRAME_SIZE;
		end *= FRAME_SIZE;
		byte[] temp = new byte[end - start];
		for ( int i = 0; i < end - start; i++ )
		{
			temp[i] = bytes[i + start];
		}
		bytes = temp;
	}
	
	private short[] toShort( byte[] bytes ) {
		short[] shorts = new short[bytes.length / 2];
		for( int i = 0; i < shorts.length; ++i )
		{
			short lower = (short)(((short)bytes[i*2])&0x00FF);
			short higher = (short)((((short)bytes[i*2+1])<<8)&0xFF00);
			shorts[i] = (short)(lower|higher);
		}
		return shorts;
	}
}
