import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ButtonGroup;
import javax.swing.ButtonModel;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JRadioButtonMenuItem;


public class ButtonMenu extends JPopupMenu {
	
	private static final long serialVersionUID = 6729620876904078605L;
	private Button button;
	ButtonGroup modeGroup;
	ButtonGroup trackGroup;
	ButtonGroup actionGroup;
	ButtonModel[] modeModels;
	ButtonModel[] trackModels;
	ButtonModel[] actionModels;
	private RedrawCallback r;
	
	public ButtonMenu( Button button, RedrawCallback r ) {
		
		// Save parent button
		this.button = button;
		this.r = r;
		
		// Add mode submenu
		JMenu mode = new JMenu( "Mode" );
		modeGroup = new ButtonGroup();
		Mode[] modes = Mode.values();
		modeModels = new ButtonModel[modes.length];
		for ( int i = 0; i < modes.length; ++i )
		{
			JMenuItem m = new JRadioButtonMenuItem( modes[i].toString() );
			modeModels[i] = m.getModel();
			m.addActionListener( new ModeActionListener( modes[i] ) );
			modeGroup.add( m );
			mode.add( m );
		}
		modeGroup.setSelected( modeModels[ button.getMode().getIndex() ], true);
		this.add( mode );
		
		// Add Track submenu
		JMenu track = new JMenu( "Track" );
		trackGroup = new ButtonGroup();
		trackModels = new ButtonModel[Configuration.NUM_BUTTONS + 1];
		JMenuItem t = new JRadioButtonMenuItem( "None" );
		trackModels[0] = t.getModel();
		t.addActionListener( new TrackActionListener( -1 ) );
		trackGroup.add( t );
		track.add( t );
		for ( int i = 0; i < Configuration.NUM_BUTTONS; ++i )
		{
			t = new JRadioButtonMenuItem( Integer.toString( i + 1 ) );
			trackModels[i+1] = t.getModel();
			t.addActionListener( new TrackActionListener( i ) );
			trackGroup.add( t );
			track.add( t );
		}
		trackGroup.setSelected( trackModels[ button.getTrack() + 1 ], true);
		this.add( track );
		
		// Add action submenu
		JMenu action = new JMenu( "Action" );
		actionGroup = new ButtonGroup();
		Action[] actions = Action.values();
		actionModels = new ButtonModel[actions.length];
		for ( int i = 0; i < actions.length; ++i )
		{
			JMenuItem a = new JRadioButtonMenuItem( actions[i].toString() );
			actionModels[i] = a.getModel();
			a.addActionListener( new ActionActionListener( actions[i] ) );
			actionGroup.add( a );
			action.add( a );
		}
		actionGroup.setSelected( actionModels[ button.getAction().getIndex() ], true);
		this.add( action );
		
	}
	
	public void redraw() {
		modeGroup.setSelected( modeModels[ button.getMode().getIndex() ], true);
		trackGroup.setSelected( trackModels[ button.getTrack() + 1 ], true);
		actionGroup.setSelected( actionModels[ button.getAction().getIndex() ], true);
	}
	
	private class ModeActionListener implements ActionListener {
		private Mode mode;
		public ModeActionListener( Mode mode )
		{
			this.mode = mode;
		}
		@Override
		public void actionPerformed(ActionEvent a) {
			button.setMode( mode );
		}
	}
	
	private class TrackActionListener implements ActionListener {
		private int track;
		public TrackActionListener( int track )
		{
			this.track = track;
		}
		@Override
		public void actionPerformed(ActionEvent a) {
			button.setTrack( track );
		}
	}
	
	private class ActionActionListener implements ActionListener {
		private Action action;
		public ActionActionListener( Action action )
		{
			this.action = action;
		}
		@Override
		public void actionPerformed(ActionEvent a) {
			button.setAction( action );
		}
	}

}
