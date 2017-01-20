

import java.awt.Dimension;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.text.NumberFormat;
import java.text.ParseException;

import javax.swing.*;

public class PanelInputs extends JPanel
	implements PropertyChangeListener{
	/**
	 * 
	 */
	//Initial Values
	private double Qi = 600;
    private double Ti = 25000;
    private double ri = 6;
    private double alphai = 30; 
    private double Wi= 60;
	
	//JLabels
	private static final long serialVersionUID = 1L;
	public JLabel Quantity= new JLabel("Quantity (u):",JLabel.RIGHT);
    public JLabel Workload= new JLabel("Workload (s):",JLabel.RIGHT) ;
    public JLabel scrap=new JLabel("Scrap rate (%):",JLabel.RIGHT) ;
    public JLabel ShiftTime= new JLabel("ShiftTime (s):",JLabel.RIGHT) ;
    public JLabel Direct= new JLabel("GL direct time (%):",JLabel.RIGHT);
    
    //JTextFields
    JFormattedTextField Q;
    JFormattedTextField W; 
    JFormattedTextField T;
    JFormattedTextField r; 
    JFormattedTextField alphaDirect;
    public NumberFormat amountFormat;
    public NumberFormat percentFormat;
    

 // Constructor
    

    public PanelInputs() throws ParseException {
   
    	 Q= new JFormattedTextField(amountFormat);
    	 Q.setPreferredSize(new Dimension(10,30));
    	 Q.setValue(new Double(Qi));
    	 Q.addPropertyChangeListener("value", this);
    	 ///
         T = new JFormattedTextField(amountFormat);
         T.setPreferredSize(new Dimension(10,30));
         T.setValue(new Double(Ti));
    	 T.addPropertyChangeListener("value", this);
    	 ///
         W =new JFormattedTextField(amountFormat);
         W.setPreferredSize(new Dimension(10,30));
         W.setValue(new Double(Wi));
    	 W.addPropertyChangeListener("value", this);
    	 ///
         r = new JFormattedTextField(percentFormat);
         r.setPreferredSize(new Dimension(10,30));
         r.setValue(new Double(ri));
    	 r.addPropertyChangeListener("value", this);
    	 ///
         alphaDirect =  new JFormattedTextField(percentFormat);
         alphaDirect.setPreferredSize(new Dimension(10,30));
         alphaDirect.setValue(new Double(alphai));
         alphaDirect.addPropertyChangeListener("value", this);
    	 ////
         Quantity.setLabelFor(Q);
         Workload.setLabelFor(W);
         scrap.setLabelFor(r);
         ShiftTime.setLabelFor(T);
         Direct.setLabelFor(alphaDirect);
         ///
    	 this.setLayout(StyleEff.disp2);
         this.setBorder(StyleEff.borderleft);
         
         this.add(this.Quantity);
         this.add(Q);
         this.add(this.Workload);
         this.add(this.W);
         this.add(this.scrap);
         this.add(this.r);
         this.add(this.ShiftTime);
         this.add(this.T);
         this.add(this.Direct);
         this.add(this.alphaDirect);
    }


	@Override
	public void propertyChange(PropertyChangeEvent e) {
		Object source = e.getSource();
        if (source == Q) {
            Qi = ((Number)Q.getValue()).doubleValue();
        } else if (source == T) {
            Ti = ((Number)T.getValue()).doubleValue();
        } else if (source == W) {
            Wi = ((Number)W.getValue()).doubleValue();
        }else if (source == r) {
            ri = ((Number)r.getValue()).doubleValue();
        }else if (source == alphaDirect) {
            alphai = ((Number)alphaDirect.getValue()).doubleValue();
        }

    }
	}


