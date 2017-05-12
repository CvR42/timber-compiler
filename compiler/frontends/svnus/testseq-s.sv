// This is a test program for the sugar V-nus frontend.
// It tests the execution of sequential programs.

/* This a C-style comment. */
/* This is a longer
 * C-style
 * comment
 * spanning
 * several
 * lines. This tests whether the lines are correctly counted
 * a comment.
 */

globalpragmas <$boundscheck=true$>;

<$notorious$> x: float = 0.0;
i: int;
V: view 1 of int;
W: view 1 of int;
n: int = 2;
c: complex;
c2: complex;
tpi: int;
tpo: int;
tpio: int;
b: boolean;
s: string;
f: int = 0;
shp: shape [5, 8] [block 5, -] of int;
A: shape [20] [block 5] of int;
B: shape [20] of int;
aview: view 2 of int;
V0: view 0 of int;
s0: shape [1] of int;
rec: record [a:int, b:float, c:char];
rec2: record [a:int, b:float];
prec: pointer to record [a:int, b:float, c:char];
prec2: pointer to record [a:int, b:float];

<$pure$> external procedure v__writeString( f: int, s: string );
external procedure v__writeLong( f: int, l: long );
external procedure v__writeInt( f: int, i: int );
external procedure v__writeComplex( f: int, c: complex );
external procedure v__writeBoolean( f: int, b: boolean );
external procedure v__writeFloat( f: int, r: float );
external function v__open( nm: string, mode: string ) returns int;
external procedure v__close( f: int );

<$pure$> procedure test_scopes( f: int, n: int )
{
    nloc: int;

    nloc := n*2;
    v__writeInt( f, (<$notorious$> n) );
    v__writeString( f, " " );
    v__writeInt( f, nloc );
    v__writeString( f, "\n" );
    <$pointless, testboolean = FALSE, testint = 42, teststring = "hello world" $>;
    {
	nsub: int;

	nsub := nloc*3;
	v__writeInt( f, n );
	v__writeString( f, " " );
	v__writeInt( f, nloc );
	v__writeString( f, " " );
	v__writeInt( f, nsub );
	v__writeString( f, "\n" );
	<$bigdeal$> if ( nsub>nloc ){
	    n: int;

	    n := 3;
	    while( n>0 ){
		n := n-1;
		nsub := nsub + n;
	    }
	    v__writeInt( f, nsub );
	    v__writeString( f, "\n" );
	}
	else
	{
	    n: int;

	    n := 6;
	    while( n>0 ){
		n := n-1;
		nsub := nsub + n;
	    }
	    v__writeInt( f, nsub );
	    v__writeString( f, "\n" );
	}
    }
    for [(i:nloc)] {
	A: shape[23] of int;

	forall [i1:getsize(A,0)] {
	    z: int;

	    z := i1+2;
	    A[i1] := z;
	}
    }
}

procedure test_dynamic(
    f: int,
    aout: pointer to shape [10] of int,
    bout: pointer to shape [*] of int,
    cout: pointer to view 1 of int,
    dout: pointer to view 1 of int,
    ain: shape [10] of int,
    bin: shape [*] of int,
    cin: view 1 of int,
    din: view 1 of int
)
{
    tmp: shape [getsize(bin,0)] of int;
    lim: int;

    lim := 10;
    if( getsize(bin,0)<lim ){
        lim := getsize(bin,0);
    }
    if( getsize(din,0)<lim ){
        lim := getsize(din,0);
    }
    v__writeString( f, "lim = " );
    v__writeInt( f, lim );
    v__writeString( f, "\n" );
    foreach [i:lim] {
	<$testpragmamangling=ain[i]$>
        tmp[i] := ain[i]+bin[i]+cin[i]+din[i];
    }
    forall [i:lim] {
        (*aout)[i] := tmp[i];
        (*bout)[i] := tmp[i];
        (*cout)[i] := tmp[i];
        (*dout)[i] := tmp[i];
    }
}

