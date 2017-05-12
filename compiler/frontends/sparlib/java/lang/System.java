/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.util.Properties;

public final class System {
	final static SecurityManager defaultSecurityManager =
		new NullSecurityManager();
        // CvR: these should be 'final', but for the moment we use the
        // old mechanism for stream redirection.
	public static InputStream in;
        public static PrintStream out;
	public static PrintStream err;
	private static Properties props;
	private static SecurityManager security;

// When trying to debug Java code that gets executed early on during
// JVM initialization, eg, before System.err is initialized, debugging
// println() statements don't work. In these cases, the following routines
// are very handy. Simply uncomment the following two lines to enable them.

/****
public static native void debug(String s);	// print s to stderr, then \n
public static native void debugE(Throwable t);	// print stack trace to stderr
****/

static {
	// XXX what are the constraints on the initialization order in here?

	security = defaultSecurityManager;

	props = initProperties(new Properties());

	// Initialise the I/O
	// CvR: don't even try to use the kaffe 'embedded' version of i/0
	in = new BufferedInputStream(new FileInputStream(FileDescriptor.in), 128);
	out = new PrintStream(new BufferedOutputStream(new FileOutputStream(FileDescriptor.out), 128), true);
	err = new PrintStream(new BufferedOutputStream(new FileOutputStream(FileDescriptor.err), 128), true);	

        // Initialize the system class loader
	// CvR: deleted, since we don't do class loading.
}

private System() { }

native public static void arraycopy(Object src,
	int src_position, Object dst, int dst_position, int length);

// CvR: provide a number of specialized implementations of arraycopy.

final public static void arraycopy( byte src[], int srcOffset, byte dst[], int dstOffset, int count ){
    for( int i=0; i<count; i++ ){
	dst[dstOffset+i] = src[srcOffset+i];
    }
}

final public static void arraycopy( char src[], int srcOffset, char dst[], int dstOffset, int count ){
    for( int i=0; i<count; i++ ){
	dst[dstOffset+i] = src[srcOffset+i];
    }
}

final public static void arraycopy( int src[], int srcOffset, int dst[], int dstOffset, int count ){
    for( int i=0; i<count; i++ ){
	dst[dstOffset+i] = src[srcOffset+i];
    }
}

final public static void arraycopy( float src[], int srcOffset, float dst[], int dstOffset, int count ){
    for( int i=0; i<count; i++ ){
	dst[dstOffset+i] = src[srcOffset+i];
    }
}

final public static void arraycopy( double src[], int srcOffset, double dst[], int dstOffset, int count ){
    for( int i=0; i<count; i++ ){
	dst[dstOffset+i] = src[srcOffset+i];
    }
}

final public static void arraycopy( boolean src[], int srcOffset, boolean dst[], int dstOffset, int count ){
    for( int i=0; i<count; i++ ){
	dst[dstOffset+i] = src[srcOffset+i];
    }
}

final public static void arraycopy( Object src[], int srcOffset, Object dst[], int dstOffset, int count ){
    for( int i=0; i<count; i++ ){
	dst[dstOffset+i] = src[srcOffset+i];
    }
}

private static void checkPropertyAccess() {
	getSecurityManager().checkPropertiesAccess();
}

native public static long currentTimeMillis();

public static void exit (int status) {
	Runtime.getRuntime().exit(status);
}

public static void gc() {
	Runtime.getRuntime().gc();
}

public static Properties getProperties() {
	checkPropertyAccess();

	return props;
}

public static String getProperty(String key) {
	checkPropertyAccess();

	return props.getProperty(key);
}

public static String getProperty(String key, String def) {
	checkPropertyAccess();

	return props.getProperty(key, def);
}

public static SecurityManager getSecurityManager() {
	return security;
}

public static String getenv(String name) {
	throw new Error("System.getenv no longer supported");
}

native public static int identityHashCode(Object x);

//native private static Properties initProperties(Properties props);

public static void load(String filename) {
	Runtime.getRuntime().load(filename,
	    Class.getStackClass(1).getClassLoader());
}

public static void loadLibrary(String libname) {
	// CvR: disabled, since we don't handle class loading.
	//Runtime.getRuntime().loadLibrary(libname, Class.getStackClass(1).getClassLoader());
}

public static String mapLibraryName(String fn) {
	return NativeLibrary.getLibPrefix() + fn + NativeLibrary.getLibSuffix();
}

public static void runFinalization() {
	Runtime.getRuntime().runFinalization();
}

public static void runFinalizersOnExit(boolean value) {
	Runtime.getRuntime().runFinalizersOnExit(value);
}

public static void setErr(PrintStream err) {
	System.err = err;
}

public static void setIn(InputStream in) {
	System.in = in;
}

public static void setOut(PrintStream out) {
	System.out = out;
}

public static void setProperties(Properties prps) {
	checkPropertyAccess();

	props = prps;
}

public static void setSecurityManager(SecurityManager s) {
	if (security != defaultSecurityManager) {
		throw new SecurityException();
	}
	if (s != null) {
		security = s;
	}
}

//native private static Properties initProperties(Properties props);

// CvR: For the moment we replace this with our own, fixed, properties.
private static Properties initProperties(Properties props)
{
    props.put( "line.separator", "\n" );
    props.put( "file.separator", "/" );
    props.put( "path.separator", ":" );
    props.put( "java.vendor", "Delft University of Technology" );
    props.put( "user.timezone", "CET" );
    return props;
}

}
