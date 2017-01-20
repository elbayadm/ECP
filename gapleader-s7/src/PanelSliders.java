


import java.awt.Font;
import java.io.IOException;
import java.util.Hashtable;
import java.util.Iterator;

import javax.swing.*;
import javax.swing.border.TitledBorder;
public class PanelSliders extends JPanel {
	
	private static final long serialVersionUID = 1L;
	/**
	 * 
	 */
	public Application app;
	public Hashtable<task,coupleText> alphaTasks;
	
	// Constructor
	public PanelSliders(Application app){
		this.setLayout(StyleEff.disp2);
	    this.setLayout(StyleEff.disp2);
	    this.setBorder(StyleEff.borderlr);
//Treating tasks
        getTasks T=new getTasks(app.xls,app.L);
			try {
				T.copyTasks();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
	      this.alphaTasks= new Hashtable<task,coupleText>();
	      Iterator<task> it=app.L.Tasks.iterator();
	      Font font1=new Font("SansSerif", Font.BOLD, 11);
	      while(it.hasNext()){
	    	  task t=it.next();
	    	  TitledBorder title= BorderFactory.createTitledBorder(StyleEff.brd,t.name,0,0,font1);
	    	  BoundedRangeModel brm = new DefaultBoundedRangeModel();
	          brm.setExtent(0);
	          brm.setMaximum(10);
	          brm.setMinimum(0);
	    	  JSlider b=new JSlider(brm);
	    	  b.setMajorTickSpacing(2);
	    	  b.setMinorTickSpacing(1);
	          b.setPaintTicks(true);
	          b.setSnapToTicks(true);
	          b.setPaintTrack(true);
	          b.setPaintLabels(true);
	          b.setBorder(title);
	          
	    	  coupleText couple=new coupleText(title,b);
	    	  this.alphaTasks.put(t, couple); 
	    	  this.add(this.alphaTasks.get(t).alpha); 
	    
	}
	    
	  
	}
	}
