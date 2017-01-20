
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;

import org.apache.poi.hssf.usermodel.HSSFSheet;
import org.apache.poi.hssf.usermodel.HSSFWorkbook;

public class getBeta {
	
		public Line actualLine;
		public String address;
		getBeta(String f ,Line L){
			actualLine=L;
			address=f;
		}
		public void copyBeta() throws IOException{
			FileInputStream fileInputStream = new FileInputStream(this.address);
			HSSFWorkbook workbook = new HSSFWorkbook(fileInputStream);
			HSSFSheet sheetMTBF = workbook.getSheet("MTBF");
			HSSFSheet sheetMTTR = workbook.getSheet("MTTR");
			HSSFSheet init = workbook.getSheet("initial");
			ArrayList<FailureCause> temp= new ArrayList<FailureCause>();
			//sheetMTBF & sheetMTTR have the same number of rows.
			for (int rn=1; rn<=sheetMTBF.getLastRowNum(); rn++){
				Hashtable<task,double[]> dict=new Hashtable<task,double[]>();
				FailureCause fc=new FailureCause(dict);
				fc.line=actualLine;
				fc.name=sheetMTBF.getRow(rn).getCell(0).getStringCellValue();
				Iterator<task> i = this.actualLine.Tasks.iterator();
				int j=1;
				while(i.hasNext()){
					task t = i.next();
					double impMTBF=sheetMTBF.getRow(rn).getCell(j).getNumericCellValue();
					double impMTTR=sheetMTTR.getRow(rn).getCell(j).getNumericCellValue();
					double[] imp={impMTBF,impMTTR};
					fc.MTBF_withoutGL=init.getRow(rn).getCell(1).getNumericCellValue();
					fc.MTTR_withoutGL=init.getRow(rn).getCell(2).getNumericCellValue();
					fc.improvement_ratios.put(t,imp);
					j++;
				}
				temp.add(fc);
			}
			this.actualLine.causes=temp;
			
			
		}
	}


