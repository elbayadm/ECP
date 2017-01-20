import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;


public class FailureCause {
	public Line line;
	public String name;
	public double MTBF_withGL;
	public double MTBF_withoutGL;
	public double MTTR_withGL;
	public double MTTR_withoutGL;
	public Hashtable<task,double[]> improvement_ratios;
	
	FailureCause(Hashtable<task,double[]> dict){
		improvement_ratios=dict;
	}

	@Override
	public String toString() {
		String res= "FailureCause [name=" + name + ", improvement_ratios=";
		Iterator<task> i=improvement_ratios.keySet().iterator();
		while (i.hasNext()){
			task t=i.next();
			double impMTBF=improvement_ratios.get(t)[0];
			double impMTTR=improvement_ratios.get(t)[1];
			res=res+t.name+"\t"+ "MTBF= "+impMTBF +"MTTR= "+impMTTR+"\t";
		}
		return res;
	}

	public void setMTBF_withGL(){
		ArrayList<task> t=this.line.Tasks;
		double temp=1;
		Iterator<task> i = t.iterator();
		while(i.hasNext()){
			task j=i.next();
			double d=this.improvement_ratios.get(j)[0];
			temp=temp*(1+d*Math.log(1+j.alphaGL)/(Math.log(11)));
			}
		this.MTBF_withGL=this.MTBF_withoutGL*temp;
	}

	public void setMTTR_withGL(){
		ArrayList<task> t=this.line.Tasks;
		double temp=1;
		Iterator<task> i = t.iterator();
		while(i.hasNext()){
			task j=i.next();
			double d=this.improvement_ratios.get(j)[1];
			temp=temp*(1-d*Math.log(1+j.alphaGL)/Math.log(11));
			}
		this.MTTR_withGL=this.MTTR_withoutGL*temp;
	}

}


