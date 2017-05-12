/*

This program was submitted to the Java bugparade:
http://developer.java.sun.com/developer/bugParade/bugs/4201432.html

The following program exhibits garbage collection degeneracy
under JDK 1.1.6, 1.1.7B, and 1.2 (2.0), as well as jview.
Specifically, the heuristic used to decide when to run
compact is broken; each GC returns a decreasing fraction of
the free memory, so the program spends all its time in GC.

This is a distillation of the same problem exhibited by a
much larger real application whose performance is not
acceptable strictly because of the GC behavior.

*/

/*
 *   Tests heap operations (garbage collection) in Java.  Shows bad cases.
 *
 *   This program shows the Java jdk1.1.7B garbage collector failing to
 *   operate efficiently because it does not compact frequently enough.
 *   Specifically, this short program causes the heuristic used to decide
 *   when to compact to *fail*.
 *
 *   Run this program with the arguments
 *
 *   java -ms8m -mx100m heap
 *
 *   (for example).  Notice how the operations take longer and longer.
 *   Then all of a sudden they are quick again, and then they take longer
 *   and longer again.
 *
 *   To understand why, add -verbosegc to the options, as in
 *
 *   java -ms8m -mx100m -verbosegc heap
 *
 *   (If your machine has <128M, then adjust the -mx line accordingly to
 *   be somewhere around 30M less than the amount of memory your machine
 *   has; if you don't have at least 80M of RAM, then don't run this
 *   program.)
 *
 *   Pay attention to the compact lines.  Note that there is typically a
 *   long sequence of gc's that don't compact, but which free objects
 *   totalling a decreasing fraction of the total free space (for instance
 *   when the heap is 12M and the freespace is xxxxx often a GC will recover
 *   only 100K---about 3% of the total free space).  If the GC would
 *   compact, the free space recovered would jump to nearly the entire
 *   freespace.
 *
 *   The program is simple.  First, it does 100K typically small allocs.
 *   Then in a loop, it allocates a new `thing' 50.4% of the time, and
 *   frees a randomly selected previously allocated thing 49.6% of the
 *   time.  For the allocations, 1% of the time they are 4096 bytes, and
 *   99% of the time they are 8 bytes.
 *
 *   This behavior is typical of a real-world application we have that is
 *   being *killed* by the current GC behavior.  Currently I have no
 *   workaround or solution.  Essentially, the GC algorithm must either be
 *   fixed to compact more frequently (for instance, if the objects recovered
 *   total in space < 50% of the free space that is there, a compact should
 *   be *forced*) or else access to tuning variables of the GC must be
 *   granted so we can eliminate this behavior.
 *
 *   Tom Rokicki
 *   xxxxx@xxxxx
 *   6 January 1999
 */
import java.lang.Math;
class heap {
    public static void main(String args[]) {
        double mallocfreq = 0.504;
        int small = 8, big = 4096;
        int initialallocs = 100000;
        double bigfreq = 0.010;
        Object[] allocs = new Object[1000];
        int operations = 0;
        int population = 0;
        int i;
        long lasttime = System.currentTimeMillis();
        long minInterval = Long.MAX_VALUE;
        long maxInterval = Long.MIN_VALUE;
        if (args.length == 2) {
            small = Integer.parseInt(args[0]);
            big = Integer.parseInt(args[1]);
        }
        while (true) {
            operations++;
            double r = Math.random();
            if (operations > initialallocs && r >= mallocfreq) {        // free
something
                if (population > 0) {
                    r = Math.floor(population * (r - mallocfreq) / (1 -
mallocfreq))
                        ;
                    population--;
                    allocs[(int) r] = allocs[population];
                    allocs[population] = null;
                }
            } else {            // alloc something
                if (population >= allocs.length) {
                    Object[] nallocs = new Object[population * 2 + 3];
                    for (i = 0; i < population; i++)
                        nallocs[i] = allocs[i];
                    allocs = nallocs;
                }
                allocs[population++] =
                    new byte[(r / mallocfreq >= bigfreq) ? small : big];
            }
            if (operations % 100000 == 0) {
                long thistime = System.currentTimeMillis();
                long thisInterval = thistime - lasttime;
                minInterval = Math.min(minInterval, thisInterval);
                maxInterval = Math.max(maxInterval, thisInterval);
                System.out.println("After " + operations + " ops, " +
population +
                                   " live allocs, interval " + thisInterval +
                                   "\tmin " + minInterval + " max " +
maxInterval);
                lasttime = thistime;
            }
        }
    }        
}
