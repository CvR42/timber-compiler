program
declarations [
    externalvariable vnusout int,
    globalvariable p pointer procedure [],
    procedure hello [] statements [
	println[vnusout,"Hello there"]
    ]
]
statements [
    assign p &hello,
    procedurecall *p []
]
