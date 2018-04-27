package scheduling_JonahKubath;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;

/**
 * 
 * @author Jonah Kubath
 * Summary: Simulation for different processor scheduling policies.
 * Uniprocessor, Real-Time Aperiodic, and Real-Time Periodic
 * Class: CS5541
 * Date: 4/19/2018
 *
 */
public class Main {
	
	//List of the processes read from the file
	public static ArrayList<Process> processList = new ArrayList<Process>();

	public static void main(String[] args) {
		//Ask the user which file to use as input
		int fileChoice = getFileChoice();
		if(fileChoice == -1) 
		{
			System.out.println("Program closed");
			return; //User exited
		}
		//Read the data from the input file
		String[] policy = readFile(fileChoice);
		if(policy == null) {
			System.out.println("ERROR: failed to read policy data");
			return;
		}
		int processCount = 0;
		
		/* Choose which policy to use based on the header of the file
		 * U: Uniprocessor Policies
		 * RA: Real-Time Aperiodic Policies
		 * RP: Real-Time Periodic Policies
		 */
		switch(policy[0]) {
			case "U":
				int quantum;
				/* Make sure that processCount and quantum are integer values */
				try {
					processCount = Integer.parseInt(policy[1]);
					quantum = Integer.parseInt(policy[2]);
				}
				catch(NumberFormatException e) {
					System.out.println("ERROR: Process Count and Quantum must be integers");
					System.out.println("Check the first line in the input file");
					return;
				}
				//Set class data for Uniprocessor
				Uniprocessor.setData(processCount, quantum);
				
				//First come first serve
				System.out.println("FCFS:");
				Uniprocessor.fcfs();
				
				//Round robin
				System.out.println("RR:");
				Uniprocessor.rr();
				
				//Shortest Process Next
				System.out.println("SPN:");
				Uniprocessor.spn();
				
				//Shortest Remaining Time
				System.out.println("SRT:");
				Uniprocessor.srt();
				
				//Highest Response Ratio Next
				System.out.println("HRRN:");
				Uniprocessor.hrrn();
				break;
			case "RA":
				/* Make sure that processCount and quantum are integer values */
				try {
					processCount = Integer.parseInt(policy[1]);
				}
				catch(NumberFormatException e) {
					System.out.println("ERROR: Process Count must be an integer");
					System.out.println("Check the first line in the input file");
					return;
				}
				
				//Set class data for Real_Time_Aperiodic
				Real_Time_Aperiodic.setData(processCount);
				
				//Earliest Deadline
				System.out.println("ED:");
				Real_Time_Aperiodic.ed();
				
				//Earliest Deadline with Unforced Idle Times
				System.out.println("EDUI:");
				Real_Time_Aperiodic.edui();
				
				//First Come First Served
				System.out.println("FCFS:");
				Real_Time_Aperiodic.fcfs();
				break;
			case "RP":
				int totalTime = 0;
				/* Make sure that processCount and totalTime are integer values */
				try {
					processCount = Integer.parseInt(policy[1]);
					totalTime = Integer.parseInt(policy[2]);
				}
				catch(NumberFormatException e) {
					System.out.println("ERROR: Process Count and Quantum must be integers");
					System.out.println("Check the first line in the input file");
					return;
				}
				
				//Set class data for Real_Time_Periodic
				Real_Time_Periodic.setData(processCount, totalTime);
				
				//Fixed-Priority
				System.out.println("FP:");
				Real_Time_Periodic.fp();
				
				//Earliest Deadline using Completion Deadlines
				System.out.println("EDCD:");
				Real_Time_Periodic.edcd();
				
		} //End of switch statements for scheduling policies
		
		Scan.close();
	} //End of main
	
	
	/**
	 * Read the correct data file
	 * @param fileNumber The choice of the user for the input file
	 * 0: process.trace
	 * 1: process_aeperiodic.trace
	 * 2: process_periodic.trace
	 * @return policy[] - the data of the first line of the file
	 */
	public static String[] readFile(int fileNumber) {
		String[] policy = null; //Header information
		String[] data; //Hold the data for each line of the input file
		String fileName = ""; //File to read the data from
		
		//Set the fileName
		if(fileNumber == 0) {
			fileName = "process.trace";
			data = new String[3];
		}
		else if(fileNumber == 1) {
			fileName = "process_aperiodic.trace";
			data = new String[4];
		}
		else
		{
			fileName = "process_periodic.trace";
			data = new String[4];
		}
		
		//Open the corresponding file
		try {
			File file = new File(fileName);
			Scan.scan = new Scanner(file);
			boolean first = true; //The first line of each file is poliy information
			
			while(Scan.scan.hasNext()) {
				Process temp = new Process();
				
				//Save the first line as the process policy
				if(first) {
					policy = Scan.scan.nextLine().split(",");
					first = false;
				}
				//Then read the actual process data
				else {
					//Split the data
					data = Scan.scan.nextLine().split(",");
					temp.processName = data[0];
					temp.processID = data[0];
					temp.arrival = Integer.parseInt(data[1]);
					temp.service = Integer.parseInt(data[2]);
					
					//For aperiodic processes parse the start deadline
					if(fileNumber == 1)
						temp.start_deadline = Integer.parseInt(data[3]);
					else if(fileNumber == 2)
						temp.end_deadline = Integer.parseInt(data[3]);

			
					//Add the Process info to the ArrayList
					processList.add(temp);
				}
				
			}
			
			
			
		}
		catch(FileNotFoundException e) {
			System.out.println("File not found");
			System.out.println("Input: " + fileName);
		}
		
		return policy;
	}
	
	/**
	 * Get the input file choice from the user
	 * @return The integer value of which file to read from
	 */
	public static int getFileChoice() {
		String input = "";
		int fileChoice = -2;
		boolean goodInput = false;
		
		
		while(!goodInput) {
			System.out.println("Enter the number of the file to run");
			System.out.println("0: process.trace - Uniprocessor Policies");
			System.out.println("1: process_aperiodic - Real-Time Aperiodic Policies");
			System.out.println("2: process_periodic - Real-Time Periodic Policies");
			System.out.println("-1 to exit");
			
			try {
				input = Scan.scan.nextLine();
				//Catch bad input - non integer values
				fileChoice = Integer.parseInt(input);
				
				if(fileChoice == -1) {
					return -1;
				}
				
				//Catch out of range input
				if(fileChoice < 0 || fileChoice > 2) {
					throw new ArithmeticException();
				}
				
				//Break from the loop
				goodInput = true;
			}
			catch(ArithmeticException e) {
				System.out.println("Choices range from 0 - 2");
			}
			catch(NumberFormatException e) {
				System.out.println("An integer must be entered");
			}
		}
		
		
		Scan.scan.close();
		return fileChoice;
	}
	
	/**
	 * print the information held in the processList array list
	 */
	public static void printProcessList() {
		for(int i = 0; i < processList.size(); i++) {
			System.out.println(processList.get(i).toString());
		}
	}

}
