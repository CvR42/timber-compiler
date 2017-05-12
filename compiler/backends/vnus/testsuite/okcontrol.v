program
declarations [
    externalvariable vnusout int,
    globalvariable v int
]
statements [
    if (3,<,4) statements [
        println [vnusout,"yes"]
    ]
    statements [
        println [vnusout,"no"]
    ],
    assign v 3,
    while (v,>,0) statements [
        println [vnusout,"v=",v],
        assign v (v,-,1)
    ],
    dowhile (v,>,0) statements [
        println [vnusout,"v=",v],
        assign v (v,-,1)
    ],
    switch (v,+,1) [
        (0, statements [
            println [vnusout,"zero"]
        ]),
        (1, statements [
            println [vnusout,"one"]
        ]),
        (2, statements [
            println [vnusout,"two"]
        ]),
	(default,statements [
	    println [vnusout,"default"]
	])
    ]
]
