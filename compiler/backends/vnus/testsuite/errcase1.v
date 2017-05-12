
program

declarations [

    procedure java_lang_Object_static_init_Object0_0 [cb1, p5]
    statements static_init_Object00 [
        assign java_lang_Object_needstatic_init_Object0_0 false
    ],

    procedure spar_testsuite_okarrcreate_static_init_okarrcreate0_0 [cb2, p6]
    statements static_init_okarrcreate00 [
        if field *cb2 java_lang_Object_needstatic_init_Object0_0 statements scope1 [
            procedurecall java_lang_Object_static_init_Object0_0 [java_lang_Object_static_init_Object0_0, p6]
        ] statements [],
        assign spar_testsuite_okarrcreate_needstatic_init_okarrcreate0_0 false
    ],

    record commonBlockRecordType [a : int],

    formalvariable cb1 static_init_Object00 pointer record commonBlockRecordType,
    formalvariable cb2 static_init_okarrcreate00 pointer record commonBlockRecordType,
    formalvariable p5 static_init_Object00 int,
    formalvariable p6 static_init_okarrcreate00 int
]

statements [
]
