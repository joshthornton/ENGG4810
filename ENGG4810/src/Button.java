import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;


public class Button extends JPanel {
	
	private static final long serialVersionUID = -7987211717336865530L;
	private ButtonConfiguration config;
	private JLabel modeLabel;
	private JLabel trackLabel;
	private JLabel actionLabel;
	private ButtonMenu contextMenu;
	private RedrawCallback r;

	public Button( int num, ButtonConfiguration config, RedrawCallback r )
	{
		this.r = r;
		this.config = config;
		
		this.setPreferredSize( new Dimension( 200, 200 ) );
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
		
		// Add Mode
		JLabel mode = new JLabel( "Mode: ", JLabel.LEFT );
		left.add( mode );
		left.add( Box.createGlue() );
		modeLabel = new JLabel( config.mode.toString(), JLabel.LEFT );
		right.add( modeLabel );
		right.add( Box.createGlue() );
		
		// Add Track
		JLabel track = new JLabel( "Track: ", JLabel.LEFT );
		left.add( track );
		left.add( Box.createGlue() );
		String s = ( config.track < 0 ) ? "None" : "Track " + Integer.toString( config.track + 1 );
		trackLabel = new JLabel( s, JLabel.LEFT );
		right.add( trackLabel );
		right.add( Box.createGlue() );
		
		// Add Action
		JLabel action = new JLabel( "Action: ", JLabel.LEFT );
		left.add( action );
		left.add( Box.createGlue() );
		actionLabel = new JLabel( config.action.toString(), JLabel.LEFT );
		right.add( actionLabel );
		right.add( Box.createGlue() );
		
		// Add listener for right click
		contextMenu = new ButtonMenu( this, r );
		this.setComponentPopupMenu( contextMenu );
		
	}
	
	public void setMode( Mode mode )
	{
		config.mode = mode;
		modeLabel.setText( config.mode.toString() );
	}
	
	public Mode getMode()
	{
		return config.mode;
	}
	
	public void setAction( Action action )
	{
		config.action = action;
		actionLabel.setText( config.action.toString() );
	}
	
	public Action getAction()
	{
		return config.action;
	}
	
	public void setTrack( int track )
	{
		config.track = track;
		if ( config.track < 0 )
		{
			trackLabel.setText( "None" );
		} else {
			trackLabel.setText( "Track " + Integer.toString( config.track + 1 ) );
		}
	}
	
	public int getTrack() {
		return config.track;
	}
	
	public void redraw() {
		modeLabel.setText( config.mode.toString() );
		actionLabel.setText( config.action.toString() );
		if ( config.track < 0 )
		{
			trackLabel.setText( "None" );
		} else {
			trackLabel.setText( "Track " + Integer.toString( config.track + 1 ) );
		}
		contextMenu.redraw();
	}
	
	public void updateConfig( ButtonConfiguration config ) {
		this.config = config;
	}
	
}
