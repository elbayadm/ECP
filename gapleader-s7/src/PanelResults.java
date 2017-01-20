







import javax.swing.*;
public class PanelResults extends JPanel {
	
	private static final long serialVersionUID = 1L;
	/**
	 * 
	 */
	// Assemblies
	 public JPanel PanelResultsnoGL = new JPanel();
	 public JPanel PanelResultswGL = new JPanel();
	 public JPanel PanelDistrib = new JPanel();
	 public JScrollPane scdistrib;
	 //Results labels
	public JLabel minAV_noGL_label=new JLabel("Min Availability", JLabel.RIGHT);
	public JLabel minAV_wGL_label=new JLabel("Min Availability ", JLabel.RIGHT);
    public JLabel availnoGL_label=new JLabel("Availability: ",JLabel.RIGHT);
    public JLabel NnoGL_label=new JLabel("Required N: ",JLabel.RIGHT);
    public JLabel availwGL_label=new JLabel("Availability: ",JLabel.RIGHT);
    public JLabel newScrap_label=new JLabel("Scrap rate: ",JLabel.RIGHT);
    public JLabel NwGL_label=new JLabel("Required N: ",JLabel.RIGHT);
   
   
    
    
    //Results texts
    public JTextField availnoGL= new JTextField(4);
    public JTextField NnoGL= new JTextField(4);
    public JTextField availwGL= new JTextField(4);
    public JTextField newScrap= new JTextField(4);
    public JTextField NwGL= new JTextField(5);
   
    public JTextField minAV_noGL= new JTextField(3);
    public JTextField minAV_wGL= new JTextField(3);
    public JTable GLdistrib;
 // Constructor
    
    public PanelResults(){
        PanelDistrib.setLayout(new BoxLayout(PanelDistrib,BoxLayout.PAGE_AXIS));
    	PanelResultswGL.setLayout(StyleEff.disp2);
    	PanelDistrib.setBorder(StyleEff.brdDistrib);
        PanelResultswGL.setBorder(StyleEff.brdwGL);
        PanelResultsnoGL.setLayout(StyleEff.disp2);
        PanelResultsnoGL.setBorder(StyleEff.brdnoGL);
        PanelResultsnoGL.add(this.minAV_noGL_label);
        minAV_noGL.setEditable(false);
        PanelResultsnoGL.add(this.minAV_noGL);
        minAV_noGL_label.setLabelFor(minAV_noGL);
        PanelResultswGL.add(this.minAV_wGL_label);
        minAV_wGL.setEditable(false);
        PanelResultswGL.add(this.minAV_wGL);
        minAV_noGL_label.setLabelFor(minAV_wGL);
        PanelResultsnoGL.add(this.availnoGL_label);
        availnoGL.setEditable(false);
        PanelResultsnoGL.add(this.availnoGL);
        PanelResultsnoGL.add(this.NnoGL_label);
        NnoGL.setEditable(false);
        PanelResultsnoGL.add(this.NnoGL);
        //
       
        PanelResultswGL.add(this.availwGL_label);
        availwGL.setEditable(false);
        PanelResultswGL.add(this.availwGL);
        PanelResultswGL.add(this.newScrap_label);
        newScrap.setEditable(false);
        PanelResultswGL.add(this.newScrap);
        PanelResultswGL.add(this.NwGL_label);
        NwGL.setEditable(false);
        PanelResultswGL.add(this.NwGL);
        String[] columnNames = {"GL","N" ,"Time"};
        String[][] data={{"","",""},{"","",""},{"","",""},{"","",""},{"","",""},{"","",""},{"","",""},{"","",""},{"","",""},{"","",""}};
        GLdistrib = new JTable(data,columnNames);
        PanelDistrib.add(GLdistrib.getTableHeader());
        PanelDistrib.add(this.GLdistrib);
        scdistrib= new JScrollPane(PanelDistrib);
        
        
        /// mixing the three panels
        this.setLayout(StyleEff.disp3);
        this.setBorder(StyleEff.borderleft);
        this.add(PanelResultsnoGL);
        this.add(PanelResultswGL);
        this.add(scdistrib);
    }
    
    //Clear all text fields
    public void clearAll(){
    	availnoGL.setText(null);
    	NnoGL.setText(null);
    	availwGL.setText(null);
    	newScrap.setText(null);
    	NwGL.setText(null);
    	
    	minAV_noGL.setText(null);
    	minAV_wGL.setText(null);
    	int rows =GLdistrib.getRowCount();
    	int columns = GLdistrib.getColumnCount();
    	for (int i=0; i<rows; i++){
    		for (int j=0; j<columns;j++)
    		{
    			GLdistrib.setValueAt(null, i, j);
    		}
    	}
  
    	
    }
    
}
