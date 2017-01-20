import java.awt.Color;
import java.awt.GridLayout;

import javax.swing.BorderFactory;
import javax.swing.border.*;
public class StyleEff {
	 //A specific border style
   public static Border borderleft=new EmptyBorder(5, 30, 5,20) ;
   public static Border borderlr=new EmptyBorder(5, 20, 5, 20) ;
   public static Border borderlrup=new EmptyBorder(20, 120, 5, 120) ;
   public static GridLayout disp2 = new GridLayout(0,2);
   public static GridLayout disp1 = new GridLayout(0,1);
   public static GridLayout disp3 = new GridLayout(0,3);
   public static Border brd=BorderFactory.createLineBorder(Color.GRAY);
   public static Border brd1=BorderFactory.createBevelBorder(0,Color.RED, Color.RED);
   public static Border brd2=BorderFactory.createBevelBorder(0,Color.GREEN, Color.GREEN);
   public static Border brdnoGL= BorderFactory.createTitledBorder(brd1,"Without GL");
   public static Border brdwGL= BorderFactory.createTitledBorder(brd2,"With GL");
   public static Border brdDistrib= BorderFactory.createTitledBorder(brd,"Operators distribution");
   

}