procedure run_test_dynamic( f: int, n: int )
{
    ain: shape [10] of int;
    bin: shape [n] of int;
    cin: shape [10] of int;
    din: shape [n+2] of int;
    vcin: view 1 of int;
    vdin: view 1 of int;
    aout: shape [10] of int;
    bout: shape [n] of int;
    cout: shape [10] of int;
    dout: shape [n+2] of int;
    vcout: view 1 of int;
    vdout: view 1 of int;
    bad: boolean;
    allok: boolean;

    vcin := view [i:10] cin[i];
    vdin := view [i:n+2] din[i];
    vcout := view [i:10] cout[i];
    vdout := view [i:n+2] dout[i];
    forall [i:10] { ain[i] := 1; }
    forall [i:n] { bin[i] := 1; }
    forall [i:10] { cin[i] := 1; }
    forall [i:n+2] { din[i] := 1; }
    test_dynamic( f, &aout, &bout, &vcout, &vdout, ain, bin, vcin, vdin );
    allok := true;
    for [i:10] {
        if( aout[i] <> 4 ){
            v__writeString( f, "test_dynamic: bad value for aout\n" );
            v__writeString( f, "aout[" );
            v__writeInt( f, i );
            v__writeString( f, "] = " );
            v__writeInt( f, aout[i] );
            v__writeString( f, "\n" );
            allok := false;
        }
    }
    bad := false;
    for [i:10] {
        bad := bad or ( bout[i] <> aout[i] );
    }
    if( bad ){
        v__writeString( f, "test_dynamic: bad value for bout\n" );
        allok := false;
    }
    bad := false;
    for [i:10] {
        bad := bad or ( cout[i] <> bout[i] );
    }
    if( bad ){
        v__writeString( f, "test_dynamic: bad value for cout\n" );
        allok := false;
    }
    bad := false;
    for [i:10] {
        bad := bad or ( dout[i] <> cout[i] );
    }
    if( bad ){
        v__writeString( f, "test_dynamic: bad value for dout\n" );
        allok := false;
    }
    if( allok ){
        v__writeString( f, "test_dynamic: everything ok\n" );
    }
}

procedure empty_proc()
{
}

procedure empty_proc1( v: pointer to shape [1] of int )
{
l ::    (*v)[0] := 42;
}

procedure Print1dView( f: int, V: view 1 of int )
{
    for [ i:getsize(V,0) ] {
	v__writeInt( f, V[i] );
	v__writeString( f, " " );
    }
    v__writeString( f, "\n" );
    return;
}

procedure Print1dShape( f: int, A: shape [*] of int )
{
    for [ i:getsize(A,0) ] {
	print( f, A[i], " " );
    }
    println( f, "" );
}

function inc( i: int ) returns res: int
{
    res := i+1;
}

function dec( i: int ) returns res: int
{
    return i-1;
}

procedure testcmpint( f: int, a: int, b: int )
{
    v__writeInt( f, a );
    v__writeString( f, "=" );
    v__writeInt( f, b );
    if( a=b ){
	v__writeString( f, " yes\n" );
    }
    else {
	v__writeString( f, " no\n" );
    }
    v__writeInt( f, a );
    v__writeString( f, "<>" );
    v__writeInt( f, b );
    if (a<>b) {
	v__writeString( f, " yes\n" );
    }
    else {
	v__writeString( f, " no\n" );
    }
    v__writeInt( f, a );
    v__writeString( f, "<" );
    v__writeInt( f, b );
    if( a<b ){
	v__writeString( f, " yes\n" );
    }
    else {
	v__writeString( f, " no\n" );
    }
    v__writeInt( f, a );
    v__writeString( f, "<=" );
    v__writeInt( f, b );
    if( a<=b ) {
	v__writeString( f, " yes\n" );
    }
    else {
	v__writeString( f, " no\n" );
    }
    v__writeInt( f, a );
    v__writeString( f, ">" );
    v__writeInt( f, b );
    if( a>b) {
	v__writeString( f, " yes\n" );
    }
    else {
	v__writeString( f, " no\n" );
    }
    v__writeInt( f, a );
    v__writeString( f, ">=" );
    v__writeInt( f, b );
    if( a>=b ){
	v__writeString( f, " yes\n" );
    }
    else {
	v__writeString( f, " no\n" );
    }
}

