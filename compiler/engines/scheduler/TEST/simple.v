program

declarations 
[
  cardinalityvariable step,
  cardinalityvariable k,

  procedure Init []
  statements [],
  procedure End []
  statements []
]

statements
[
  pragma [task=(Init (40)),
	  send = (End () 5)]
  procedurecall Init [],

  pragma [receive=(Init () 5),
          task=(End (10))]
  procedurecall End []
]
