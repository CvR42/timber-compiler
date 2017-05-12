// Test for the equivalence of named records and nameless records.

program

declarations [
    record rec1 [a:int, b:char],
    record rec2 [a:int, b:char, c:int],
    record rec3 [n:int, l:pointer record rec3],
    globalvariable a record rec1 record [3,'a'],
    globalvariable b record rec2 record [2,'b',-1],
    globalvariable c record [f1:int, f2:char],
    globalvariable d record [f1:int, f2:char, f3:int] record [0,'@',0],
    globalvariable pa pointer record rec1 new record rec1,
    globalvariable pb pointer record rec2 new record rec2
]

statements [
    assign c a,
    assign b d,
    assign pa cast pointer record rec1 pb,
    assign pb cast pointer record rec2 pa
]
