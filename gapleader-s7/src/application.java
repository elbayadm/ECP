import javax.swing.*;

import java.awt.*;
import java.text.DecimalFormat;
import java.text.ParseException;
import java.util.Iterator;


class Application {
	public Line L;
	public String xls;
    public JFrame window=new JFrame("Gap Leader v.0"); 

    // different Panels
    public PanelInputs Inputs;
    public PanelResults Results;
    public PanelSliders Sliders;
    public JPanel PanelButtons= new JPanel();
    public JPanel Select = new JPanel();
    public JScrollPane sp;
  
    //buttons
    public JButton buttonSubmit= new JButton("Submit");;
    public JButton buttonQuit= new JButton("Exit");
    public JButton buttonChange= new JButton("Proceed");
    public JButton buttonSelect= new JButton("Select an xls");
    public JTextField PathSelected= new JTextField(20);
   
  // Constructor

    public Application() throws ParseException {
    	ImageIcon img = new ImageIcon("logo.png");
    	window.setIconImage(img.getImage());
    	
      this.L=new Line();
      this.Results= new PanelResults();
      this.Inputs= new PanelInputs();

     
      //Defining Actions
      //
      gQuit quit = new gQuit();
      
      this.buttonQuit.addActionListener(quit);

      gSubmit sbm = new gSubmit(this);
      
      this.buttonSubmit.addActionListener(sbm);
      gRead read= new gRead(this);
      this.buttonSelect.addActionListener(read);
      gProceed change= new gProceed(this);
      this.buttonChange.addActionListener(change);
      
     // adding other objects to container
      PanelButtons.setLayout(StyleEff.disp3);
      PanelButtons.setBorder(StyleEff.borderlrup);
      buttonSubmit.setEnabled(false);
      PanelButtons.add(this.buttonSubmit);
      PanelButtons.add(this.buttonQuit);
      
      
     // Adding Panels to Frame
      Select.setLayout(new BorderLayout());
      JPanel buttonsUP = new JPanel();
      buttonsUP.add(this.buttonSelect);
      this.buttonChange.setEnabled(false);
      buttonsUP.add(this.buttonChange);
      this.PathSelected.setPreferredSize(new Dimension(10,40));
      buttonsUP.add(this.PathSelected);
      Select.add(buttonsUP, BorderLayout.PAGE_START);
      sp= new JScrollPane(Select);
      sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
      sp.setBounds(10, 10, 10, 10);
      sp.setPreferredSize(new Dimension( 300, 400 ));
      window.add(sp,BorderLayout.PAGE_START );
     
      window.add(Inputs, BorderLayout.LINE_START);
      window.add(Results, BorderLayout.CENTER);
      window.add(PanelButtons, BorderLayout.PAGE_END);
    
      this.fontChange(Inputs);
      this.fontChange(Results.PanelResultsnoGL);
      this.fontChange(Results.PanelResultswGL);
     
      
      
     
     
      
      // Show window
      //
      this.window.pack();
      this.window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
      this.window.setPreferredSize(new Dimension(600, 400));
      this.window.setVisible(true);
    }

    public void ShowResults(){
    	// A specific float format
        DecimalFormat df = new DecimalFormat("#.##");
        DecimalFormat di = new DecimalFormat("#");
    	this.Results.availnoGL.setText(df.format(L.workstation_availability_noGL*100)+"%");
    	this.Results.minAV_noGL.setText(df.format(L.minAV_noGL*100)+"%");
    	if (L.workstation_availability_noGL>L.minAV_noGL) {this.Results.NnoGL.setText(di.format(L.NwithouthGL));}
    	else {	this.Results.NnoGL.setText(di.format(L.NwithouthGL));
    			JOptionPane.showMessageDialog(window, "This line wouldn't work without GL.");
    	}
    	
    	
    		
    
    	
    	this.Results.availwGL.setText(df.format(L.workstation_availability_withGL*100)+"%");
    	this.Results.newScrap.setText(df.format(L.scrapRate_withGL*100)+"%");
    	this.Results.minAV_wGL.setText(df.format(L.minAV_wGL*100)+"%");
    	if (L.workstation_availability_withGL>L.minAV_wGL){
    	if (L.maxNperGL==0){JOptionPane.showMessageDialog(window, "Too many tasks for a GL to handle.\n Even with N=1, Total time= "+this.L.setOccupation(1)+" s");}
    	else{this.Results.NwGL.setText(di.format(L.NwithGL));
    	Iterator<String> s = L.GLdistrib.keySet().iterator();
    	int i=0;
    	while(s.hasNext()){
    		String GL=s.next();
    		int n= L.GLdistrib.get(GL);
    		double occup=L.setOccupation(n);
    		this.Results.GLdistrib.setValueAt(GL, i, 0);
    		this.Results.GLdistrib.setValueAt(Integer.toString(n), i, 1);
    		this.Results.GLdistrib.setValueAt(df.format(occup), i, 2);
    		i++;
    	}}}
    	else  {
    		this.Results.NwGL.setText(di.format(L.NwithGL));
    		JOptionPane.showMessageDialog(window, "Even with GL, this line wouldn't work.");
    	}
    	
    	
    	
    	
    	
    }
   
   public void fontChange(JPanel panel){	
    	 //Font
        Font font1=new Font("SansSerif", Font.BOLD, 11);
     
        Component[] L=panel.getComponents();
       for (int i=0; i<L.length;i++){
      	 L[i].setFont(font1); 
       } 	}
    
  

}



