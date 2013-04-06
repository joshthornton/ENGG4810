import java.awt.Dimension;
import java.awt.GridLayout;

import javax.swing.JPanel;


public class ButtonPanel extends JPanel {

	private static final long serialVersionUID = -8726704462364227154L;
	public final static int width = 4;
	public final static int height = 4;
	private Button[] buttons;
	RedrawCallback r;
	
	public ButtonPanel( Configuration config, RedrawCallback r )
	{	
		this.r = r;
		// Create Layout
		GridLayout layout = new GridLayout( width, height );
		this.setLayout( layout );
		this.setPreferredSize( new Dimension( 600, 600 ) );
		
		// Create Buttons
		buttons = new Button[width * height];
		for ( int i = 0; i < width * height; ++i )
		{
			buttons[i] = new Button( i, config.buttons[i], r );
			this.add( buttons[i] );
		}
		
		
	}
	
	public void updateConfig( Configuration config ) {
		for ( int i = 0; i < width * height; ++i )
		{
			buttons[i].updateConfig( config.buttons[i] );
		}
	}
	
	public void redraw() {
		for ( int i = 0; i < width * height; ++i )
		{
			buttons[i].redraw();
		}
	}
	
}
