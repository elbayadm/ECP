import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;




public class Line {
	public double workstation_MTBF_noGL;
	public double workstation_MTTR_noGL;
	public double minAV_noGL;
	public double minAV_wGL;
	public double workstation_availability_noGL;
	public double workstation_MTBF_withGL;
	public double workstation_MTTR_withGL;
	public double workstation_availability_withGL;
	public double workload;
	public double quantity;
	public double scrapRate_withoutGL;
	public double scrapRate_withGL;
	public double shiftTime;
	public int NwithouthGL;
	public int NwithGL;
	public double directGL;
	public ArrayList<FailureCause> causes;
	public ArrayList<task> Tasks;
	public double occupation;
	public int maxNperGL;
	public Hashtable<String,Integer> GLdistrib;
	
	public void setMTBF_noGL(){
		double temp=0;
		Iterator<FailureCause> it=this.causes.iterator();
		while(it.hasNext()){
			FailureCause fc=it.next();
			temp+=1/fc.MTBF_withoutGL;}
		this.workstation_MTBF_noGL=1/temp;
	}
	public void setMTTR_noGL(){
		double temp=0;
		Iterator<FailureCause> it=this.causes.iterator();
		while(it.hasNext()){
			FailureCause fc=it.next();
			temp+=fc.MTTR_withoutGL;}
		this.workstation_MTTR_noGL=temp/this.causes.size();
		
	}
	public void setminAV_noGL(){
		this.minAV_noGL=(this.quantity*this.workload)/((1-this.scrapRate_withoutGL)*this.shiftTime+this.quantity*this.workload);
	}
	public void setminAV_wGL(){
		this.minAV_wGL=(this.quantity*this.workload)/((1-this.scrapRate_withGL)*this.shiftTime*(1+this.directGL/this.maxNperGL)+this.quantity*this.workload);
	}
	public void setavailabilityL_noGL(){
		this.workstation_availability_noGL=this.workstation_MTBF_noGL/(this.workstation_MTBF_noGL+this.workstation_MTTR_noGL);
		}
	public void setNwithoutGL(){
		double q=this.workload*this.quantity/(1-this.scrapRate_withoutGL);
		double t=q*(1/this.workstation_availability_noGL-1);
		double temp=q/(this.shiftTime-t);
	if (this.shiftTime>t){
		this.NwithouthGL=(int) Math.floor(temp)+1;}
	else {this.NwithouthGL=(int) Double.NEGATIVE_INFINITY;}}
	
	public void setMTBF_withGL(){
		double temp=0;
		Iterator<FailureCause> it=this.causes.iterator();
		while(it.hasNext()){
			FailureCause fc=it.next();
			temp+=1/fc.MTBF_withGL;}
		this.workstation_MTBF_withGL=1/temp;
	}
	public void setMTTR_withGL(){
		double temp=0;
		Iterator<FailureCause> it=this.causes.iterator();
		while(it.hasNext()){
			FailureCause fc=it.next();
			temp+=fc.MTTR_withGL;}
		this.workstation_MTTR_withGL=temp/this.causes.size();
	}
	public void setavailabilityL_withGL(){
	this.workstation_availability_withGL=this.workstation_MTBF_withGL/(this.workstation_MTBF_withGL+this.workstation_MTTR_withGL);
	}
	public void setscrapRate_withGL(){
	double temp=1;
	Iterator<task> it=Tasks.iterator();
	while(it.hasNext()){
		task t=it.next();
		temp*=1-t.scrap_imp*Math.log(1+t.alphaGL)/Math.log(11);	
	} 
	this.scrapRate_withGL=this.scrapRate_withoutGL*temp;
}
	
	public void setNwithGL(){
	double q=this.workload*this.quantity/(1-this.scrapRate_withGL);
	double t=q*(1/this.workstation_availability_withGL-1);
	double temp=q/(this.shiftTime*(1+this.directGL/this.maxNperGL)-t);
	if (this.minAV_wGL<this.workstation_availability_withGL){
		this.NwithGL=(int) Math.floor(temp)+1;}
	else this.NwithGL=(int) Double.NEGATIVE_INFINITY;}

	public double setOccupation(int n){
		double total=this.directGL*this.shiftTime;
		Iterator<task> it=Tasks.iterator();
		while(it.hasNext()){
			task t=it.next();
			double dep=1;
			if (t.N_dependancy==1){dep=n;}
			total=total+t.frequency*t.duration*dep*t.alphaGL/10;
		}
		return total;
		
	}
	public void setmaxNperGL(){
		double A=0;
		double B=0;
		Iterator<task> it=Tasks.iterator();
		while(it.hasNext()){
			task t=it.next();
			if (t.N_dependancy==0) A=A+t.frequency*t.duration*t.alphaGL/10;
			if (t.N_dependancy==1) B=B+t.frequency*t.duration*t.alphaGL/10;
		}
		if (B!=0){
			if((shiftTime-A-shiftTime*directGL)/B>=1)
				{double d=(shiftTime-A-shiftTime*directGL)/B;
					this.maxNperGL=(int) Math.floor(d);}
			else{this.maxNperGL=0;}}
		else {
			if(shiftTime-A-shiftTime*directGL>0)
			this.maxNperGL=12344578;
			else{this.maxNperGL=0;}}
		}
	
	
	
	
	
	public void setGLdistrib(){
		int Max=this.maxNperGL;
	
		this.GLdistrib= new Hashtable<String,Integer>();
		double Req_time=this.occupation;
		int i=1;
		int remain=this.NwithGL;
		if (Max!=12344578 && remain<=Max  )
		{this.GLdistrib.put("GL1", remain);}
		if (Max!=12344578 && remain>Max  )
		{while (remain>=Max){
			this.GLdistrib.put("GL"+i, Max);
			Req_time=Req_time-this.setOccupation(Max);
			remain=remain-Max;
			i=i+1;
			}
		if (remain!=0) { this.GLdistrib.put("GL"+i, remain);
		Req_time=Req_time-this.setOccupation(remain);}
		}
		if (Max==12344578){if (this.occupation<=this.shiftTime){
			this.GLdistrib.put("GL"+i, remain);
		}
		
			}
			
			}

	public void setAll(){
		//without GL
		this.setMTBF_noGL();
		this.setMTTR_noGL();
		this.setavailabilityL_noGL();
		this.setminAV_noGL();
		this.setNwithoutGL();
		//with GL

		this.setMTBF_withGL();
		this.setMTTR_withGL();
		this.setmaxNperGL();
		this.setavailabilityL_withGL();
		this.setscrapRate_withGL();
		this.setminAV_wGL();
		this.setNwithGL();
		this.occupation=this.setOccupation(this.NwithGL);
		
		if (this.maxNperGL!=0) 
			this.setGLdistrib();
		
	}
}
