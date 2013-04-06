
public class RedrawCallback {

	private MainFrame m;
	
	public RedrawCallback( MainFrame m ) {
		this.m = m;
	}
	
	public void redraw() {
		m.redraw();
	}
	
}
