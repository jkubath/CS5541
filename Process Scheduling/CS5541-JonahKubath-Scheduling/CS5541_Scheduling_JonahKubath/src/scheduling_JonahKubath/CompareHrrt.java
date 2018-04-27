package scheduling_JonahKubath;

import java.util.Comparator;

/**
 * Sorts from lower HRRN to high HRRN
 * @author Jonah Kubath
 *
 */
class CompareHrrn implements Comparator<Process> {
    public int compare(Process o1, Process o2) {
    	//Sort higher hrrn to lower hrrn
        return (int) (o2.hrrn - o1.hrrn);
    }
}
