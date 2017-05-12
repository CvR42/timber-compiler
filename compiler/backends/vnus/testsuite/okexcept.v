program
declarations [
    formalvariable v catchblock pointer record [],
    record r1 [
	id:int
    ],
    globalvariable x pointer record [id:int]
]
statements [
    catch v statements catchblock [
	println [1,"1"],
	assign x new record[id:int],
	assign field *x id 42,
	throw x
    ]
    statements [
	println [1,"2"]
    ],
    println [1,"3"]
]
