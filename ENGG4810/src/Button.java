import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;


public class Button extends JPanel {
	
	private static final long serialVersionUID = -7987211717336865530L;
	private ButtonConfiguration config;
	private JLabel modeLabel;
	private JLabel trackLabel;
	private JLabel actionLabel;
	private ButtonMenu contextMenu;
	

	public Button( int num, ButtonConfiguration config )
	{
		this.config = config;
		
		// Add border
		this.setBorder( BorderFactory.createLineBorder( Color.BLACK) );
		
		// Create Layout
		GridBagLayout layout = new GridBagLayout();
		this.setLayout( layout );
		GridBagConstraints c = new GridBagConstraints();
		
		// Add number
		c.gridx = 0;
		c.gridy = 0;
		c.gridwidth = 1;
		c.gridheight = 1;
		JLabel number = new JLabel( Integer.toString(num) );
		this.add( number, c );
		
		// Add Mode
		JLabel mode = new JLabel( "Mode: " );
		c.gridx = 0;
		c.gridy = 1;
		this.add( mode, c );
		modeLabel = new JLabel( config.mode.toString() );
		c.gridx = 1;
		c.gridy = 1;
		this.add( modeLabel, c );
		
		// Add Track
		JLabel track = new JLabel( "Track: " );
		c.gridx = 0;
		c.gridy = 2;
		this.add( track, c );
		trackLabel = new JLabel( Integer.toString( config.track ) );
		c.gridx = 1;
		c.gridy = 2;
		this.add( trackLabel, c );
		
		// Add Action
		JLabel action = new JLabel( "Action: " );
		c.gridx = 0;
		c.gridy = 3;
		this.add( action, c );
		actionLabel = new JLabel( config.action.toString() );
		c.gridx = 1;
		c.gridy = 3;
		this.add( actionLabel, c );
		
		// Add listener for right click
		contextMenu = new ButtonMenu( this );
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
		trackLabel.setText( Integer.toString( config.track ) );
	}
	
	public int getTrack() {
		return config.track;
	}
	
	public void redraw() {
		modeLabel.setText( config.mode.toString() );
		actionLabel.setText( config.action.toString() );
		trackLabel.setText( Integer.toString( config.track ) );
		contextMenu.redraw();
	}
	
	public void updateConfig( ButtonConfiguration config ) {
		this.config = config;
	}
	
}
