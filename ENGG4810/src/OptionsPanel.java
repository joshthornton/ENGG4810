import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.EmptyBorder;


public class OptionsPanel extends JPanel {
	
	private static final long serialVersionUID = 6728484959922842006L;
	private JTextField bmp;
	private JComboBox effectOne;
	private JComboBox effectTwo;
	private Configuration config;
	private MPCEffect[] effects;
	private OptionsPanel that;
	
	public OptionsPanel( Configuration c ) {
		
		that = this;
		this.config = c;
		
		this.setPreferredSize( new Dimension( 1000, 200 ) );
		this.setBorder( BorderFactory.createLineBorder( Color.BLACK ) );
		this.setBackground( Color.WHITE );
		
		// Create Layout
		this.setLayout( new BoxLayout( this, BoxLayout.PAGE_AXIS ) );
		Box left = Box.createVerticalBox();
		Box right = Box.createVerticalBox();
		Box top = Box.createHorizontalBox();
		Box middle = Box.createHorizontalBox();
		middle.add( Box.createGlue() );
		middle.add( left );
		middle.add( right );
		middle.add( Box.createGlue() );
		this.add( top );
		this.add( middle );
		
		// Add number
		JLabel title = new JLabel( "Options" );
		title.setFont( new Font( "Serif", Font.BOLD, 30 ) );
		title.setAlignmentY( Component.LEFT_ALIGNMENT );
		title.setBorder( new EmptyBorder(4, 4, 4, 4) );
		top.add( title );
		top.add( Box.createGlue() );
		
		// Add BPM
		JLabel bmpName = new JLabel( "BPM: ", JLabel.LEFT );
		left.add( bmpName );
		left.add( Box.createGlue() );
		bmp = new JTextField( Integer.toString( config.bpm ));
		bmp.setMaximumSize( bmp.getPreferredSize() );
		bmp.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				try {
					int temp = Integer.parseInt(bmp.getText());
					config.bpm = temp;
				} catch (NumberFormatException e) {
					bmp.setText( Integer.toString( config.bpm ));
					JOptionPane.showMessageDialog(that,
						    e.getMessage(),
						    "Error",
						    JOptionPane.ERROR_MESSAGE);
				}
				
			}
		});
		right.add( bmp );
		right.add( Box.createGlue() );
		
		// Effects strings
		effects = MPCEffect.values();
		String[] strings = new String[effects.length];
		for( int i = 0; i < effects.length; i++ )
			strings[i] = effects[i].toString();
		
		// Add Effect One
		JLabel effectOneName = new JLabel( "Effect One:", JLabel.LEFT );
		left.add( effectOneName );
		left.add( Box.createGlue() );
		effectOne = new JComboBox( strings );
		effectOne.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				config.effectOne = effects[effectOne.getSelectedIndex()];
			}
		});
		right.add( effectOne );
		right.add( Box.createGlue() );
		
		JLabel effectTwoName = new JLabel( "Effect Two:", JLabel.LEFT );
		left.add( effectTwoName );
		left.add( Box.createGlue() );
		effectTwo = new JComboBox( strings );
		effectTwo.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				config.effectTwo = effects[effectTwo.getSelectedIndex()];
			}
		});
		right.add( effectTwo );
		right.add( Box.createGlue() );
	}
	
}
