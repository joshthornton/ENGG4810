import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JToggleButton;
import javax.swing.border.LineBorder;


public class IconToggleButton extends JToggleButton {

	private static final long serialVersionUID = -7585598930694749847L;

	public IconToggleButton( String icon )
	{
		this.setBorderPainted(false);
		this.setContentAreaFilled(false);
		setBorder(LineBorder.createGrayLineBorder());
		try {
			ImageIcon play = new ImageIcon(ImageIO.read(getClass().getResource(icon)));
			this.setIcon( play );
		} catch( Exception ex ) {}
	}
	
}
