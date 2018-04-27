package scheduling_JonahKubath;

import java.util.ArrayList;

public class Uniprocessor {
	//List of process to read from
	static ArrayList<Process> processList = Main.processList;
	static int processCount;
	static int quantum;
	
	/**
	 * Set the class data for Uniprocess
	 * @param count The process count
	 * @param time The time quantum (time-slice)
	 */
	public static void setData(int count, int time) {
		processCount = count;
		quantum = time;
	}
	
	/**
	 * First-Come-First-Served
	 */
	public static void fcfs() {
		int start = 0;
		int time = 0;
		
		/* Run the processes in order of arrival */
		for(int i = 0; i < processCount; i++) {
			//Wait for the next process to arrive if needed
			if(processList.get(i).arrival > time) {
				time = processList.get(i).arrival;
			}
			//Run the first process to created
			time += processList.get(i).service;
			//Print the change
			printChange(processList.get(i), start, time);
			start = time;
		}
	}
	
	/**
	 * Round Robin
	 */
	public static void rr() {
		int time = 0;
		int index = 0;
		int start = 0;
		int quanta = quantum;
		ProcessQueue queue = new ProcessQueue();
		
		//Run while processes need to run or while processes haven't been created yet
		while(queue.getFront() != null || index < processCount) {
			//New process has arrived
			while(index < processCount && processList.get(index).arrival == time) {
				QueueElement temp = new QueueElement(processList.get(index));
				queue.add(temp);
				index++;
			}
			
			time++;
			
			//Run the current process
			if(queue.getFront() != null) {
				queue.getFront().data.service--;
				quanta--;
				
				if(queue.getFront().data.service == 0) {
					printChange(queue.getFront().data, start, time);
					//Remove from the list and reset the run quantum
					queue.pop();
					quanta = quantum;
					start = time;
				}
				else if(quanta <= 0) {
					printChange(queue.getFront().data, start, time);
					//put the first element to the back of the line
					queue.add(queue.pop());
					quanta = quantum;
					start = time;
				}
				
			}
			
		} //End of all processes
		
	}
	
	/**
	 * Shortest Process Next
	 */
	public static void spn() {
		int time = 0;
		int start = 0;
		int index = 0;
		ArrayList<Process> pList = new ArrayList<Process>();
		
		while(index < processCount || pList.size() > 0) {
			while(index < processCount && processList.get(index).arrival <= time) {
				//System.out.println("Adding - " + processList.get(index).processName);
				int placement = 0;
				//Find the position to add the process to
				while(placement < pList.size() && processList.get(index).service > pList.get(placement).service) {
					placement++;
				}
				
				//Add the process in the correct order
				pList.add(placement, processList.get(index));
				index++;
			}
			
			//printList(pList);
			
			//Are there processes to run?
			if(pList.size() != 0) {
				start = time;
				time += pList.get(0).service;
				printChange(pList.get(0), start, time);
				//Remove the process when it is done running
				pList.remove(0);
			}
			else
				time++;
			
		} //End of all processes
		
	}
	
	/**
	 * Shortest Reamining Time
	 */
	public static void srt() {
		int time = 0;
		int start = 0;
		int index = 0;
		ArrayList<Process> pList = new ArrayList<Process>();
		
		while(index < processCount || pList.size() > 0) {
			while(index < processCount && processList.get(index).arrival <= time) {
				int placement = 0;
				//Find the position to add the process to
				while(placement < pList.size() && processList.get(index).service > pList.get(placement).service) {
					placement++;
				}
				
				/* If the current running process is pre empted
				 * Print the change
				 */
				if(placement == 0 && pList.size() != 0) {
					printChange(pList.get(0), start, time);
					start = time;
				}
				
				//Add the process (copy) in the correct order
				pList.add(placement, Process.copyProcess(processList.get(index)));
				index++;
			}
			
			time++;
			
			//Are there processes to run?
			if(pList.size() != 0) {
				pList.get(0).service--;
				
				if(pList.get(0).service == 0) {
					
					time += pList.get(0).service;
					printChange(pList.get(0), start, time);
					//Remove the process when it is done running
					pList.remove(0);
					
					start = time;
				}
			}
			
			
			
		}//End of all processes
		
	}
	
	/**
	 * Highest Response Ration Next()
	 */
	public static void hrrn() {
		int time = 0;
		int start = 0;
		int index = 0;
		ArrayList<Process> pList = new ArrayList<Process>();
		
		//Add and run all the processes
		while(index < processCount || pList.size() > 0) {
			//Add any processes that have arrived at this time
			while(index < processCount && processList.get(index).arrival <= time) {
				Process temp = Process.copyProcess(processList.get(index));
				temp.hrrn = 1;
				temp.wait = 0;
	
				pList.add(temp);	
				index++;
			}
			
			//printList(pList);
			
			//Are there processes to run?
			if(pList.size() != 0) {
				int service = pList.get(0).service;
				time += service;
				
				//Change the hrrn values
				//Start at the second value as the first will be removed
				for(int i = 1; i < pList.size(); i++) {
					int arrival = pList.get(i).arrival;
					pList.get(i).changeHrrn(time - arrival);
				}
				
				printChange(pList.get(0), start, time);
				
				//Remove the current process
				pList.remove(0);
					
				start = time;
	
			}
			else
				time++;
			
			//Sort the list based on hrrn value
			pList.sort(new CompareHrrn());
			
			
		} //Done running processes
		
		
	}
	
	/**
	 * Print the time range that the process was running
	 * @param data Process Object
	 * @param start Start time
	 * @param end End time
	 */
	public static void printChange(Process data, int start, int end) {
		String print = String.format("%s:%d->%d", data.processName, start, end);
		System.out.println(print);
	}
	
	/**
	 * Print the given array list
	 * @param list The list to print data from
	 */
	public static void printList(ArrayList<Process> list) {
		if(list.size() <= 0)
			return;
		String print = "";
		for(int i = 0; i < list.size(); i++) {
			print = String.format("%s(%.4f) -> ", list.get(i).processName, list.get(i).hrrn);
			System.out.print(print);
		}
		System.out.println();
	}
	
}
