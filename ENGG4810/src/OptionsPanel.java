import java.awt.Color;
import java.awt.Dimension;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;


public class OptionsPanel extends JPanel {
	
	private static final long serialVersionUID = 6728484959922842006L;
	private RedrawCallback r;
	
	public OptionsPanel( RedrawCallback r ) {
		this.r = r;
		this.setPreferredSize( new Dimension( 600, 200 ) );
		this.setBorder( BorderFactory.createLineBorder( Color.BLACK ) );
		this.setBackground( Color.WHITE );
		
		this.add( new JLabel( "TEST" ) );
	}
	
}
