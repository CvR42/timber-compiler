// Tests of the constant folder.

program
declarations [
    externalvariable vnusout int,
    externalvariable a boolean,
    externalvariable b boolean,
    externalvariable c boolean,
    externalvariable i int,
    externalvariable j int,
    externalvariable k int
]
statements [
    print [vnusout, if (3,<,4) "yes" "no"],
    print [vnusout, (true,shortand,(false,shortand,true))],
    print [vnusout, (true,shortor,(false,shortor,true))],
    print [vnusout, (a,shortand,(b,shortand,c))],
    print [vnusout, (a,shortand,(b,shortand,a))],
    print [vnusout, (a,shortor,(b,shortor,c))],
    print [vnusout, (a,shortor,(b,shortor,a))],
    print [vnusout, reduction shortand [a,b,c]],
    print [vnusout, reduction shortand [a,b,a]],
    print [vnusout, reduction shortand [a,b,b]],
    print [vnusout, reduction shortor [a,b,c]],
    print [vnusout, reduction shortor [a,b,a]],
    print [vnusout, reduction shortor [a,b,b]],
    print [vnusout, reduction + [i,j,k]],
    print [vnusout, reduction + [i,j,(~,i)]],
    print [vnusout, reduction + [i,(~,i)]],
    print [vnusout, reduction + [i,1,-3]],
    print [vnusout, reduction + [i,0]],
    print [vnusout, reduction + [0]],
    print [vnusout, ((a,shortand,b),shortand,c)],
    print [vnusout, ((a,shortand,b),shortand,a)],
    print [vnusout, ((a,shortor,b),shortor,c)],
    print [vnusout, ((a,shortor,b),shortor,a)],
    print [vnusout, reduction + [i,(j,-,k)]],
    print [vnusout, reduction + [(j,-,k),i]],
    print [vnusout, (1,<<,7)],
    print [vnusout, "(i,+,0)", (i,+,0)],
    print [vnusout, "(0,+,i)", (0,+,i)],
    print [vnusout, "(i,-,0)", (i,-,0)],
    print [vnusout, "(i,or,0)", (i,or,0)],
    print [vnusout, "(0,or,i)", (0,or,i)],
    print [vnusout, "(i,*,1)", (i,*,1)],
    print [vnusout, "(1,*,i)", (1,*,i)],
    print [vnusout, "(i,/,1)", (i,/,1)]
]
