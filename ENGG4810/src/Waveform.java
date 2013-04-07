import java.awt.Color;
import java.awt.Graphics;
import javax.swing.JPanel;


public class Waveform extends JPanel {

	private static final long serialVersionUID = 1L;
	private int[] samples;
	static final int lineHeight = 23;
	private double playhead;
	
	public void load( int[] samples )
	{
		this.samples = samples;
		playhead = 0;
	}
	
	public void setPlayhead( double p ) {
		this.playhead = p;
	}
	
	public void paintComponent( Graphics g ) {
		
		g.clearRect(0, 0, getWidth(), getHeight());
		
		g.setColor( Color.BLACK );
		g.drawLine( 0, lineHeight, (int)getWidth(), lineHeight );
		
		if ( samples != null ) {
			g.setColor( Color.GREEN );
			int x1 = 0;
			int y1 = 0;
			int increment = (int)Math.ceil(samples.length / getWidth());
			double scaleFactor = getHeight() / Math.pow(2, 16 ) / 2;
			for( int i = 0; i < samples.length; i += increment )
			{
				y1 = (int)(scaleFactor * samples[i]);
				g.drawLine( x1, y1 + lineHeight, x1, lineHeight - y1 );
				x1++;
			}
		}
		
		g.setColor( Color.RED );
		int p = (int) (getWidth() * playhead);
		g.drawLine( p, getHeight(), p, 0 );
		
	}
	
}
