package scheduling_JonahKubath;

import java.util.ArrayList;
import java.util.HashMap;

public class Real_Time_Periodic {
	static ArrayList<Process> processList = Main.processList;
	static Integer[] process;
	static HashMap<String, Integer> priority = new HashMap<String, Integer>();
	static int processCount;
	static int totalTime;
	
	/**
	 * Set class data
	 * @param count The process count
	 * @param time The total time to run
	 */
	public static void setData(int count, int time) {
		processCount = count;
		totalTime = time;
		process = new Integer[processCount];
		
		for(int i = 0; i < processCount; i++) {
			process[i] = 1;
			priority.put(processList.get(i).processName, i);
		}
	}
	
	/**
	 * Fixed Priority
	 */
	public static void fp() {
		int time = 0;
		int start = 0;
		int index = 0;
		ArrayList<Process> pList = new ArrayList<Process>();
		
		//printList(processList);
		
		//Run for the given time period
		while(time < totalTime) {
			//Add any processes that have met their time period
			while(index < processCount) {
				if(time % processList.get(index).end_deadline == 0) {
					int placement = 0;
					while(placement < pList.size() && 
							priority.get(processList.get(index).processID) >= priority.get(pList.get(placement).processID)) {
						placement++;
					}
					
					//Is the newly process pre-empting the current process
					if(placement == 0 && pList.size() != 0) {
						printChange(pList.get(0), start, time);
						start = time;
					}
					
					
					Process temp = Process.copyProcess(processList.get(index));
					
					temp.arrival = 20 * (process[index] - 1);
					temp.end_deadline *= (process[index]);
					temp.processName += "(" + process[index] + ")";
					process[index]++;
					
					pList.add(placement, temp);
					
				}
				index++;
			}
			index = 0;
			
			//printList(pList);
			
			time++;
			
			//Are there processes to run?
			if(pList.size() != 0) {
				int pastDeadline = 0;
				while(pastDeadline < pList.size() && pList.get(pastDeadline).end_deadline < time) {
					System.out.println(pList.get(pastDeadline).processName + ":Missed");
					pList.remove(pastDeadline);
					pastDeadline++;
				}
				
				/* Check to make sure we haven't deleted
				 * the entire list by missed deadlines
				 */
				if(pList.size() != 0) {
					pList.get(0).service--;
					//Current process is done running
					if(pList.get(0).service == 0) {
						printChange(pList.get(0), start, time);
						
						//Remove the current process
						pList.remove(0);
						
						start = time;
					}
				}
			}
			
			
			
	
		} //End of time
		
	}
	
	/**
	 * Earliest Deadline with Completion Deadlines
	 */
	public static void edcd() {
		int time = 0;
		int start = 0;
		int index = 0;
		ArrayList<Process> pList = new ArrayList<Process>();
		resetProcessArray();
		
		//printList(processList);
		
		//Run for the given time period
		while(time < totalTime) {
			//Add any processes that have met their time period
			while(index < processCount) {
				if(time % processList.get(index).end_deadline == 0) {
					int placement = 0;
					
					Process temp = Process.copyProcess(processList.get(index));
					temp.arrival = 20 * (process[index] - 1);
					temp.end_deadline *= (process[index]);
					temp.processName += "(" + process[index] + ")";
					process[index]++;
					
					//Sort based on end_deadline
					while(placement < pList.size() && 
						temp.end_deadline >= pList.get(placement).end_deadline) {
						placement++;
					}
					
					//Is the newly process pre-empting the current process
					if(placement == 0 && pList.size() != 0) {
						printChange(pList.get(0), start, time);
						start = time;
					}
					
					//Add the process
					
					
					pList.add(placement, temp);
				}
				index++;
			}
			index= 0;
			
			//printList(pList);
			
			time++;
			
			//Are there processes to run?
			if(pList.size() != 0) {
				//Remove any processes that missed their deadline
				int pastDeadline = 0;
				while(pastDeadline < pList.size() && pList.get(pastDeadline).end_deadline < time) {
					System.out.println(pList.get(pastDeadline).processName + ":Missed");
					pList.remove(pastDeadline);
					pastDeadline++;
				}
				
				/* Check to make sure we haven't deleted
				 * the entire list by missed deadlines
				 */
				if(pList.size() != 0) {
					pList.get(0).service--;
					//Current process is done running
					if(pList.get(0).service == 0) {
						printChange(pList.get(0), start, time);
						
						//Remove the current process
						pList.remove(0);
						
						start = time;
					}
				}
			}
			
			
			
		}//End of time
		
	}
	
	/**
	 * Print the time range of the running process
	 * @param data The process object
	 * @param start start time
	 * @param end end time
	 */
	public static void printChange(Process data, int start, int end) {
		String print = String.format("%s:%d->%d", data.processName, start, end);
		System.out.println(print);
	}
	
	/**
	 * Print the given list
	 * @param list The list to print from
	 */
	public static void printList(ArrayList<Process> list) {
		if(list.size() <= 0)
			return;
		String print = "";
		for(int i = 0; i < list.size(); i++) {
			print = String.format("%s(%d)(%d) -> ", list.get(i).processName, list.get(i).end_deadline, list.get(i).service);
			System.out.print(print);
		}
		System.out.println();
	}
	
	/**
	 * Reset the process[] to 1
	 */
	public static void resetProcessArray() {
		for(int i = 0; i < process.length; i++) {
			process[i] = 1;
		}
	}
	
	
}
