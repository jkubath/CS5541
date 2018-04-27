package scheduling_JonahKubath;

import java.util.ArrayList;

public class Real_Time_Aperiodic {
	static ArrayList<Process> processList = Main.processList;
	static int processCount;
	
	public static void setData(int count) {
		processCount = count;
	}
	
	public static void ed() {
		int time = 0;
		int start = 0;
		int index = 0;
		ArrayList<Process> pList = new ArrayList<Process>();
		
		//Add and run all the processes
		while(index < processCount || pList.size() > 0) {
			//Add any processes that have arrived at this time
			while(index < processCount && processList.get(index).arrival <= time) {
				int placement = 0;
				//Find the position to add the process to
				while(placement < pList.size() && processList.get(index).start_deadline >= pList.get(placement).start_deadline) {
					placement++;
				}
				
				pList.add(placement, Process.copyProcess(processList.get(index)));
				
				index++;
			}
			
			
			if(pList.size() == 0)
			{
				time++;
				start = time;
				continue;
			}
			
			
			//Are there processes to run?
			if(pList.size() != 0) {
				int pastDeadline = 0;
				while(pastDeadline < pList.size() && pList.get(pastDeadline).start_deadline < time) {
					System.out.println(pList.get(pastDeadline).processName + ":Missed");
					pList.remove(pastDeadline);
					pastDeadline++;
				}
				
				if(pList.size() != 0) {
					time += pList.get(0).service;
					
					printChange(pList.get(0), start, time);
					
					//Remove the current process
					pList.remove(0);
						
					start = time;
				}
	
			}
			
			//Sort the list based on start_deadline value
			pList.sort(new CompareDeadline());
			
			
		} //Done running processes
		
	}
	
	public static void edui() {
		int time = 0;
		int start = 0;
		int index = 0;
		ArrayList<Process> pList = new ArrayList<Process>();
		
		//Add and run all the processes
		while(index < processCount || pList.size() > 0) {
			//Add any processes that have arrived at this time
			while(index < processCount && processList.get(index).arrival <= time) {
				int placement = 0;
				//Find the position to add the process to
				while(placement < pList.size() && processList.get(index).start_deadline >= pList.get(placement).start_deadline) {
					placement++;
				}
				
				pList.add(placement, Process.copyProcess(processList.get(index)));
				
				index++;
			}
			
			
			if(pList.size() == 0)
			{
				time++;
				start = time;
				continue;
			}
			/* Can the next process run before the current process
			 * hits its start_deadline?  Then wait.
			 */
			else if(index < processList.size() && 
					(processList.get(index).service + time) < pList.get(0).start_deadline){
				time++;
				start = time;
				continue;
			}
			
			
			//Are there processes to run?
			if(pList.size() != 0) {
				int pastDeadline = 0;
				while(pastDeadline < pList.size() && pList.get(pastDeadline).start_deadline < time) {
					System.out.println(pList.get(pastDeadline).processName + ":Missed");
					pList.remove(pastDeadline);
					pastDeadline++;
				}
				
				if(pList.size() != 0) {
					time += pList.get(0).service;
					
					printChange(pList.get(0), start, time);
					
					//Remove the current process
					pList.remove(0);
						
					start = time;
				}
	
			}
			
			//Sort the list based on start_deadline value
			pList.sort(new CompareDeadline());
			
			
		} //Done running processes
		
	}
	
	public static void fcfs() {
		int start = 0;
		int time = 0;
		
		/* Run the processes in order of arrival */
		for(int i = 0; i < processCount; i++) {
			//Wait for the next process to arrive if needed
			if(processList.get(i).start_deadline > time) {
				if(processList.get(i).arrival > time)
					time = processList.get(i).arrival;
				start = time;
			}
			else {
				System.out.println(processList.get(i).processName + ":Missed");
				continue;
			}
			//Run the first process to created
			time += processList.get(i).service;
			//Print the change
			printChange(processList.get(i), start, time);
			//start = time;
		}
	}
	
	public static void printChange(Process data, int start, int end) {
		String print = String.format("%s:%d->%d", data.processName, start, end);
		System.out.println(print);
	}
	
	public static void printList(ArrayList<Process> list) {
		if(list.size() <= 0)
			return;
		String print = "";
		for(int i = 0; i < list.size(); i++) {
			print = String.format("%s(%d) -> ", list.get(i).processName, list.get(i).start_deadline);
			System.out.print(print);
		}
		System.out.println();
	}
	
	
	
	
}
