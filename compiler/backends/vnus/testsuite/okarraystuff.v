program

declarations [
    globalvariable _a1 array char,
    globalvariable _a2 array boolean,
    globalvariable _a3 array int
]

statements [
    assign _a1 newarray char 23,
    assign _a2 newfilledarray boolean 40 false,
    assign _a3 newfilledarray int 20 14,
    assign (_a1,[checkedindex 2 23]) '?',
    assign (_a2,[2]) true,
    println [ 1, (_a1,[checkedindex 2 23])],
    println [ 1, (_a2,[checkedindex 2 40])],
    println [ 1, (_a2,[1])],
    println [ 1, (_a3,[1])],
]
