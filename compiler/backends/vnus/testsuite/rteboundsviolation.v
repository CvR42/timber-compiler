program
declarations [
    externalvariable vnusout int,
    globalvariable a pointer shape [dontcare] int nullednew shape [3] int null,
    procedure VnusEventArrayBoundViolated []
    statements [
	println [vnusout,"Bounds violation"]
    ]
]
statements [
    assign (*a,[5]) 3
]
