import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.io.File;
import java.io.IOException;

import javax.swing.BorderFactory;
import javax.swing.JPanel;


public class TrackPanel extends JPanel {

	private static final long serialVersionUID = -5261110566246194381L;
	private Track[] tracks;
	
	public TrackPanel() {
		this.setPreferredSize( new Dimension( 600, 802 ) );
		this.setBorder( BorderFactory.createLineBorder( Color.BLACK ) );
		this.setBackground( Color.white );
		
		this.setLayout( new GridLayout( 16, 1 ) );
		
		tracks = new Track[16];
		for ( int i = 0; i < tracks.length; ++i )
		{
			tracks[i] = new Track( i + 1 );
			this.add( tracks[i] );
		}
	}
	
	public void writeData( File[] f ) throws IOException
	{
		for ( int i = 0; i < 16; ++i )
			tracks[ Configuration.BOARD_MAPPING[i] ].writeData( f[i] );
	}
	
	public void readData( File[] f ) throws IOException
	{
		for ( int i = 0; i < 16; ++i )
			tracks[ Configuration.BOARD_MAPPING[i] ].readData( f[i] );
	}

}
