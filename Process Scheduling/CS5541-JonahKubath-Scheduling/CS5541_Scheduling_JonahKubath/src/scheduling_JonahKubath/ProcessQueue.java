package scheduling_JonahKubath;

public class ProcessQueue {
	QueueElement start;
	QueueElement end;
	
	public ProcessQueue() {
		start = null;
		end = start;
	}
	
	public void add(QueueElement data) {
		//Queue is empty
		if(start == null) {
			start = data;
			data.previous = null;
		}
		else {
			data.previous = end;
			end.next = data;
		}
		
		data.next = null;
		end = data;
		
	}
	
	public QueueElement pop() {
		//List is empty
		if(start == null) {
			return null;
		}
		else {
			QueueElement temp = start;
			start = start.next;
			if(start != null)
				start.previous = null;
		
			return temp;
		}
	}
	
	/*
	 * Return the first element in the queue
	 */
	public QueueElement getFront() {
		return start;
	}
	
	public void printQueue() {
		QueueElement temp = start;
		
		if(temp == null) {
			System.out.println("Empty List");
		}
		else {
			while(temp != null) {
				System.out.print(temp.data.processName + "->");
				temp = temp.next;
			}
			System.out.println();
		}
	}
	
	
}
