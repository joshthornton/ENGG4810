import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JPanel;


public class Track extends JPanel {

	private static final long serialVersionUID = -7862960515614918660L;
	private TrackMenu contextMenu;
	private Waveform canvas;
	Sample sample;
	
	public Track() {
		
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
		
		// Add play/pause and back buttons
		JButton playPause = new IconButton( "resources/playpause.png" );
		JButton back = new IconButton( "resources/back.png" );
		playPause.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				sample.playPause();
			} 	
		});
		back.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				sample.back();
			} 	
		});
		
		buttons.add( playPause );
		buttons.add( back );
		
	}
	
	public void loadWav( File file ) throws InvalidFileException {
		sample = new Sample( file, canvas, Sample.SampleType.WAV );
	}
	
	public void loadMP3( File file ) throws InvalidFileException {
		sample = new Sample( file, canvas, Sample.SampleType.MP3 );
	}
	
	public void cutLeft() {
		sample.cutLeft();
	}
	
	public void cutRight() {
		sample.cutRight();
	}
	
	public void delay(int ms, double amount ) {
		sample.applyEffect( new Delay( ms, amount ) );
	}
	
	public void echo(int ms, double amount ) {
		sample.applyEffect( new Echo( ms, amount ) );
	}
	
	public void eq( double[] gains ) {
		sample.applyEffect( new EQ( gains ) );
	}
	
	public void bitcrusherDecimator(int bitcrusher, int decimator ) {
		if ( bitcrusher > 0 )
			sample.applyEffect( new BitCrusher( bitcrusher ) );
		if ( decimator > 0 )
			sample.applyEffect( new Decimator( decimator ) );
	}
	
	public void pitchshift(int factor ) {
		if ( factor != 0 )
			sample.applyEffect( new PitchShift( factor ) );
	}
	
}
