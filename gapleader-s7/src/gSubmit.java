import java.awt.event.*;
import java.io.IOException;
import java.util.Iterator;

class gSubmit implements ActionListener {
    private Application application;
    

    public gSubmit(Application appli) {
        this.application = appli;
    }

    public void actionPerformed(ActionEvent e) {
    	this.application.Results.clearAll();
    	Application app=this.application;
		getBeta B=new getBeta(app.xls,app.L);
		try {
			B.copyBeta();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
    	PanelInputs temp=app.Inputs;
    	PanelSliders sl=app.Sliders;
    	app.L.quantity= Double.parseDouble(temp.Q.getText());
    	app.L.workload= Double.parseDouble(temp.W.getText());
    	app.L.scrapRate_withoutGL= Double.parseDouble(temp.r.getText())/100;
    	app.L.shiftTime= Double.parseDouble(temp.T.getText());
    	app.L.directGL= Double.parseDouble(temp.alphaDirect.getText())/100;
    	Iterator<task> it=app.L.Tasks.iterator();
        while(it.hasNext()){
      	  task t=it.next();
      	  t.alphaGL=sl.alphaTasks.get(t).alpha.getValue();
      	 
        	}
        
        Iterator<FailureCause> iter=app.L.causes.iterator();
		while(iter.hasNext()){
			FailureCause fc=iter.next();
			fc.setMTBF_withGL();
			fc.setMTTR_withGL();}
		app.L.setAll();
		app.ShowResults();
		
}
}