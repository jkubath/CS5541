package scheduling_JonahKubath;

import java.util.Comparator;

/**
 * Sorts by earliest deadline
 * @author Jonah Kubath
 *
 */
public class CompareDeadline implements Comparator<Process> {
	public int compare(Process o1, Process o2) {
        return (o1.start_deadline - o2.start_deadline);
    }
}
