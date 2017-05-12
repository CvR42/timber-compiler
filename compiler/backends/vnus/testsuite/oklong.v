// Test the behavior or longs

program

declarations [
    externalvariable vnusout int,
    globalvariable v long,
    cardinalityvariable i
]
statements [
    assign v 1l,
    for [i:62] statements [
	assign v (v,+,v),
	println [vnusout,v]
    ]
]
