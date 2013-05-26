import java.awt.Color;
//import java.awt.Font;
import java.awt.Graphics;
//import java.awt.Paint;

import javax.swing.JPanel;


public class Waveform extends JPanel {

	private static final long serialVersionUID = 1L;
	private byte[] bytes;
	static final int lineHeight = 23;
	private double playhead;
	private int bytesLength;
	//private int index;
	
	public Waveform( int index )
	{
		//this.index = index;
	}
	
	public void load( byte[] bytes, int length )
	{
		this.bytes = bytes;
		this.bytesLength = length;
		playhead = 0;
	}
	
	public void setPlayhead( double p ) {
		this.playhead = p;
	}
	
	public void paintComponent( Graphics g ) {
		
		g.clearRect(0, 0, getWidth(), getHeight());
		
		g.setColor( Color.BLACK );
		g.drawLine( 0, lineHeight, (int)getWidth(), lineHeight );
		
		if ( bytes != null ) {
			g.setColor( Color.GREEN );
			int x1 = 0;
			int y1 = 0;
			int increment = (int)Math.ceil(bytesLength / getWidth());
			if ( increment == 0 )
				increment = 1;
			short max = 0;
			for( int i = 0; i < bytesLength; i += increment )
			{
				short lower = (short)(((short)bytes[(i/2)*2])&0x00FF);
				short higher = (short)((((short)bytes[(i/2)*2+1])<<8)&0xFF00);
				short both = (short)(lower|higher);
				if ( both > max )
					max = both;
			}
			double scaleFactor = ((double)getHeight()) / max;
			for( int i = 0; i < bytesLength; i += increment )
			{
				if ( i > 4 * 44100 )
					g.setColor( Color.BLUE );
				short lower = (short)(((short)bytes[(i/2)*2])&0x00FF);
				short higher = (short)((((short)bytes[(i/2)*2+1])<<8)&0xFF00);
				short both = (short)(lower|higher);
				y1 = (int)(scaleFactor * both);
				g.drawLine( x1, y1 + lineHeight, x1, lineHeight - y1 );
				x1++;
			}
		}
		
		g.setColor( Color.RED );
		int p = (int) (getWidth() * playhead);
		g.drawLine( p, getHeight(), p, 0 );
		
		// draw track number
		// g.setColor( new Color( 0, 0, 0, 20 ) );
		// g.setFont( new Font("Monospaced", Font.BOLD, getHeight() ) );
		// g.drawString(("Button " + index), 2, getHeight() - 10 );
		
	}
	
}
