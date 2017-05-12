// This is a test program for the sugar V-nus frontend.

globalpragmas <$boundscheck=true$>;

x: float;
i: int;
desti: int;
V: view 1 of int;
W: view 1 of int;
n: int;
c: complex;
c2: complex;
tpi: int;
tpo: int;
tpio: int;
b: boolean;
s: string;
f: int;
shp: shape [5, 8] [block 5,-] of int;
A: shape [20] [block 5] of int;
B: shape [20] of int;
aview: view 2 of int;
//V0: view 0 of int;
//s0: shape [1] of int;

external procedure v__writeString( f: int, s: string, p: int );
external procedure v__writeInt( f: int, n: int, p: int );
external procedure v__writeComplex( f: int, c: complex, p: int );
external procedure v__writeBoolean( f: int, b: boolean, p: int );
external procedure v__writeFloat( f: int, r: float, p: int );
external function v__open( nm: string, mode: string, p: int ) returns int;
external procedure v__close( f: int, p: int );
external numberOfProcessors: int;

procedure test_scopes( f: int, n: int, p: int )
{
    nloc: int;

    nloc := n*2;
    <$constant$> v__writeInt( f, n, p );
    v__writeString( f, " ", p );
    v__writeInt( f, nloc, p );
    v__writeString( f, "\n", p );
    {
	nsub: int;

	nsub := nloc*3;
	v__writeInt( f, n, p );
	v__writeString( f, " ", p );
	v__writeInt( f, nloc, p );
	v__writeString( f, " ", p );
	v__writeInt( f, nsub, p );
	v__writeString( f, "\n", p );
	if( nsub>nloc ){
	    n: int;

	    n := 3;
	    while( n>0 ){
		n := n-1;
		nsub := nsub + n;
	    }
	    v__writeInt( f, nsub, p );
	    v__writeString( f, "\n", p );
	}
	else {
	    n: int;

	    n := 6;
	    while( n>0 ){
		n := n-1;
		nsub := nsub + n;
	    }
	    v__writeInt( f, nsub, p );
	    v__writeString( f, "\n", p );
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

procedure empty_proc()
{
}

procedure empty_proc1( v: pointer to shape [*] of int )
{
l::   (*v)[0] := 42;
}

procedure Print1dView( f: int, V: view 1 of int, p: int )
{
    for [ i:getsize( V, 0 ) ] {
	print( f, p, V[i], " " );
    }
    println( f, p, "" );
}

procedure Print1dShape( f: int, A: pointer to shape [*] of int, p: int )
{
    for [ i:getsize( *A, 0 ) ] {
	v__writeInt( f, (*A)[i], p );
	v__writeString( f, " ", p );
    }
    v__writeString( f, "\n", p );
    if( ismultidim( *A ) ){
	println( f, p, "Distributed over the multi-dimensional processor array" );
    }
    else {
	println( f, p, "Distributed over the one-dimensional processor array" );
    }
}

function inc( i: int ) returns res: int
{
    res := i+1;
}

function dec( i: int ) returns res: int
{
    res := i-1;
}

procedure testcmpinteger( f: int, a: int, b: int, p: int )
{
    v__writeInt( f, a, p );
    v__writeString( f, "=", p );
    v__writeInt( f, b, p );
    if( a=b ){
	v__writeString( f, " yes\n", p );
    }
    else {
	v__writeString( f, " no\n", p );
    }
    v__writeInt( f, a, p );
    v__writeString( f, "<>", p );
    v__writeInt( f, b, p );
    if (a<>b) {
	v__writeString( f, " yes\n", p );
    }
    else {
	v__writeString( f, " no\n", p );
    }
    v__writeInt( f, a, p );
    v__writeString( f, "<", p );
    v__writeInt( f, b, p );
    if( a<b ){
	v__writeString( f, " yes\n", p );
    }
    else {
	v__writeString( f, " no\n", p );
    }
    v__writeInt( f, a, p );
    v__writeString( f, "<=", p );
    v__writeInt( f, b, p );
    if( a<=b ) {
	v__writeString( f, " yes\n", p );
    }
    else {
	v__writeString( f, " no\n", p );
    }
    v__writeInt( f, a, p );
    v__writeString( f, ">", p );
    v__writeInt( f, b, p );
    if( a>b) {
	v__writeString( f, " yes\n", p );
    }
    else {
	v__writeString( f, " no\n", p );
    }
    v__writeInt( f, a, p );
    v__writeString( f, ">=", p );
    v__writeInt( f, b, p );
    if( a>=b ){
	v__writeString( f, " yes\n", p );
    }
    else {
	v__writeString( f, " no\n", p );
    }
}

forkall [p:numberOfProcessors] {
V0: view 0 of int;
s0: shape [1] of int;
    barrier;
    if( p=0 ) {
	f := v__open ( "runout", "w", p );
    }
    barrier;
    v__writeString( f, "Hello world\n", p );
    barrier;
    if( true ){
	v__writeString( f, "Yep, true is true\n", p );
    }
    else {
	v__writeString( f, "Huh, true is false?\n", p );
    }
    if( false ){
	v__writeString( f, "Huh, false is true?\n", p );
    }
    else {
	v__writeString( f, "Yep, false is false\n", p );
    }
    barrier;
    if( p=0 ){
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
	n := n / 3;
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
    };
    barrier;
    v__writeString( f, "c: ", p );
    v__writeComplex( f, c, p );
    v__writeString( f, "\n", p );
    barrier;
    if( p=0 ){
	c := +c;
    }
    barrier;
    v__writeString( f, "c: ", p );
    v__writeComplex( f, c, p );
    v__writeString( f, "\n", p );
    barrier;
    if( p = 0 ){
	c := c+c2;
    }
    barrier;
    v__writeString( f, "c: ", p );
    v__writeComplex( f, c, p );
    v__writeString( f, "\n", p );
    barrier;
    if( p=0 ) {
	c := c*c2;
    }
    barrier;
    v__writeString( f, "c: ", p );
    v__writeComplex( f, c, p );
    v__writeString( f, "\n", p );
    barrier;
    if( p=0 ){
	c := c-c2;
    }
    barrier;
    v__writeString( f, "c: ", p );
    v__writeComplex( f, c, p );
    v__writeString( f, "\n", p );
    barrier;
    if( p=0 ){
	c := c/c2;
    }
    barrier;
    v__writeString( f, "c: ", p );
    v__writeComplex( f, c, p );
    v__writeString( f, "\n", p );
    v__writeString( f, "n: ", p );
    v__writeInt( f, n, p );
    v__writeString( f, "\n", p );
    v__writeString( f, "i: ", p );
    v__writeInt( f, i, p );
    v__writeString( f, "\n", p );
    v__writeString( f, "b: ", p );
    v__writeBoolean( f, b, p );
    v__writeString( f, "\n", p );
    v__writeString( f, "x: ", p );
    v__writeFloat( f, x, p );
    v__writeString( f, "\n", p );
    v__writeString( f, "s: ", p );
    v__writeString( f, s, p );
    v__writeString( f, "\n", p );
    v__writeString( f, "c: ", p );
    v__writeComplex( f, c, p );
    v__writeString( f, "\n", p );
    v__writeString( f, "numberOfProcessors: ", p );
    v__writeInt( f, numberOfProcessors, p );
    v__writeString( f, "\n", p );
    v__writeString( f, "\n", p );
    barrier;
    if( p=0 ) {
	i := 14;
    }
    barrier;
    while( i>0 ){
	v__writeString( f, "i = ", p );
	v__writeInt( f, i, p );
	v__writeString( f, "\n", p );
	barrier;

	if( p=0 ) {
	    i := i-1;
	}
	barrier;
    }
    for [ i:3, j:4 ] {
	v__writeString( f, "i = ", p );
	v__writeInt( f, i, p );
	v__writeString( f, ", ", p );
	v__writeString( f, "j = ", p );
	v__writeInt( f, j, p );
	v__writeString( f, "\n", p );
    }
    testcmpinteger( f, 1, 2, p );
    testcmpinteger( f, 1, 1, p );
    testcmpinteger( f, 2, 1, p );
    testcmpinteger( f, 223, 1, p );
    barrier;
    forall [i:5, j:8] {
	if( IsOwner( shp[i,j], p ) ){
	    shp[i,j] := i+j;
	}
    }
    barrier;
    for [i:5,j:8] {
	v__writeString( f, "shp[", p );
	v__writeInt( f, i, p );
	v__writeString( f, ",", p );
	v__writeInt( f, j, p );
	v__writeString( f, "] = ", p );
	v__writeInt( f, shp[i,j], p );
	v__writeString( f, "\n", p );
    }
    aview := view [i:8, j:5] shp[j,i];
    for [i:8, j:5] {
	v__writeString( f, "aview[", p );
	v__writeInt( f, i, p );
	v__writeString( f, ",", p );
	v__writeInt( f, j, p );
	v__writeString( f, "] = ", p );
	v__writeInt( f, aview[i,j], p );
	v__writeString( f, "\n", p );
    }
    v__writeString( f, "A := 66;\n", p );

    barrier;
    for [i:getsize(A,0)] {
	if( IsOwner( A[i], p ) ){
	    A[i] := 66;
	}
    }
    barrier;

    Print1dView( f, view [i:getsize(A,0)] A[i], p );

    v__writeString( f, "V <- A;\nV := 2112;\n", p );

    barrier;
    V := view [j:10] A[j];

    for [i:getsize(V,0)] {
	V[i] := 2112;
    }

    Print1dView( f, V, p );
    Print1dShape( f, &A, p );

    W := view [j:getsize(V,0)] A[(3*j+2) mod 10];

    for [i:getsize(V,0)] {
	barrier;
	W[i] := i;
	barrier;
    }
    Print1dShape( f, &A, p );
    v__writeString( f, "A[i] := 0;\n", p );

    barrier;
    forall [i:getsize(A,0)] {
	if( IsOwner( A[i], p ) ){
	    A[i] := 0;
	}
    }
    barrier;

    v__writeString( f, "A: ", p );

    Print1dShape( f, &A, p );

    v__writeString( f, "B[i] := i;\n", p );

    forall [i:getsize(A,0)] {
	if( IsOwner( B[i], p ) ){
	    B[i] := i;
	}
    }
    barrier;

    v__writeString( f, "B: ", p );
    Print1dShape ( f, &B, p );

    v__writeString( f, "A[i] := B[i];\n", p );

    forall [i:getsize(A,0)] {
	if( IsOwner( A[i], p ) ){
	    A[i] := B[i];
	}
    }
    barrier;
    v__writeString( f, "A: ", p );
    Print1dShape( f, &A, p );

    v__writeString( f, "A[i] := 0;\n", p );

    barrier;
    forall [i:getsize(A,0)] {
	if( IsOwner( A[i], p ) ){
	    A[i] := 0;
	}
    }
    barrier;
    v__writeString( f, "A: ", p );
    Print1dShape( f, &A, p );

    v__writeString( f, "A[i] := B[i];\n", p );
    barrier;

    forall [i:getsize(A,0)] {
	desti := 19-i;
	if( Sender( B[i] ) = p ){
	    send( p, Owner( A[desti] ), B[i] );
	}
	if( IsOwner( A[desti], p ) ){
	    receive( p, Sender( B[i] ),  A[desti] );
	}
    }
    barrier;

    v__writeString( f, "A: ", p);
    Print1dShape( f, &A, p );

    println( f, p, "Block size of A: ", getblocksize( A, 0 ) );
//    v__writeString( f, "Block size of B: ", p );
//    v__writeInt( f, blocksize( B, 0 ), p );
//    v__writeString( f, "\n", p );

    test_scopes( f, 12, p );
    v__close( f, p );
    empty_proc();
    empty_proc1( &s0 );
    V0 := view [] s0[0];
}
