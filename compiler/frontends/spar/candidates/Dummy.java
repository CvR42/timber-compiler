

import java.lang.ref.WeakReference;

public class Dummy {

    String name_;
    byte[] volume_ = new byte[100000];

    public Dummy(String name) {
        name_ = name;
    }

    protected void finalize() {
        System.out.println(toString() + ".finalize()");
    }

    public String toString() {
        return "Dummy('" + name_ + "')";
    }

    //------------------------------------------------
    public static void main(String[] args) {
        Runtime runtime = Runtime.getRuntime();

        // Create a 'big' object and keep a weak
        // reference to this object
        Dummy dummy = new Dummy("Weakly ref'ed");
        WeakReference weakRef = new WeakReference(dummy);
        dummy = null;
        
        // Some objects will be strongly referenced 
        // by this array.
        Object[] tabs = new Object[5000000];

        // Loop until 'OutOfMemoryError' exception
        int cnt = 0;
        while(true) {
            for (int i=0; i < 10; i++) {
                // Create a 'big' object but keep no reference
                // to this object (it can be reclaimed...)
                Dummy tmp = new Dummy("Unref'ed #" + cnt);

                // Create another 'big' object and keep a reference
                // to this object (it cannot be reclaimed!)
                tabs[cnt] = new Dummy("Strongly Ref'ed #" + cnt);
                cnt++;
            }
                
            // Once in a while: display memory status
            System.out.println("Memory consumption: "
                               + (int)(100 * (float)runtime.freeMemory() 
                                       / runtime.totalMemory())
                               + "%");

            // See if our weak reference still references an object (should
            // either display the object, or 'null').
            System.out.println("Weak ref'ed: " + (Dummy)weakRef.get());
        }
    }
}
