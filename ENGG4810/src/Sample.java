import java.io.File;
import java.io.InputStream;

import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
import javax.sound.sampled.LineUnavailableException;

import org.tritonus.sampled.convert.SampleRateConversionProvider;


public class Sample {
	
	private AudioFormat format;
	private byte[] bytes;
	private Clip clip;
	private static final int FRAME_SIZE = 2;
	private static final String TEMP_FILE = "src/temp/temp.wav";
	public enum SampleType {
		WAV,
		MP3
	}
	
	public Sample( File file, SampleType type ) throws InvalidFileException {
		
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
			loadClip( bytes, (int)frameLength, channels );
			
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
	
	private void loadClip( byte[] bytes, int frameLength, int numChannels ) throws LineUnavailableException
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
		
		clip = AudioSystem.getClip();
		clip.open(format, this.bytes, 0, this.bytes.length );
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
	
	public void playPause() {
		if ( clip.isActive() ) {
			clip.stop();
		} else {
			clip.start();
		}
	}
	
	public double playhead() {
		return ((double)clip.getFramePosition()) / clip.getFrameLength();
	}
	
	public void play() {
		clip.start();
	}
	
	public void pause() {
		clip.stop();
	}
	
	public void back() {
		clip.setFramePosition(0);
	}
	
	public void cutLeft() {
		int start = clip.getFramePosition();
		int end = clip.getFrameLength();
		trim( start, end );
	}
	
	public void cutRight() {
		int start = 0;
		int end = clip.getFramePosition();
		trim( start, end );
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
		try {
			clip = AudioSystem.getClip();
			clip.open(format, bytes, 0, this.bytes.length );
		} catch ( Exception e ) {
			e.printStackTrace();
		}
	}
}
