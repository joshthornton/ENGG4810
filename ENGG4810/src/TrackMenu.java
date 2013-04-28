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
		JMenuItem delay = new JMenuItem( "Delay..." );
		JMenuItem echo = new JMenuItem( "Echo..." );
		JMenuItem playbackSpeed = new JMenuItem( "Playback Speed..." );
		JMenuItem decimatorBitcrusher = new JMenuItem( "Decimator / Bitcrusher..." );
		JMenuItem cutLeft = new JMenuItem( "Cut Left" );
		JMenuItem cutRight = new JMenuItem( "Cut Right" );
		JMenuItem eq = new JMenuItem( "Graphic EQ" );
		
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
		
		eq.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				JSlider h31 = new JSlider( -20, 50 );
				JSlider h62 = new JSlider( -20, 50 );
				JSlider h125 = new JSlider( -20, 50 );
				JSlider h250 = new JSlider( -20, 50 );
				JSlider h500 = new JSlider( -20, 50 );
				JSlider h1k = new JSlider( -20, 50 );
				JSlider h2k = new JSlider( -20, 50 );
				JSlider h4k = new JSlider( -20, 50 );
				JSlider h8k = new JSlider( -20, 50 );
				JSlider h16k = new JSlider( -20, 50 );
				h31.setValue( 0 );
				h62.setValue( 0 );
				h125.setValue( 0 );
				h250.setValue( 0 );
				h500.setValue( 0 );
				h1k.setValue( 0 );
				h2k.setValue( 0 );
				h4k.setValue( 0 );
				h8k.setValue( 0 );
				h16k.setValue( 0 );
				final JComponent[] inputs = new JComponent[] {
						new JLabel("31Hz:"),
						h31,
						new JLabel("62Hz:"),
						h62,
						new JLabel("125Hz:"),
						h125,
						new JLabel("250Hz:"),
						h250,
						new JLabel("500Hz:"),
						h500,
						new JLabel("1KHz:"),
						h1k,
						new JLabel("2KHz:"),
						h2k,
						new JLabel("4KHz:"),
						h4k,
						new JLabel("8KHz:"),
						h8k,
						new JLabel("16KHz:"),
						h16k
				};
				if ( JOptionPane.showConfirmDialog(null, inputs, "Pitch Shift", JOptionPane.PLAIN_MESSAGE) == JOptionPane.OK_OPTION ) {
					try {
						track.eq( new double[]{ ((double)h31.getValue()) / 100 , ((double)h62.getValue()) / 100 , ((double)h125.getValue()) / 100 , ((double)h250.getValue()) / 100, ((double)h500.getValue()) / 100, ((double)h1k.getValue()) / 100,  ((double)h2k.getValue()) / 100,  ((double)h4k.getValue()) / 100,  ((double)h8k.getValue()) / 100,  ((double)h16k.getValue()) / 100 }  );
					} catch ( Exception ex ) {
						JOptionPane.showMessageDialog(track,
							    ex.getMessage(),
							    "Could not parse value",
							    JOptionPane.ERROR_MESSAGE);
					}
				}
			}
		});
		
		this.add( delay );
		this.add( echo );
		this.add( playbackSpeed );
		this.add( decimatorBitcrusher );
		this.add( cutLeft );
		this.add( cutRight );
		this.add( eq );
		
	}

}
