import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;

import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;


public class MainFrame extends JFrame {

	private static final long serialVersionUID = 2269971701250845501L;
	private MainFrame that = this;
	private ButtonPanel buttons;
	private TrackPanel tracks;
	private OptionsPanel options;
	private Configuration config;
	
	public static void main(String[] args) {
        
		MainFrame mainFrame = new MainFrame();
        mainFrame.setVisible(true);
        
    }
	
	public MainFrame() {
		
		setName( "ENGG4810 Group 27" );
		setTitle( "ENGG4810 Group 27" );
		setBounds( 100, 100, 1600, 800 );
		setResizable(false);
		
		// Creating menu
		JMenuBar menu = new JMenuBar();
		this.setJMenuBar(menu);
		JMenu file = new JMenu("File");
		JMenu help = new JMenu("Help");
		menu.add(file);
		menu.add(help);
		JMenuItem open = new JMenuItem("Open");
		JMenuItem save = new JMenuItem("Save");
		JMenuItem exit = new JMenuItem("Exit");
		JMenuItem about = new JMenuItem("About");
		file.add(open);
		file.add(save);
		file.add(exit);
		help.add(about);
		
		// Adding listeners for menu
		open.addActionListener(new ActionListener() {

			public void actionPerformed(ActionEvent e) {
				
				//Creating the file chooser popup
				final JFileChooser fileChooser = new JFileChooser();
				
				//Displaying the file chooser
				fileChooser.setFileSelectionMode( JFileChooser.DIRECTORIES_ONLY );
			    int status = fileChooser.showOpenDialog(that);
			    
			    if (status == JFileChooser.APPROVE_OPTION) {
			    	
			    	final JDialog dialog = new JDialog( that, "Please Wait", false );
			    	JPanel p = new JPanel();
			    	p.setBorder( new EmptyBorder( 20, 20, 20, 20 ) );
			    	p.add( new JLabel( "Reading - do not disconnect." ) );
		    		dialog.getContentPane().add( p );
		    		dialog.pack();
		    		dialog.setResizable( false );
			    	dialog.setLocationRelativeTo(null);
			    	dialog.setVisible( true );		    	
			    	
			    	Thread t = new Thread(new Runnable() {
			            public void run() {
			            	try {
						    	
					    		//Getting the selected file
						    	File folder = fileChooser.getSelectedFile();
						    	
						    	File configFile = new File( folder.getAbsolutePath() + File.separator + "cfg.cfg" ); 
						    	File[] buttonFiles = new File[16];
						    	
						    	try {
						    		for ( int i = 0; i < 16; ++i )
							    	{
							    		buttonFiles[i] = new File( folder.getAbsolutePath() + File.separator + (i+1) + ".dat" );
							    	}
						    		config.readStructure( configFile );
						    		tracks.readData( buttonFiles );
							    	
						    	} catch( IOException ex ) {
						    		throw new InvalidFileException( ex );
						    	}
						    	buttons.updateConfig( config );
						    	redraw();
					    	} catch (InvalidFileException ex) {
					    		
					    		JOptionPane.showMessageDialog(that,
									    ex.getMessage(),
									    "Error",
									    JOptionPane.ERROR_MESSAGE);
					    		
					    	}
			            	dialog.setVisible( false );
			            }
			         });
			    	t.setDaemon( true );
			    	t.start();
			    	
			    	
			    }
				
			}}
		);
		save.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				
				//Creating a new file listener
				final JFileChooser fileChooser = new JFileChooser();
				
				//Displaying the file chooser
				fileChooser.setFileSelectionMode( JFileChooser.DIRECTORIES_ONLY );
			    int status = fileChooser.showSaveDialog(that);
			    
			    if (status == JFileChooser.APPROVE_OPTION) {
			    	
			    	final JDialog dialog = new JDialog( that, "Please Wait", false );
			    	JPanel p = new JPanel();
			    	p.setBorder( new EmptyBorder( 20, 20, 20, 20 ) );
			    	p.add( new JLabel( "Writing - do not disconnect." ) );
		    		dialog.getContentPane().add( p );
		    		dialog.pack();
		    		dialog.setResizable( false );
			    	dialog.setLocationRelativeTo(null);
			    	dialog.setVisible( true );		    	
			    	
			    	Thread t = new Thread(new Runnable() {
			            public void run() {
			            	try {
					    		
					    		//Getting the selected file
						    	File folder = fileChooser.getSelectedFile();
						    	
						    	File configFile = new File( folder.getAbsolutePath() + File.separator + "cfg.cfg" ); 
						    	File[] buttonFiles = new File[16];
						    	
						    	try {
						    		configFile.createNewFile();
						    		for ( int i = 0; i < 16; ++i )
							    	{
							    		buttonFiles[i] = new File( folder.getAbsolutePath() + File.separator + (i+1) + ".dat" );
							    		buttonFiles[i].createNewFile();
							    	}
						    		config.writeStructConfig( configFile );
						    		tracks.writeData( buttonFiles );
							    	
						    	} catch( IOException ex ) {
						    		throw new InvalidFileException( ex );
						    	}
						    	
					    	} catch (InvalidFileException ex) {
					    		
					    		JOptionPane.showMessageDialog(that,
									    ex.getMessage(),
									    "Error",
									    JOptionPane.ERROR_MESSAGE);
					    		
					    	}
			            	dialog.setVisible( false );
			            }
			         });
			    	t.setDaemon( true );
			    	t.start();
			    }
				
			}}
		);
		exit.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				//Closing the frame
				that.dispose();				
			}});
		
		//Adding an actionListener to the about button
		about.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				JOptionPane.showMessageDialog(that,
					    "Version 0.1 of ENGG4810 Group 27 Configuration",
					    "About",
					    JOptionPane.INFORMATION_MESSAGE);
				
			}}
		);
		
		// Initialising configuration
		config = new Configuration();
		for ( int i = 0; i < Configuration.NUM_BUTTONS; ++i )
			config.buttons[i] = new ButtonConfiguration();
		
		// Initialising panels
		buttons = new ButtonPanel( config);
		tracks = new TrackPanel( );
		options = new OptionsPanel( config );
		
		// Creating layout
		GridBagLayout gl = new GridBagLayout();
		this.setLayout( gl );
		
		GridBagConstraints c = new GridBagConstraints();
		c.gridx = 0;
		c.gridy = 0;
		c.gridheight = 1;
		c.gridwidth = 1;
		this.add( buttons, c );
		c.gridy = 1;
		this.add( options, c );
		c.gridx = 1;
		c.gridy = 0;
		c.gridheight = 2;
		this.add( tracks, c );
		
		this.pack();
	}
	
	void redraw() {
		buttons.redraw();
		options.redraw();
	}

}
