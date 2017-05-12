// File: DataParallel.java

package spar.lang;

public final class DataParallel {
    private DataParallel() { /* Prevent instantiation of this class. */ }
    public static native int getNumberOfProcessors();
}
