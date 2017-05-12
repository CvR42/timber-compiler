/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.text;

import java.lang.String;
import java.io.Serializable;
import java.util.ResourceBundle;
import java.util.Locale;

public class DecimalFormatSymbols implements Serializable, Cloneable {

private static final long serialVersionUID = 5772796243397350300L;
char digit;
char patternSeparator;
char zeroDigit;
char groupSeparator;
char decimalSeparator;
char percentSign;
char permillSign;
char minusSign;
String infinity;
String nan;
char currencySign;

public DecimalFormatSymbols() {
	this(Locale.getDefault());
}

public DecimalFormatSymbols(Locale loc) {
	// CvR: for the moment this is hard-coded.
	digit = '#';
	patternSeparator = ';';
	zeroDigit = '0';
	groupSeparator = ',';
	decimalSeparator = '.';
	percentSign = '%';
	permillSign = '\u2030';
	minusSign = '-';
	infinity = "\u221e";
	nan = "\ufffd";
	currencySign = '\u00a4';
}

public Object clone() {
	try {
		return (super.clone());
	}
	catch (CloneNotSupportedException _) {
		return (null);
	}
}

public boolean equals(Object obj) {
	try {
		DecimalFormatSymbols other = (DecimalFormatSymbols)obj;
		if (digit == other.digit &&
		    patternSeparator == other.patternSeparator &&
		    zeroDigit == other.zeroDigit &&
		    groupSeparator == other.groupSeparator &&
		    decimalSeparator == other.decimalSeparator &&
		    percentSign == other.percentSign &&
		    permillSign == other.permillSign &&
		    minusSign == other.minusSign &&
		    infinity.equals(other.infinity) &&
		    nan.equals(other.nan)) {
			return (true);
		}
	}
	catch (ClassCastException _) {
	}
	return (false);
}

public char getDecimalSeparator() {
	return (decimalSeparator);
}

public char getDigit() {
	return (digit);
}

public char getGroupingSeparator() {
	return (groupSeparator);
}

public String getInfinity() {
	return (infinity);
}

public char getMinusSign() {
	return (minusSign);
}

public String getNaN() {
	return (nan);
}

public char getPatternSeparator() {
	return (patternSeparator);
}

public char getPercent() {
	return (percentSign);
}

public char getPerMill() {
	return (permillSign);
}

public char getZeroDigit() {
	return (zeroDigit);
}

public int hashCode() {
	return (super.hashCode());
}

public void setDecimalSeparator(char val) {
	decimalSeparator = val;
}

public void setDigit(char val) {
	digit = val;
}

public void setGroupingSeparator(char val) {
	groupSeparator = val;
}

public void setInfinity(String val) {
	infinity = val;
}

public void setMinusSign(char val) {
	minusSign = val;
}

public void setNaN(String val) {
	nan = val;
}

public void setPatternSeparator(char val) {
	patternSeparator = val;
}

public void setPercent(char val) {
	percentSign = val;
}

public void setPerMill(char val) {
	permillSign = val;
}

public void setZeroDigit(char val) {
	zeroDigit = val;
}

}