f := v__open ( "runout", "w" );
v__writeString( f, "Hello world\n" );
println( f, "Hello world" );
if( true ){
    v__writeString( f, "Yep, true is true\n" );
}
else {
    v__writeString( f, "Huh, true is false?\n" );
}
if( false ){
    v__writeString( f, "Huh, false is true?\n" );
}
else {
    v__writeString( f, "Yep, false is false\n" );
}
b := false;
b := not b;
b := false or b;
b := true and b;
i := 2;
i := 50-i;
i := 5+i;
i := 5*i;
i := i/12;
i := -i;
i := +i;
i := inc( inc( i ) );
i := dec( i );
n := 7;
n := +n;
n := 7+n;
n := n-5;
n := n*12;
n := n/13;
n := n*6;
n := n/3;
x := 2.1;
x := -x;
x := +x;
x := 0.7+x;
x := x-5e2;
x := x*0.12;
x := x/0.13;
s := "test string";
c := complex ( 2, 3 );
c2 := complex ( 5, 12 );
c := -c;
v__writeString( f, "c: " );
v__writeComplex( f, c );
v__writeString( f, "\n" );
c := +c;
v__writeString( f, "c: " );
v__writeComplex( f, c );
v__writeString( f, "\n" );
c := c+c2;
v__writeString( f, "c: " );
v__writeComplex( f, c );
v__writeString( f, "\n" );
c := c*c2;
v__writeString( f, "c: " );
v__writeComplex( f, c );
v__writeString( f, "\n" );
c := c-c2;
v__writeString( f, "c: " );
v__writeComplex( f, c );
v__writeString( f, "\n" );
c := c/c2;
v__writeString( f, "c: " );
v__writeComplex( f, c );
v__writeString( f, "\n" );
v__writeString( f, "n: " );
v__writeInt( f, n );
v__writeString( f, "\n" );
v__writeString( f, "i: " );
v__writeInt( f, i );
v__writeString( f, "\n" );
v__writeString( f, "b: " );
v__writeBoolean( f, b );
v__writeString( f, "\n" );
v__writeString( f, "x: " );
v__writeFloat( f, x );
v__writeString( f, "\n" );
v__writeString( f, "s: " );
v__writeString( f, s );
v__writeString( f, "\n" );
v__writeString( f, "c: " );
v__writeComplex( f, c );
v__writeString( f, "\n" );
v__writeString( f, "\n" );
i := -42;
v__writeString( f, "i: " );
v__writeInt( f, i );
v__writeString( f, "\n" );
println( f, "n: ", n );
println( f, "i: ", i );
println( f, "b: ", b );
println( f, "s: ", s );

v__writeFloat( f, 1 );
v__writeString( f, "\n");

v__writeFloat( f, 1b+'c' );
v__writeString( f, "\n");


i := 14;
while( i>0 ){
    v__writeString( f, "i = " );
    v__writeInt( f, i );
    v__writeString( f, "\n" );

    i := i-1;
}
for [ i:3, j:4 ] {
    v__writeString( f, "i = " );
    v__writeInt( f, i );
    v__writeString( f, ", " );
    v__writeString( f, "j = " );
    v__writeInt( f, j );
    v__writeString( f, "\n" );
}
testcmpint( f, 1, 2 );
testcmpint( f, 1, 1 );
testcmpint( f, 2, 1 );
testcmpint( f, 223, 1 );
forall [i:5, j:8] {
    shp[i,j] := i+j;
}
for [i:5,j:8] {
    v__writeString( f, "shp[" );
    v__writeInt( f, i );
    v__writeString( f, "," );
    v__writeInt( f, j );
    v__writeString( f, "] = " );
    v__writeInt( f, shp[i,j] );
    v__writeString( f, "\n" );
}
aview := view [i:8, j:5] shp[j,i];
for [i:8, j:5] {
    v__writeString( f, "aview[" );
    v__writeInt( f, i );
    v__writeString( f, "," );
    v__writeInt( f, j );
    v__writeString( f, "] = " );
    v__writeInt( f, aview[i,j] );
    v__writeString( f, "\n" );
}
v__writeString( f, "A := 66;\n" );

