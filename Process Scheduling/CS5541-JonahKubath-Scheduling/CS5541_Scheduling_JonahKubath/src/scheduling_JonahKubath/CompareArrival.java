package scheduling_JonahKubath;

import java.util.Comparator;

/**
 * Sorts from early arrival to late arrival
 * @author Jonah Kubath
 *
 */
class CompareArrival implements Comparator<Process> {
    public int compare(Process o1, Process o2) {
        return (o1.arrival - o2.arrival);
    }
}
