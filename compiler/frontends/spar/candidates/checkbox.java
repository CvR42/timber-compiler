// Tests the presence of at least minimal support for AWT.

import java.awt.*;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;

public class checkbox extends Frame implements ItemListener {
    Checkbox cb = new Checkbox( "Checkbox" );

    checkbox() {
	super( "Checkbox Example" );
	add( cb, BorderLayout.NORTH );
	cb.addItemListener( this );
	pack();
	show();
    }

    public void itemStateChanged( ItemEvent evt ){
	Object obj = evt.getSource();
	if( obj instanceof Checkbox ){
	    System.out.println( "checkbox: " + ((Checkbox) obj).getState());
	}
    }

    static public void main( String[] args ){
	new checkbox();
    }
}
