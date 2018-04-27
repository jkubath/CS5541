package scheduling_JonahKubath;

public class QueueElement {
	QueueElement next;
	QueueElement previous;
	Process data;
	
	public QueueElement() {
		
	}
	
	public QueueElement(Process pData) {
		//copy the data
		Process temp = new Process(pData.processName, 
				pData.arrival, pData.service);
		data = temp;
		next = previous = null;
	}
	
	
	
}
