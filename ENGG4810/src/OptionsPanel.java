import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.EmptyBorder;


public class OptionsPanel extends JPanel {
	
	private static final long serialVersionUID = 6728484959922842006L;
	private JLabel bpm;
	private JComboBox effectOne;
	private JComboBox effectTwo;
	private Configuration config;
	private MPCEffect[] effects;
	private OptionsPanel that;
	private int currentOne;
	private int currentTwo;
	
	public OptionsPanel( Configuration c ) {
		
		that = this;
		this.config = c;
		currentOne = 0;
		currentTwo = 0;
		
		this.setPreferredSize( new Dimension( 1000, 202 ) );
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
		JLabel bpmName = new JLabel( "BPM: ", JLabel.LEFT );
		left.add( bpmName );
		left.add( Box.createGlue() );
		Box bpmBox = Box.createHorizontalBox();
		//bpmBox.setLayout( new BorderLayout() );
		bpm = new JLabel( Integer.toString( config.bpm ) );
		JButton btn = new JButton( "Set" );
		//bpm.setMaximumSize( bpm.getPreferredSize() );
		btn.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				JTextField bpmfield = new JTextField( Integer.toString( config.bpm ) );
				final JComponent[] inputs = new JComponent[] {
						new JLabel("BPM:"),
						bpmfield
				};
				if ( JOptionPane.showConfirmDialog(null, inputs, "BPM", JOptionPane.PLAIN_MESSAGE) == JOptionPane.OK_OPTION ) {
					try {
						int temp = Integer.parseInt( bpmfield.getText() );
						if ( temp < 10 || temp > 240 )
							throw new RuntimeException( "BPM must be a number between 10 and 240." );
						config.bpm = temp;
						that.bpm.setText( Integer.toString( config.bpm ) );
					} catch ( Exception ex ) {
						JOptionPane.showMessageDialog( that,
							    ex.getMessage(),
							    "BPM Error",
							    JOptionPane.ERROR_MESSAGE);
					}
				}	
			}
		});
		bpmBox.add( Box.createGlue() );
		bpmBox.add( bpm );
		bpmBox.add( Box.createGlue() );
		bpmBox.add( btn, BorderLayout.LINE_END );
		bpmBox.add( Box.createGlue() );
		right.add( bpmBox );
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
				if ( (config.effectTwo == MPCEffect.ECHO || config.effectTwo == MPCEffect.DELAY) && ( effects[effectOne.getSelectedIndex()] == MPCEffect.ECHO ||  effects[effectOne.getSelectedIndex()] == MPCEffect.DELAY ) )
				{
					JOptionPane.showMessageDialog( that,
						    "Cannot select delay and echo at the same time",
						    "Effect Error",
						    JOptionPane.ERROR_MESSAGE);
					effectOne.setSelectedIndex( currentOne );
				} else {
					config.effectOne = effects[effectOne.getSelectedIndex()];
					currentOne = effectOne.getSelectedIndex();
				}
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
				if ( (config.effectOne == MPCEffect.ECHO || config.effectOne == MPCEffect.DELAY) && ( effects[effectTwo.getSelectedIndex()] == MPCEffect.ECHO ||  effects[effectTwo.getSelectedIndex()] == MPCEffect.DELAY ) )
				{
					JOptionPane.showMessageDialog( that,
						    "Cannot select delay and echo at the same time",
						    "Effect Error",
						    JOptionPane.ERROR_MESSAGE);
					effectTwo.setSelectedIndex( currentTwo );
				} else {
					config.effectTwo = effects[effectTwo.getSelectedIndex()];
					currentTwo = effectTwo.getSelectedIndex();
				}
			}
		});
		right.add( effectTwo );
		right.add( Box.createGlue() );
	}
	
	public void redraw()
	{
		currentOne = config.effectOne.getArrIndex();
		currentTwo = config.effectTwo.getArrIndex();
		effectOne.setSelectedIndex( config.effectOne.getArrIndex() );
		effectTwo.setSelectedIndex( config.effectTwo.getArrIndex() );
		bpm.setText( Integer.toString( config.bpm ) );
	}
	
}
