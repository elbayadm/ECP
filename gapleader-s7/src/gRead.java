
import org.apache.poi.hssf.usermodel.HSSFWorkbook;

import java.awt.HeadlessException;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileInputStream;

import java.io.IOException;
import java.util.ArrayList;

import javax.swing.JFileChooser;
import javax.swing.JOptionPane;




class gRead implements ActionListener {
    private Application application;

    public gRead(Application appli) {
        this.application = appli;
    }

    public void actionPerformed(ActionEvent e) {
    	
    	Application app=this.application;
         JFileChooser fc = new JFileChooser();
         fc.showOpenDialog(fc);
         String path=fc.getSelectedFile().getAbsolutePath();
    
         
         try {
			if(path.endsWith(".xls"))
			 {   if ( gRead.verifyxls(path)){
			     app.buttonChange.setEnabled(true);
			     app.xls=path;
			     app.PathSelected.setText(app.xls);}
			 else{JOptionPane.showMessageDialog(app.window, "Required sheets are missing");}}
			else{JOptionPane.showMessageDialog(app.window, "File format incompatible");}
		} catch (HeadlessException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		}
        	 
        	 
       public static boolean verifyxls (String path) throws IOException{
    	   ArrayList<String >Test=new ArrayList<String>();
    	   Test.add("scrap"); Test.add("MTBF"); Test.add("MTTR"); Test.add("tasks"); Test.add("initial");
    	   FileInputStream fileInputStream = new FileInputStream(path);
   		   HSSFWorkbook workbook = new HSSFWorkbook(fileInputStream);
   		   int nb= workbook.getNumberOfSheets();
   		   ArrayList<String> SheetNames= new ArrayList<String>();
   		   for (int i=0; i<nb ;i++){
   			   SheetNames.add(workbook.getSheetName(i));
   		   }
   		   return SheetNames.containsAll(Test);
   		   
   		   
    	   
       } 	 
        	 
        	
    	
    }
