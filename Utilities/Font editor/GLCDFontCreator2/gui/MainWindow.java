/*
 * Created on 11.02.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package gui;

import gui.util.FontCreatorMenuBar;
import gui.util.FontCreatorSideToolBar;
import gui.util.FontCreatorTopToolBar;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

import javax.swing.JDesktopPane;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JInternalFrame;
import javax.swing.JTextField;

import util.Font;
import util.FontExport;
import util.SerializableFont;

/**
 * @author Fabian Maximilian Thiele
 */
public class MainWindow extends JFrame {
	private JDesktopPane desktop;
	private JTextField status;
	private int frameCount;
	
	private CharEditor selectedEditor;
	private FontCreatorSideToolBar sideToolbar;
	private FontCreatorTopToolBar topToolbar;
	
	public MainWindow() {
		frameCount = 0;
		
		initGUI();
		initFrame();
	}
	
	private void initGUI() {
		Container c = getContentPane();
		c.setLayout(new BorderLayout());

		FontCreatorMenuBar menubar = new FontCreatorMenuBar(this);
		setJMenuBar(menubar);

		topToolbar = new FontCreatorTopToolBar(this);
		c.add(BorderLayout.NORTH, topToolbar);

		sideToolbar = new FontCreatorSideToolBar(this);
		c.add(BorderLayout.WEST, sideToolbar);

		desktop = new JDesktopPane();
		c.add(BorderLayout.CENTER, desktop);
		
		status = new JTextField("ready");
		status.setEditable(false);
		c.add(BorderLayout.SOUTH, status);
	}
	
	private void initFrame() {
		setTitle("GLCD FontCreator 2");
		setSize(800, 600);
		Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
		setLocation(d.width/2-getWidth()/2, d.height/2-getHeight()/2);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
	}
	
	public void newFont() {
		NewFontDialog dialog = new NewFontDialog(this);
		int retVal = dialog.showNewFontDialog();
		if(retVal == NewFontDialog.APPROVE_OPTION) {
			Font font = dialog.getNewFont();

			FontEditor editor = new FontEditor(font, this);
			editor.setLocation(frameCount*30, frameCount*30);
			if(++frameCount == 5)
				frameCount = 0;
			desktop.add(editor);
			editor.setVisible(true);
		}
	}

	public void saveFont() {
		if(desktop.getSelectedFrame() instanceof FontEditor) {
			FontEditor editor = (FontEditor)desktop.getSelectedFrame();
			JFileChooser chooser = new JFileChooser(".");
			int rv = chooser.showSaveDialog(this);
			if(rv == JFileChooser.APPROVE_OPTION) {
				File dest = chooser.getSelectedFile();
				try {
					ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream(dest));
					SerializableFont font = new SerializableFont(editor.getEditedFont());
					out.writeObject(font);
					out.close();
				} catch (IOException e) {
					System.err.println("IOException: "+e.toString());
				}
			}
		}
	}

	public void loadFont() {
		JFileChooser chooser = new JFileChooser(".");
		int rv = chooser.showOpenDialog(this);
		if(rv == JFileChooser.APPROVE_OPTION) {
			File dest = chooser.getSelectedFile();
			try {
				ObjectInputStream in = new ObjectInputStream(new FileInputStream(dest));
				SerializableFont sFont = (SerializableFont) in.readObject();
				in.close();
				Font font = new Font(sFont);
				
				FontEditor editor = new FontEditor(font, this);
				editor.setLocation(frameCount*30, frameCount*30);
				if(++frameCount == 5)
					frameCount = 0;
				desktop.add(editor);
				editor.setVisible(true);
			} catch(Exception e) {
				System.err.println("Excpetion: "+e.toString());
			}
		}
	}

	public void exportFont() {
		if(desktop.getSelectedFrame() instanceof FontEditor) {
			FontEditor editor = (FontEditor)desktop.getSelectedFrame();
			FontExport export = new FontExport(this);
			export.exportFont(editor.getEditedFont());
		}
	}

	public void editFontSettings() {
		/*
		 * Not yet implemented
		 */
	}
	
	public void selectedEditorChanged(CharEditor selectedEditor) {
		this.selectedEditor = selectedEditor;
		
		sideToolbar.setColor(selectedEditor.getColor(), selectedEditor.getEditorColorModel());
		topToolbar.setZoom((selectedEditor.getZoom()));
	}
	
	public void setActiveEditorColor(int color) {
		if(selectedEditor != null)
			selectedEditor.setColor(color);
	}
	
	public void setActiveEditorTool(int tool) {
		if(selectedEditor != null)
			selectedEditor.setTool(tool);
	}
	
	public void setActiveEditorZoom(int zoom) {
		JInternalFrame frame = desktop.getSelectedFrame();
		if(frame instanceof FontEditor) {
			FontEditor editor = (FontEditor) frame;
			editor.setZoom(zoom);
		}
	}
	
	public void setStatusText(String text) {
		status.setText(text);
	}
}
