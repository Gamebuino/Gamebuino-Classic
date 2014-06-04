/*
 * Created on 24.03.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package gui.util;

import gui.CharEditor;
import gui.FontCreatorColorChooser;
import gui.MainWindow;

import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.IndexColorModel;

import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JToolBar;

/**
 * @author Fabian Maximilian Thiele
 */
public class FontCreatorSideToolBar extends JToolBar implements ActionListener {
	private MainWindow mainWnd;

	private FontCreatorColorChooser colorChooser;

	public FontCreatorSideToolBar(MainWindow mainWnd) {
		super(JToolBar.VERTICAL);
		this.mainWnd = mainWnd;
		
		initToolbar();
	}

	private void initToolbar() {
		colorChooser = new FontCreatorColorChooser(mainWnd);
		add(colorChooser);
		
		Insets toolButtonInsets = new Insets(3,3,3,3);
		
		JPanel tools = new JPanel(new GridLayout(6,1,5,5));
		
		JButton penTool = new JButton("Pen");
		penTool.setMargin(toolButtonInsets);
		penTool.setActionCommand("pen");
		penTool.addActionListener(this);
		penTool.setPreferredSize(new Dimension(40, 25));
		tools.add(penTool);
		
		JButton lineTool = new JButton("Line");
		lineTool.setMargin(toolButtonInsets);
		lineTool.setActionCommand("line");
		lineTool.addActionListener(this);
		lineTool.setPreferredSize(new Dimension(40, 25));
		tools.add(lineTool);
		
		JButton drawRectTool = new JButton("draR");
		drawRectTool.setMargin(toolButtonInsets);
		drawRectTool.setActionCommand("drawRect");
		drawRectTool.addActionListener(this);
		drawRectTool.setPreferredSize(new Dimension(40, 25));
		tools.add(drawRectTool);
		
		JButton fillRectTool = new JButton("fillR");
		fillRectTool.setMargin(toolButtonInsets);
		fillRectTool.setActionCommand("fillRect");
		fillRectTool.addActionListener(this);
		fillRectTool.setPreferredSize(new Dimension(40, 25));
		tools.add(fillRectTool);
		
		JButton drawOvalTool = new JButton("draO");
		drawOvalTool.setMargin(toolButtonInsets);
		drawOvalTool.setActionCommand("drawOval");
		drawOvalTool.addActionListener(this);
		drawOvalTool.setPreferredSize(new Dimension(40, 25));
		tools.add(drawOvalTool);
		
		JButton fillOvalTool = new JButton("fillO");
		fillOvalTool.setMargin(toolButtonInsets);
		fillOvalTool.setActionCommand("fillOval");
		fillOvalTool.addActionListener(this);
		fillOvalTool.setPreferredSize(new Dimension(40, 25));
		tools.add(fillOvalTool);
		
		tools.setMaximumSize(new Dimension(45,180));
		add(tools);
	}

	/* (non-Javadoc)
	 * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	public void actionPerformed(ActionEvent e) {
		String cmd = e.getActionCommand();
		if(cmd.equals("pen")) {
			mainWnd.setActiveEditorTool(CharEditor.TOOL_FREEHAND);
		} else if(cmd.equals("line")) {
			mainWnd.setActiveEditorTool(CharEditor.TOOL_STRAIGHT_LINES);
		} else if(cmd.equals("drawRect")) {
			mainWnd.setActiveEditorTool(CharEditor.TOOL_DRAW_RECT);
		} else if(cmd.equals("fillRect")) {
			mainWnd.setActiveEditorTool(CharEditor.TOOL_FILL_RECT);
		} else if(cmd.equals("drawOval")) {
			mainWnd.setActiveEditorTool(CharEditor.TOOL_DRAW_OVAL);
		} else if(cmd.equals("fillOval")) {
			mainWnd.setActiveEditorTool(CharEditor.TOOL_FILL_OVAL);
		}
	}

	public void setColor(int color, IndexColorModel cm) {
		colorChooser.setColor(color, cm);
	}
}
