program
declarations [
    externalvariable vnusout int,
    globalvariable p pointer shape [dontcare] int
]
statements [
    assign p nullednew shape [20] int null,
    if (p,<>,cast pointer shape [dontcare] int null) statements [
        assign (*p,[0]) 122,
        assign (*p,[1]) -2,
        println [vnusout,(*p,[1])," ",(*p,[0])],
        delete p
    ]
    statements [
        println [vnusout,"Cannot allocate shape"]
    ]
]
