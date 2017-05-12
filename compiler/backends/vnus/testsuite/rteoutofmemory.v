program
declarations [
    externalprocedure Java_spar_compiler_GC_setMemoryLimit[sm],
    formalvariable sm Java_spar_compiler_GC_setMemoryLimit long,
    cardinalityvariable i,
    record rec [
	a:complex,
	b:complex,
	c:complex,
	d:complex,
	e:complex,
	f:complex,
	g:complex,
	h:complex,
	i:complex,
	j:complex,
	k:complex,
	l:complex,
	m:complex,
	n:complex,
	o:complex,
	p:complex,
	q:complex,
	r:complex,
	s:complex
    ],
    globalvariable p pointer record rec 
]
statements [
    procedurecall Java_spar_compiler_GC_setMemoryLimit[ 200000L ],
    while true statements [
	assign p new record rec,
	assign field *p a complex 1.0d 1.0d
    ]
]
