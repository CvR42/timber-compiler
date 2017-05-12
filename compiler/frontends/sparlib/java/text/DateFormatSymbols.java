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

public class DateFormatSymbols implements Serializable, Cloneable {

private static final long serialVersionUID = -5987973545549424702L;

String amPmStrings[] = { "AM", "PM" };
String eras[] = { "BC", "AD" };
String months[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December", "" };
String shortMonths[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "" };
String shortWeekdays[] = { "", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
String weekdays[] = { "", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
String zoneStrings[][] = {
    { "PST", "Pacific Standard Time", "PST", "Pacific Daylight Time", "PDT" },
    { "America/Los_Angeles", "Pacific Standard Time", "PST", "Pacific Daylight Time", "PDT" },
    { "MST", "Mountain Standard Time", "MST", "Mountain Daylight Time", "MDT" },
    { "America/Denver", "Mountain Standard Time", "MST", "Mountain Daylight Time", "MDT" },
    { "PNT", "Mountain Standard Time", "MST", "Mountain Standard Time", "MST" },
    { "America/Phoenix", "Mountain Standard Time", "MST", "Mountain Standard Time", "MST" },
    { "CST", "Central Standard Time", "CST", "Central Daylight Time", "CDT" },
    { "America/Chicago", "Central Standard Time", "CST", "Central Daylight Time", "CDT" },
    { "EST", "Eastern Standard Time", "EST", "Eastern Daylight Time", "EDT" },
    { "America/New_York", "Eastern Standard Time", "EST", "Eastern Daylight Time", "EDT" },
    { "IET", "Eastern Standard Time", "EST", "Eastern Standard Time", "EST" },
    { "America/Indianapolis", "Eastern Standard Time", "EST", "Eastern Standard Time", "EST" },
    { "HST", "Hawaii Standard Time", "HST", "Hawaii Standard Time", "HST" },
    { "Pacific/Honolulu", "Hawaii Standard Time", "HST", "Hawaii Standard Time", "HST" },
    { "AST", "Alaska Standard Time", "AKST", "Alaska Daylight Time", "AKDT" },
    { "America/Anchorage", "Alaska Standard Time", "AKST", "Alaska Daylight Time", "AKDT" },
    { "America/Halifax", "Atlantic Standard Time", "AST", "Atlantic Daylight Time", "ADT" },
    { "CNT", "Newfoundland Standard Time", "NST", "Newfoundland Daylight Time", "NDT" },
    { "America/St_Johns", "Newfoundland Standard Time", "NST", "Newfoundland Daylight Time", "NDT" },
    { "ECT", "Central European Standard Time", "CET", "Central European Daylight Time", "CEST" },
    { "Europe/Paris", "Central European Standard Time", "CET", "Central European Daylight Time", "CEST" },
    { "GMT", "Greenwich Mean Time", "GMT", "Greenwich Mean Time", "GMT" },
    { "Africa/Casablanca", "Greenwich Mean Time", "GMT", "Greenwich Mean Time", "GMT" },
    { "Asia/Jerusalem", "Israel Standard Time", "IST", "Israel Daylight Time", "IDT" },
    { "JST", "Japan Standard Time", "JST", "Japan Standard Time", "JST" },
    { "Asia/Tokyo", "Japan Standard Time", "JST", "Japan Standard Time", "JST" },
    { "Europe/Bucharest", "Eastern European Standard Time", "EET", "Eastern European Daylight Time", "EEST" },
    { "CTT", "China Standard Time", "CST", "China Standard Time", "CDT" },
    { "Asia/Shanghai", "China Standard Time", "CST", "China Standard Time", "CDT" }
};
String localPatternChars = "GyMdkHmsSEDFwWahKz";

public DateFormatSymbols() {
	this(Locale.getDefault());
}

public DateFormatSymbols(Locale loc) {
//	ResourceBundle bundle = Format.getResources("dateformat", loc);
//
//	amPmStrings = (String[])bundle.getObject("amPmStrings");
//	eras = (String[])bundle.getObject("eras");
//	localPatternChars = bundle.getString("localPatternChars");
//	months = (String[])bundle.getObject("months");
//	shortMonths = (String[])bundle.getObject("shortMonths");
//	shortWeekdays = (String[])bundle.getObject("shortWeekdays");
//	weekdays = (String[])bundle.getObject("weekdays");
//	zoneStrings = (String[][])bundle.getObject("zoneStrings");
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
		DateFormatSymbols other = (DateFormatSymbols)obj;
		if (!localPatternChars.equals(other.localPatternChars)) {
			return (false);
		}
		for (int i = 0; i < eras.length; i++) {
			if (!eras[i].equals(other.eras[i])) {
				return (false);
			}
		}
		for (int i = 0; i < amPmStrings.length; i++) {
			if (!amPmStrings[i].equals(other.amPmStrings[i])) {
				return (false);
			}
		}
		for (int i = 0; i < months.length; i++) {
			if (!months[i].equals(other.months[i])) {
				return (false);
			}
		}
		for (int i = 0; i < shortMonths.length; i++) {
			if (!shortMonths[i].equals(other.shortMonths[i])) {
				return (false);
			}
		}
		for (int i = 0; i < weekdays.length; i++) {
			if (!weekdays[i].equals(other.weekdays[i])) {
				return (false);
			}
		}
		for (int i = 0; i < shortWeekdays.length; i++) {
			if (!shortWeekdays[i].equals(other.shortWeekdays[i])) {
				return (false);
			}
		}
		for (int i = 0; i < zoneStrings.length; i++) {
			for (int j = 0; j < zoneStrings[j].length; j++) {
				if (!zoneStrings[i][j].equals(other.zoneStrings[i][j])) {
					return (false);
				}
			}
		}
		return (true);
	}
	catch (ClassCastException _) {
		return (false);
	}
}

public String[] getAmPmStrings() {
	return (amPmStrings);
}

public String[] getEras() {
	return (eras);
}

public String getLocalPatternChars() {
	return (localPatternChars);
}

public String[] getMonths() {
	return (months);
}

public String[] getShortMonths() {
	return (shortMonths);
}

public String[] getShortWeekdays() {
	return (shortWeekdays);
}

public String[] getWeekdays() {
	return (weekdays);
}

public String[][] getZoneStrings() {
	return (zoneStrings);
}

public int hashCode() {
	return (super.hashCode());
}

public void setAmPmStrings(String[] data) {
	amPmStrings = data;
}

public void setEras(String[] data) {
	eras = data;
}

public void setLocalPatternChars(String data) {
	localPatternChars = data;
}

public void setMonths(String[] data) {
	months = data;
}

public void setShortMonths(String[] data) {
	shortMonths = data;
}

public void setShortWeekdays(String[] data) {
	shortWeekdays = data;
}

public void setWeekdays(String[] data) {
	weekdays = data;
}

public void setZoneStrings(String[][] data) {
	zoneStrings = data;
}

}
