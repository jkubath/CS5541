package scheduling_JonahKubath;

public class Process {
	String processName;
	String processID;
	int arrival;
	int service;
	int start;
	int end;
	boolean run = false;
	double hrrn;
	int wait;
	int start_deadline;
	int end_deadline;
	
	/**
	 * Initialize to null
	 */
	public Process() {
		processName = null;
		processID = null;
		arrival = -1;
		service = -1;
		start = -1;
		end = -1;
		hrrn = -1;
		wait = -1;
		start_deadline = -1;
		end_deadline = -1;
	}
	
	/**
	 * Initialize data
	 * @param name The name of the process
	 * @param iArrival The arrival time
	 * @param iService The service time
	 */
	public Process(String name, int iArrival, int iService) {
		processName = name;
		processID = name;
		arrival = iArrival;
		service = iService;
		start = -1;
		end = -1;
		hrrn = -1;
		wait = -1;
		start_deadline = -1;
		end_deadline = -1;
	}
	
	/**
	 * Return data of the process as a string
	 */
	public String toString() {
		String returnString = String.format("%s\t%d - %d", processName, arrival, service);
		if(start_deadline != -1)
			returnString += ", " + start_deadline;
		
		return returnString;
	}
	
	/**
	 * Update the HRRN ratio
	 * @param time The amount of time the process has been waiting
	 */
	public void changeHrrn(int time) {
		//wait = time;
		hrrn = (time + service) / service;
	}
	
	/**
	 * Copies the original process object to a new
	 * one and returns the new process object
	 * @param orig The original process that will be copied from
	 * @return The new process that was created
	 */
	public static Process copyProcess(Process orig) {
		Process temp = new Process(orig.processName, orig.arrival, orig.service);
		temp.end_deadline = orig.end_deadline;
		temp.start_deadline = orig.start_deadline;
		temp.processID = orig.processID;
		return temp;
	}
	
}
