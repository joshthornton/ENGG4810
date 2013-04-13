import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.JSlider;
import javax.swing.JTextField;
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
		delay.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				JTextField delay = new JTextField( "500" );
				JSlider amount = new JSlider( 0, 1000 );
				amount.setValue( 700 );
				final JComponent[] inputs = new JComponent[] {
						new JLabel("Delay (ms):"),
						delay,
						new JLabel("Amount (0.0-1.0):"),
						amount
				};
				if ( JOptionPane.showConfirmDialog(null, inputs, "Delay", JOptionPane.PLAIN_MESSAGE) == JOptionPane.OK_OPTION ) {
					try {
						track.delay(Integer.parseInt(delay.getText()), ((double)amount.getValue()) / 1000 );
					} catch ( Exception ex ) {
						JOptionPane.showMessageDialog(track,
							    ex.getMessage(),
							    "Could not parse value",
							    JOptionPane.ERROR_MESSAGE);
					}
				}
			}
		});
		echo.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				JTextField echo = new JTextField( "500" );
				JSlider amount = new JSlider( 0, 1000 );
				amount.setValue( 700 );
				final JComponent[] inputs = new JComponent[] {
						new JLabel("Delay (ms):"),
						echo,
						new JLabel("Amount (0.0-1.0):"),
						amount
				};
				if ( JOptionPane.showConfirmDialog(null, inputs, "Echo", JOptionPane.PLAIN_MESSAGE) == JOptionPane.OK_OPTION ) {
					try {
						track.echo(Integer.parseInt(echo.getText()), ((double)amount.getValue()) / 1000 );
					} catch ( Exception ex ) {
						JOptionPane.showMessageDialog(track,
							    ex.getMessage(),
							    "Could not parse value",
							    JOptionPane.ERROR_MESSAGE);
					}
				}
			}
		});
		decimatorBitcrusher.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				JSlider bitcrusher = new JSlider(0, 8 );
				bitcrusher.setValue(0);
				JSlider decimator = new JSlider( 0, 30 );
				decimator.setValue(0);
				final JComponent[] inputs = new JComponent[] {
						new JLabel("Num bits to crush (0-8):"),
						bitcrusher,
						new JLabel("Num samples to decimate (0-30):"),
						decimator
				};
				if ( JOptionPane.showConfirmDialog(null, inputs, "Decimator / Bitcrusher", JOptionPane.PLAIN_MESSAGE) == JOptionPane.OK_OPTION ) {
					try {
						track.bitcrusherDecimator( bitcrusher.getValue(), decimator.getValue() );
					} catch ( Exception ex ) {
						JOptionPane.showMessageDialog(track,
							    ex.getMessage(),
							    "Could not parse value",
							    JOptionPane.ERROR_MESSAGE);
					}
				}
			}
		});
		playbackSpeed.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				JSlider shift = new JSlider( -4, 8 );
				shift.setValue( 0 );
				final JComponent[] inputs = new JComponent[] {
						new JLabel("Shift (1/4x - 8x):"),
						shift
				};
				if ( JOptionPane.showConfirmDialog(null, inputs, "Pitch Shift", JOptionPane.PLAIN_MESSAGE) == JOptionPane.OK_OPTION ) {
					try {
						track.pitchshift( shift.getValue() );
					} catch ( Exception ex ) {
						JOptionPane.showMessageDialog(track,
							    ex.getMessage(),
							    "Could not parse value",
							    JOptionPane.ERROR_MESSAGE);
					}
				}
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