for [i:getsize(A,0)] {
    A[i] := 66;
}
//A := 66;

A := A;

Print1dShape( f, A );

v__writeString( f, "V <- A;\nV := 2112;\n" );

V  := view [j:10] A[j];

for [i:getsize(V,0)] {
    V[i] := 2112;
}

Print1dView( f, view [i:getsize(V,0)] V[i] );
Print1dShape( f, A );

W := view [j:getsize(V,0)] A[(3*j+2) mod 10];

for [i:getsize(V,0)] {
    W[i] := i;
}
Print1dShape( f, A );
v__writeString( f, "A[i] := 0;\n" );

forall [i:getsize(A,0)] {
    A[i] := 0;
}

v__writeString( f, "A: " );

Print1dShape( f, A );

v__writeString( f, "B[i] := i;\n" );

forall [i:getsize(A,0)] {
    B[i] := i;
}

v__writeString( f, "B: " );
Print1dShape( f, B );

v__writeString( f, "A[i] := B[i];\n" );

forall [i:getsize(A,0)] {
    A[i] := B[i];
}

v__writeString( f, "A: ");
Print1dShape( f, A );
test_scopes( f, 42 );
run_test_dynamic( f, 42 );
v__writeString( f, 3<4?"yes\n":"no\n" );
v__writeInt( f, (int) 12b );
v__writeString( f, "\n" );
rec.a := 3;
rec.b := 55;
rec.c := 'x';
rec := [ 1,1.0,'z' ];
rec2 := rec;
prec := &rec;
prec2 := (pointer to record [a:int, b:float]) prec;
rec2 := (record [a:int, b:float]) rec;
(*prec).a := (*prec2).a;
{
    ok: boolean;

    ok := prec=null;
    if( ok ){
	println( f, "Strange, pointer is null" );
    }
    else {
	println( f, "Good, pointer is not null" );
    }
}
prec2 := new( record [a:int, b:float] );
if(prec2=null) {
    println( f, "Strange, pointer is null" );
}
delete prec2;
garbagecollect;
switch 3 {
    4: {
	println( f, "four" );
    }

    3: {
	println( f, "three" );
    }

    2: {}
    
    1: {
	println( f, "one" );
	goto next;
    }

    0: {
	println ( f, "zero" );
    }
    default: {
	println( f, "default" );
    }
}
next::
println( f, "left shifts: int:",1<<2," byte:",1b<<2," short:",1s<<2," long:",1l<<2 );
println( f, "right shifts: int:",55>>2," byte:",55b>>2," short:",55s>>2," long:",55l>>2 );
println( f, "unsigned right shifts: int:",55>>>2," byte:",55b>>>2," short:",55s>>>2," long:",55l>>>2 );
println( f, "shifts of negative numbers: left:",(-11)<<2," right:",(-11)>>2," unsigned right:",(-11)>>>2 );
println( f, "bitwise operations: and:", 17 and 18, " or:",17 or 18," xor:",17 xor 18 );
println( f, "boolean operations: and:", true and false, " or:",true or false," xor:",true xor false );
println( f,"bitwise not:", not 12," ",not (-144));
println( f,"boolean not:", not false," ",not true);
v__close( f );
empty_proc();
empty_proc1( &s0 );
V0 := view [] s0[0];
