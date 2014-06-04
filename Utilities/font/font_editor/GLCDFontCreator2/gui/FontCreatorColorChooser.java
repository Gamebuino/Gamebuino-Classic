/*
 * Created on 24.03.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.IndexColorModel;

import javax.swing.JComponent;

/**
 * @author Fabian Maximilian Thiele
 */
public class FontCreatorColorChooser extends JComponent implements MouseListener {
	private MainWindow mainWnd;
	
	private FontCreatorColorChooserFrame chooser;
	private IndexColorModel cm;
	private int color;

	public FontCreatorColorChooser(MainWindow mainWnd) {
		this.mainWnd = mainWnd;
		color = -1;
		setPreferredSize(new Dimension(40,30));
		setMaximumSize(new Dimension(40,35));
		addMouseListener(this);
		chooser = new FontCreatorColorChooserFrame(this); 
	}

	public void paint(Graphics g) {
		g.setColor(Color.GRAY);
		g.drawRect(5,5,30,20);
		if(cm != null && color > -1) {
			int red = cm.getRed(color);
			int green = cm.getGreen(color);
			int blue = cm.getBlue(color);
			g.setColor(new Color(red, green, blue));
			g.fillRect(6,6,29,19);
		} else {
			g.setColor(Color.LIGHT_GRAY);
			g.fillRect(6,6,29,19);
		}
	}

	public void setChoosedColor(int color) {
		this.color = color;
		mainWnd.setActiveEditorColor(color);
		repaint();
	}

	public void setColor(int color, IndexColorModel model) {
		this.color = color;
		cm = model;
		chooser.setColors(model);
		chooser.setCurrentColor(color);
		chooser.repaint();
		repaint();
	}

	public void mouseClicked(MouseEvent e) {
		if(e.getClickCount() % 2 == 0 && cm != null) {
			chooser.setVisible(true);
		}
	}

	public void mousePressed(MouseEvent arg0) {
		// unused
	}

	public void mouseReleased(MouseEvent arg0) {
		// unused
	}

	public void mouseEntered(MouseEvent arg0) {
		// unused
	}

	public void mouseExited(MouseEvent arg0) {
		// unused
	}
}
