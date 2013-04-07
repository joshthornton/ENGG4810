import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.filechooser.FileNameExtensionFilter;

import com.google.gson.Gson;
import com.google.gson.stream.JsonReader;


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
				JFileChooser fileChooser = new JFileChooser(".");
				
				//Ensuring only .for files can be opened
				fileChooser.setAcceptAllFileFilterUsed(false);
				
				//Ensuring only formulas are opened
				fileChooser.addChoosableFileFilter(new FileNameExtensionFilter(
						"Configurations", new String[] {"engg4810"}));
				
				//Displaying the file chooser
			    int status = fileChooser.showOpenDialog(that);
			    
			    if (status == JFileChooser.APPROVE_OPTION) {
			    	try {
				    	File selectedFile = fileChooser.getSelectedFile();
				    	FileReader reader = null;
				    	try {
				    		 reader = new FileReader( selectedFile );
				    	} catch ( FileNotFoundException ex ) {
				    		throw new InvalidFileException( ex );
				    	}
				    	JsonReader jsonReader = new JsonReader( reader );
				    	Gson gson = new Gson();
				    	config = gson.fromJson( jsonReader, Configuration.class );
				    	buttons.updateConfig( config );
				    	redraw();
			    	} catch (InvalidFileException ex) {
			    		
			    		JOptionPane.showMessageDialog(that,
							    ex.getMessage(),
							    "Error",
							    JOptionPane.ERROR_MESSAGE);
			    		
			    	}
			    }
				
			}}
		);
		save.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				
				//Creating a new file listener
				JFileChooser fileChooser = new JFileChooser(".");
				
				//Displaying the file chooser
			    int status = fileChooser.showSaveDialog(that);
			    
			    if (status == JFileChooser.APPROVE_OPTION) {
			    	try {
			    		
			    		
			    		//Getting the selected file
				    	File selectedFile = fileChooser.getSelectedFile();
				    	
				    	//Checking for correct file extension
				    	if (!selectedFile.getName().endsWith(".engg4810")){ 
				    		selectedFile = new File(selectedFile.getAbsolutePath() 
				    				+".engg4810"); 
				    	}
				    	
				    	try {
				    		FileWriter writer = new FileWriter( selectedFile );
				    		Gson gson = new Gson();
				    		String test = gson.toJson( config, Configuration.class );
					    	writer.write( test );
					    	writer.close();
				    	} catch( IOException ex ) {
				    		throw new InvalidFileException( ex );
				    	}
				    	
			    	} catch (InvalidFileException ex) {
			    		
			    		JOptionPane.showMessageDialog(that,
							    ex.getMessage(),
							    "Error",
							    JOptionPane.ERROR_MESSAGE);
			    		
			    	}
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
	}

}
