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

public final class Double extends Number {

  public static final double POSITIVE_INFINITY = __doublebits 0x7ff0000000000000L;
  public static final double NEGATIVE_INFINITY = __doublebits 0xfff0000000000000L;
  public static final double NaN = __doublebits 0x7ff8000000000000L;
  public static final double MIN_VALUE = __doublebits 0x1L;
  public static final double MAX_VALUE = __doublebits 0x7fefffffffffffffL;
  public static final Class TYPE = Class.getPrimitiveClass("double");

  private static final int NUM_MANTISSA_BITS = 52;
  private static final long EXPONENT_MASK = 0x7ff0000000000000L;
  private static final long MANTISSA_MASK = 0x000fffffffffffffL;

  // This table that tells us how many decimal digits are needed to uniquely
  // specify N binary bits, i.e.: bitsToDecimal[N-1] = Ceiling(N ln 2 / ln 10).
  static final int bitsToDecimal[] = new int[] {
     1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,
     6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10,
    10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15,
    15, 16, 16, 16
  };

  /* This is what Sun's JDK1.1 "serialver java.lang.Double" spits out */
  private static final long serialVersionUID = -9172774392245257468L;

  private final double value;

  public static native long doubleToLongBits(double value);
  public static native double longBitsToDouble(long bits);

  public Double(final double value) {
    this.value = value;
  }

  public Double(String s) throws NumberFormatException {
    this.value = valueOf(s).value;
  }

  public double doubleValue() {
    return value;
  }

  public static String toString(final double value) {
    int precision, bitIndex;

    // Handle exceptional values
    if (isNaN(value))
      return "NaN";
    if (value == POSITIVE_INFINITY)
      return "Infinity";
    if (value == NEGATIVE_INFINITY)
      return "-Infinity";

    // Determine number of digits of decimal precision to display
    long bits = doubleToLongBits(value);
    if ((bits & EXPONENT_MASK) == 0) {			// denormalized value
	    for (bitIndex = NUM_MANTISSA_BITS - 1;
		bitIndex > 0 && ((1L << bitIndex) & bits) == 0;
		bitIndex--);
	    precision = bitsToDecimal[bitIndex];
    } else {						// normalized value
	    precision = bitsToDecimal[NUM_MANTISSA_BITS - 1];
    }

    // Add an extra digit to handle rounding
    precision++;

    // Convert value
    return spar.compiler.Double.toStringWithPrecision(value, precision);
  }

  public String toString() {
    return toString(value);
  }

  public boolean equals(Object obj) {
    if ( (obj!=null) && (obj instanceof Double)) {
      Double that=(Double )obj;
      if ((this.isNaN()==true) && (that.isNaN()==true)) return true;
    
      double left = this.value;
      double right = that.value;

      if ((left==+0.0) && (right==-0.0)) return false;
      if ((left==-0.0) && (right==+0.0)) return false;

      return (left==right);
    }
    else {
      return false;
    }
  }
  
  public float floatValue()
  {
    return ((float)value);
  }

  public int hashCode()
  {
    return ((int)value);
  }
  
  public int intValue()
  {
    return ((int)value);
  }
  
  public boolean isInfinite() {
    return ((value == POSITIVE_INFINITY) || (value == NEGATIVE_INFINITY));
  }
  
  public static boolean isInfinite(double v) {
    return ((v == POSITIVE_INFINITY) || (v == NEGATIVE_INFINITY));
  }
  
  public boolean isNaN() {
    return isNaN(value);
  }
  
  public static boolean isNaN(double v) {
    long bits = doubleToLongBits(v);

    return ((bits & EXPONENT_MASK) == EXPONENT_MASK
	&& (bits & MANTISSA_MASK) != 0);
  }
 
  public long longValue() {
    return (long) value;
  }

  public static Double valueOf(String s) throws NumberFormatException {
    return (new Double(spar.compiler.Double.valueOf0(s.value,s.offset,s.count)));
  }

  public static double parseDouble(String s) throws NumberFormatException {
    return spar.compiler.Double.valueOf0(s.value,s.offset,s.count);
  }

}
