import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;

import org.apache.poi.hssf.usermodel.HSSFSheet;
import org.apache.poi.hssf.usermodel.HSSFWorkbook;


public class getTasks {
	public Line actualLine;
	public String Fileaddress;
	getTasks(String f ,Line L){
		actualLine=L;
		Fileaddress=f;
	}
	public void copyTasks() throws IOException{
		FileInputStream fileInputStream = new FileInputStream(Fileaddress);
		HSSFWorkbook workbook = new HSSFWorkbook(fileInputStream);
		HSSFSheet sheet = workbook.getSheet("tasks");
		HSSFSheet scrapsheet = workbook.getSheet("scrap");
		ArrayList<task> temp= new ArrayList<task>();
		for (int rn=1; rn<=sheet.getLastRowNum(); rn++){
			String Name=sheet.getRow(rn).getCell(1).getStringCellValue();
			double duration=sheet.getRow(rn).getCell(2).getNumericCellValue();
			double freq=sheet.getRow(rn).getCell(3).getNumericCellValue();
			int depend=(int) sheet.getRow(rn).getCell(4).getNumericCellValue();
			double scrapimp=scrapsheet.getRow(rn).getCell(2).getNumericCellValue();
			task t=new task();
			t.name=Name; 
			t.duration=duration;
			t.frequency=freq;
			t.N_dependancy=depend;
			t.scrap_imp=scrapimp;
			temp.add(t);
	     }
		this.actualLine.Tasks=temp;
		
	}
}
