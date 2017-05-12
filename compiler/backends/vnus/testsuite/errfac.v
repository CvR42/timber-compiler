// fac.v -- The Factorial Function.

program

declarations [
    formalvariable n Fac long,
    formalvariable a FacRec long,

    globalvariable x long,
    externalvariable vnusin int,
    externalvariable vnusout int,
    externalfunction v__open [v__open_fnm,v__open_mode] int,
    formalvariable v__open_fnm v__open string,
    formalvariable v__open_mode v__open string,
    externalprocedure v__close [v__close_hnd],
    formalvariable v__close_hnd v__close int,
    externalprocedure v__writeLong [v__writeLong_hnd,v__writeLong_n],
    formalvariable v__writeLong_n v__writeLong long,
    formalvariable v__writeLong_hnd v__writeLong int,
    externalprocedure v__readLong [v__readLong_hnd,v__readLong_n],
    formalvariable v__readLong_n v__readLong pointer long,
    formalvariable v__readLong_hnd v__readLong int,
    externalprocedure v__writeString [v__writeString_hnd,v__writeString_s],
    formalvariable v__writeString_hnd v__writeString int,
    formalvariable v__writeString_s v__writeString string,
    localvariable m Fac long,
    // Iterative factorial function.
    function Fac [n] long statements Fac [
     	assign m 1l,
     	while (n, >, 1l) statements [
             assign m (m, *, n),
	     assign n (n, -, 1l)
        ],
	return m
    ],

	// Recursive factorial function.
    function FacRec [a] long statements FacRec [
	if (a, =, 0l)  statements [
            return 1
	]
	statements [ // else
	    return (a, *, functioncall FacRec [(a,-,1l)])
        ]
    ]
]


statements [

    procedurecall v__writeString 
            [ vnusout, "Welcome to the Vnus Factorial Calculator\n\n"],

    procedurecall v__writeString 
            [ vnusout, "Please give me a positive integer (1000 to stop): "],

    procedurecall v__readLong
            [ vnusin, &x ],

    while (x, <>, 1000l) statements [

        procedurecall v__writeString 
                [ vnusout, "\nIterative:\t"],
   
        procedurecall v__writeLong
	        [ vnusout, x ],

        procedurecall v__writeString 
	        [ vnusout, "! = "],

        procedurecall v__writeLong
	        [ vnusout, functioncall Fac [x] ],

        procedurecall v__writeString 
                [ vnusout, "\nRecursive:\t"],

        procedurecall v__writeLong
	        [ vnusout, x ],

        procedurecall v__writeString 
	        [ vnusout, "! = "],

        procedurecall v__writeLong
	        [ vnusout, functioncall FacRec [x] ],

        procedurecall v__writeString 
                [ vnusout, "\n\n"],

        procedurecall v__writeString 
                [ vnusout, "Please give me a positive integer (1000 to stop): "],

        procedurecall v__readLong
                [ vnusin, &x ]
    ],

    procedurecall v__writeString 
            [ vnusout, "\nThank you for using the Vnus Factorial Calculator.\nHave a nice day!\n\n"]
   
]
