import java.awt.Color;
import java.awt.Graphics;
import javax.swing.JPanel;


public class Waveform extends JPanel {

	private static final long serialVersionUID = 1L;
	private int[][] samples;
	static final int lineHeight = 23;
	private int frameSize;
	private float playhead;
	
	public void load( byte[] bytes, int numChannels, int frameLength, int frameSize )
	{
		this.frameSize = frameSize;
		samples = new int[numChannels][frameLength];
		int sampleIndex = 0;
		for ( int t = 0; t < bytes.length;) {
			for ( int channel = 0; channel < numChannels; channel++ ) {
				int sample = 0;
				for ( int frame = 0; frame < frameSize; frame++ )
				{
					int b = bytes[t++] & 0x000000FF;
					b = b << (8 * frame); 
					sample = sample | b;
				}
				samples[channel][sampleIndex] = sample;
			}
			sampleIndex++;
		}
		playhead = 0;
	}
	
	public void setPlayhead( float p ) {
		this.playhead = p;
	}
	
	public void paintComponent( Graphics g ) {
		g.setColor( Color.BLACK );
		g.drawLine( 0, lineHeight, (int)getWidth(), lineHeight );
		
		if ( samples != null && samples.length > 0) {
			g.setColor( Color.GREEN );
			int x0 = 0;
			int y0 = getHeight() / 2 ;
			int x1 = 0;
			int y1 = 0;
			int increment = (int)Math.ceil(samples[0].length / getWidth());
			double scaleFactor = getHeight() / Math.pow(2, 8 * frameSize ) / 2;
			for( int i = 0; i < samples[0].length; i += increment )
			{
				y1 = (int)(scaleFactor * samples[0][i]);
				g.drawLine( x1, y1 + lineHeight, x1, lineHeight - y1 );
				x0 = x1;
				y0 = y1;
				x1++;
			}
		}
		
		g.setColor( Color.RED );
		int p = (int) (getWidth() * playhead);
		g.drawLine( p, lineHeight, p, -lineHeight );
		
		
	}
	
}
