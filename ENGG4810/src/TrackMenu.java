import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.JFileChooser;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.filechooser.FileNameExtensionFilter;


public class TrackMenu extends JPopupMenu {
	
	private static final long serialVersionUID = 6729620876904078605L;
	private Track track;
	
	public TrackMenu( Track t ) {
		
		// Save parent button
		this.track = t;
		
		// Load sample
		JMenuItem wav = new JMenuItem( "Load WAV..." );
		wav.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				
				//Creating the file chooser popup
				JFileChooser fileChooser = new JFileChooser(".");
				
				//Ensuring only .for files can be opened
				fileChooser.setAcceptAllFileFilterUsed(false);
				
				//Ensuring only formulas are opened
				fileChooser.addChoosableFileFilter(new FileNameExtensionFilter(
						"WAV Files", new String[] {"wav"}));
				
				//Displaying the file chooser
			    int status = fileChooser.showOpenDialog( track );
			    
			    if (status == JFileChooser.APPROVE_OPTION) {
			    	try {
				    	File selectedFile = fileChooser.getSelectedFile();
				    	track.loadWav( selectedFile );
			    	} catch (InvalidFileException ex) {
			    		
			    		JOptionPane.showMessageDialog(track,
							    ex.getMessage(),
							    "Error",
							    JOptionPane.ERROR_MESSAGE);
			    		
			    	}
			    }
			}
		});
		this.add( wav );
		JMenuItem mp3 = new JMenuItem( "Load MP3..." );
		mp3.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				
				//Creating the file chooser popup
				JFileChooser fileChooser = new JFileChooser(".");
				
				//Ensuring only .for files can be opened
				fileChooser.setAcceptAllFileFilterUsed(false);
				
				//Ensuring only formulas are opened
				fileChooser.addChoosableFileFilter(new FileNameExtensionFilter(
						"MP3 Files", new String[] {"mp3"}));
				
				//Displaying the file chooser
			    int status = fileChooser.showOpenDialog( track );
			    
			    if (status == JFileChooser.APPROVE_OPTION) {
			    	try {
				    	File selectedFile = fileChooser.getSelectedFile();
				    	track.loadMP3( selectedFile );
			    	} catch (InvalidFileException ex) {
			    		
			    		JOptionPane.showMessageDialog(track,
							    ex.getMessage(),
							    "Error",
							    JOptionPane.ERROR_MESSAGE);
			    		
			    	}
			    }
			}
		});
		this.add( mp3 );
		this.addSeparator();
		
		// Effects
		JMenuItem volumeEnveloping = new JMenuItem( "Volume Enveloping..." );
		JMenuItem delay = new JMenuItem( "Delay..." );
		JMenuItem echo = new JMenuItem( "Echo..." );
		JMenuItem playbackSpeed = new JMenuItem( "Playback Speed..." );
		JMenuItem decimatorBitcrusher = new JMenuItem( "Decimator / Bitcrusher..." );
		JMenuItem cutLeft = new JMenuItem( "Cut Left" );
		JMenuItem cutRight = new JMenuItem( "Cut Right" );
		
		// Add listeners
		cutLeft.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				track.cutLeft();
			}
		});
		cutRight.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				track.cutRight();
			}
		});
		
		this.add( volumeEnveloping );
		this.add( delay );
		this.add( echo );
		this.add( playbackSpeed );
		this.add( decimatorBitcrusher );
		this.add( cutLeft );
		this.add( cutRight );
		
	}

}
