program
declarations [
    //globalvariable x pointer function [record[]],
    globalvariable x pointer function [record []] int,
    globalvariable y pointer function [record[a:int,b:int]] int
]
statements [
    assign x cast pointer function [record[]] int y
]
