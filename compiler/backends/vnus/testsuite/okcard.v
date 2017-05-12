// File: okcard.v
//
// Tests of cardinality lists.

program
declarations [
    cardinalityvariable i
]
statements [
    for [i:4] statements [
	println [1,"i=",i]
    ],
    for [] statements [
	println [1,"Hi there"]
    ]
]
