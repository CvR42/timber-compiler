program

declarations 
[
  cardinalityvariable step,
  cardinalityvariable k,

  procedure Init []
  statements [],
  procedure Pivot []
  statements [],
  procedure Row []
  statements [],
  procedure End []
  statements []
]

statements
[
  pragma [task=(Init (40))]
  procedurecall Init [],

  forall [step: 8]
  statements
  [
    pragma [receive=(if (eq @step 0) (Init () 7) (Row ((subtract @step 1) 0) 7)),
            task=(Pivot (sum @step 40))]
    procedurecall Pivot [],

    forall [k: ((8,-,step),-,1)]
    statements
    [
      pragma [receive=(Pivot (@step) 7),
              receive=(if (eq @step 0) (Init () 7) (Row ((subtract @step 1) (sum @k 1)) 7)),
              task=(Row 40)]
      procedurecall Row []
    ]
  ],

  pragma [receive=(for i 8 (Pivot i 7)),
          task=(End (10))]
  procedurecall End []
]
