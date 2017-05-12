/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

// CvR: this class is hacked because we don't support library loading.

package java.lang;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.StringTokenizer;

public class Runtime
{
/**
 *  XXX This is not part of the public interface.
 */
public static interface MemoryAdvice {

	// These should match those in kaffe.lang.MemoryAdvice
	public static final int GREEN = 0;
	public static final int YELLOW = 1;
	public static final int ORANGE = 2;
	public static final int RED = 3;
}

private static Runtime currentRuntime = new Runtime();
private static kaffe.lang.MemoryAdvice advice
	= kaffe.lang.MemoryAdvice.getInstance();

private Runtime () {
}

public Process exec(String command) throws IOException {
	return exec(command, null, null);
}

public Process exec(String command, String envp[]) throws IOException {
	return exec(command, envp, null);
}

public Process exec(String command, String envp[], File dir)
		throws IOException {
	StringTokenizer tokenizer=new StringTokenizer(command);

	int count=tokenizer.countTokens();
	String cmdarray[]=new String[count];

	for (int pos=0; pos<count; pos++) {
		cmdarray[pos]=tokenizer.nextToken();
	}

	return exec(cmdarray, envp, dir);
}

public Process exec(String cmdarray[]) throws IOException {
	return exec(cmdarray, null, null);
}

public Process exec(String cmdarray[], String envp[]) throws IOException {
	return exec(cmdarray, envp, null);
}

public Process exec(String[] cmdarray, String[] envp, File dir)
		throws IOException {
	System.getSecurityManager().checkExec(cmdarray[0]);
	return execInternal(cmdarray, envp, dir);
}

private native Process execInternal(String cmdary[], String envp[], File dir)
	throws IOException;

public void exit(int status) {
	System.getSecurityManager().checkExit(status);
	exitInternal(status);
}

public void halt(int status) {
	exitInternal(status);
}

native private void exitInternal(int status);

public long freeMemory() { return spar.compiler.GC.freeMemory(); }

public void gc() { spar.compiler.GC.collect(); }

/**
 * @deprecated
 */
public InputStream getLocalizedInputStream(InputStream in) {
	return (in);
}

/**
 * @deprecated
 */
public OutputStream getLocalizedOutputStream(OutputStream out) {
	return (out);
}

public static Runtime getRuntime() {
	return currentRuntime;		
}

public void loadLibrary(String libname) {
	loadLibrary(libname, Class.getStackClass(1).getClassLoader());
}

void loadLibrary(String libname, ClassLoader loader) {
	// CvR: severely hacked, since we don't support class loading.
	String errmsg = libname + ": not found";
	throw new UnsatisfiedLinkError(errmsg);
}

public void load(String filename) {
	load(filename, Class.getStackClass(1).getClassLoader());
}

void load(String filename, ClassLoader loader) {
	System.getSecurityManager().checkLink(filename);
	new NativeLibrary(filename, loader);
}

int getMemoryAdvice() {
	return (advice.getColor());
}

int waitForMemoryAdvice(int level) throws InterruptedException {
	return (advice.waitForOtherColor(level));
}

public void addShutdownHook(Thread hook) {
	// XXX implement me
	throw new kaffe.util.NotImplemented(
	    getClass().getName() + ".addShutdownHook()");
}

public boolean removeShutdownHook(Thread hook) {
	// XXX implement me
	return false;
}

native public void runFinalization();

native public static void runFinalizersOnExit(boolean value);

public long totalMemory() { return spar.compiler.GC.totalMemory(); }

native public void traceInstructions(boolean on);

native public void traceMethodCalls(boolean on);
}
