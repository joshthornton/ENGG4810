import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.io.File;

import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JToggleButton;


public class Track extends JPanel implements MouseListener {

	private static final long serialVersionUID = -7862960515614918660L;
	private RedrawCallback r;
	private Clip clip;
	private TrackMenu contextMenu;
	private Waveform canvas;
	private int frameLength;
	private int numChannels;
	private int frameSize;
	private byte[] bytes;
	
	public Track( RedrawCallback r ) {
		
		this.r = r;
		this.setPreferredSize( new Dimension( 600, 50 ) );
		this.setBorder( BorderFactory.createLineBorder( Color.BLACK ) );
		contextMenu = new TrackMenu( this );
		this.setComponentPopupMenu( contextMenu );
		
		// Set layout
		this.setLayout( new BorderLayout() );
		JPanel buttons = new JPanel();
		buttons.setPreferredSize( new Dimension( 25, 50 ) );
		buttons.setLayout( new GridLayout( 2, 1 ) );
		canvas = new Waveform();
		canvas.setBorder( BorderFactory.createMatteBorder(0, 1, 0, 0, Color.BLACK ) );
		canvas.setPreferredSize( new Dimension( 570, 50 ) );
		this.add( buttons, BorderLayout.LINE_START );
		this.add( canvas, BorderLayout.LINE_END );
		
		// Add play/pause and loop buttons
		JButton playPause = new IconButton( "resources/playpause.png" );
		final JToggleButton loop = new IconToggleButton( "resources/loop.png" );
		playPause.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				if ( clip.isActive() )
					clip.stop();
				else
					clip.start();
			} 	
		});
		loop.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				if ( loop.isSelected() )
					clip.loop( 0 );
				else 
					clip.loop( Clip.LOOP_CONTINUOUSLY );
			} 	
		});
		
		buttons.add( playPause );
		buttons.add( loop );
		
	}
	
	public void load( File file ) throws InvalidFileException {
		AudioInputStream audio;
		try {
			audio = AudioSystem.getAudioInputStream( file );
			frameLength = (int) audio.getFrameLength();
			numChannels = audio.getFormat().getChannels();
			frameSize = (int) audio.getFormat().getFrameSize();
			bytes = new byte[frameLength * frameSize];
			int temp = 0;
			try {
				temp = audio.read( bytes );
			} catch ( Exception e ) {
				e.printStackTrace();
			}
			System.out.println( "temp: " + temp );
			canvas.load( bytes, numChannels, frameLength, frameSize );
			clip = AudioSystem.getClip();
			clip.open(audio.getFormat(), bytes, 0, frameLength * frameSize);
		} catch ( Exception e ) {
			throw new InvalidFileException( e );
		}
		canvas.repaint();
	}
	
	public void draw() {
	}

	@Override
	public void mouseClicked(MouseEvent e) {
		contextMenu.show( this, e.getX(), e.getY() );
	}

	@Override
	public void mouseEntered(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseExited(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mousePressed(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseReleased(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
	}
	
}
