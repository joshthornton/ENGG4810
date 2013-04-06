import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;

import javax.swing.BorderFactory;
import javax.swing.JPanel;


public class TrackPanel extends JPanel {

	private static final long serialVersionUID = -5261110566246194381L;
	private RedrawCallback r;
	private Track[] tracks;
	
	public TrackPanel( RedrawCallback r ) {
		this.r = r;
		this.setPreferredSize( new Dimension( 600, 800 ) );
		this.setBorder( BorderFactory.createLineBorder( Color.BLACK ) );
		this.setBackground( Color.white );
		
		this.setLayout( new GridLayout( 16, 1 ) );
		
		tracks = new Track[16];
		for ( int i = 0; i < tracks.length; ++i )
		{
			tracks[i] = new Track( r );
			this.add( tracks[i] );
		}
	}

}