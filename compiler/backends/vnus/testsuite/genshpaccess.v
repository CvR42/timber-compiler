// Generated by sugar Vnus front-end

program pragma [boundscheck = false]

declarations [
    globalvariable A shape [30000] double,
    cardinalityvariable loop,
    cardinalityvariable i
]

statements [

	statements main [
     
	   for [loop:500] statements [

			forall [i:30000] statements [
                assign (A, [i]) (cast Double i, *, 1.1d)
            ]

		],

		print [1, "Checksum: "],
		println [1, (A, [1500])]
	]
]
