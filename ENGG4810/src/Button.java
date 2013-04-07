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
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;


public class Button extends JPanel {
	
	private static final long serialVersionUID = -7987211717336865530L;
	private ButtonConfiguration config;
	private JComboBox modeLabel;
	private JComboBox trackLabel;
	private JComboBox actionLabel;
	private JComboBox loopLabel;
	
	private Mode[] modes;
	private LoopInterval[] loops;
	private Action[] actions;
	private TrackEnum[] tracks;
	

	public Button( int num, ButtonConfiguration c)
	{
		this.config = c;
		
		this.setPreferredSize( new Dimension( 250, 250 ) );
		this.setBackground( Color.WHITE );
		
		// Add border
		this.setBorder( BorderFactory.createLineBorder( Color.BLACK ) );
		
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
		JLabel number = new JLabel( Integer.toString(num) );
		number.setFont( new Font( "Serif", Font.BOLD, 30 ) );
		number.setAlignmentY( Component.LEFT_ALIGNMENT );
		number.setBorder( new EmptyBorder(4, 4, 4, 4) );
		top.add( number );
		top.add( Box.createGlue() );
		
		// Create option arrays
		modes = Mode.values();
		loops = LoopInterval.values();
		actions = Action.values();
		tracks = TrackEnum.values();
		
		// Add Mode
		JLabel mode = new JLabel( "Mode: ", JLabel.LEFT );
		left.add( mode );
		left.add( Box.createGlue() );
		modeLabel = new JComboBox( enumToString( modes ) );
		modeLabel.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				config.mode = modes[modeLabel.getSelectedIndex()];
			}
		});
		right.add( modeLabel );
		right.add( Box.createGlue() );
		
		// Add Loop
		JLabel loop = new JLabel( "Loop Int:", JLabel.LEFT );
		left.add( loop );
		left.add( Box.createGlue() );
		loopLabel = new JComboBox( enumToString( loops ) );
		loopLabel.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				config.loop = loops[loopLabel.getSelectedIndex()];
			}
		});
		right.add( loopLabel );
		right.add( Box.createGlue() );
		
		// Add Track
		JLabel track = new JLabel( "Track: ", JLabel.LEFT );
		left.add( track );
		left.add( Box.createGlue() );
		trackLabel = new JComboBox( enumToString( tracks ) );
		trackLabel.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				config.track = tracks[trackLabel.getSelectedIndex()];
			}
		});
		right.add( trackLabel );
		right.add( Box.createGlue() );
		
		// Add Action
		JLabel action = new JLabel( "Action: ", JLabel.LEFT );
		left.add( action );
		left.add( Box.createGlue() );
		actionLabel = new JComboBox( enumToString( actions ) );
		actionLabel.addActionListener( new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				config.action = actions[actionLabel.getSelectedIndex()];
			}
		});
		right.add( actionLabel );
		right.add( Box.createGlue() );
		
		redraw();
		
	}
	
	private static <T> String[] enumToString( T[] arr ) {
		String[] s = new String[ arr.length ];
		for ( int i = 0; i < arr.length; i++ ) {
			s[i] = arr[i].toString();
		}
		return s;
	}
	
	public void redraw() {
		modeLabel.setSelectedIndex( config.mode.getIndex() );
		loopLabel.setSelectedIndex( config.loop.getIndex() );
		trackLabel.setSelectedIndex( config.track.getIndex() );
		actionLabel.setSelectedIndex( config.action.getIndex() );
	}
	
	public void updateConfig( ButtonConfiguration config ) {
		this.config = config;
	}
	
}
