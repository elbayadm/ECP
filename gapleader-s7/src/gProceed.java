import java.awt.BorderLayout;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;


import javax.swing.SwingUtilities;

class gProceed implements ActionListener {
    private Application application;

    public gProceed(Application appli) {
        this.application = appli;
    }

    public void actionPerformed(ActionEvent e) {
    	
    	 Application app=this.application;
    	 PanelSliders sld= new PanelSliders(app);
         app.Sliders=sld;
         app.Select.add(sld, BorderLayout.CENTER);
         app.buttonSubmit.setEnabled(true);
         SwingUtilities.updateComponentTreeUI(app.Select);
     
 		}
    	
    }
