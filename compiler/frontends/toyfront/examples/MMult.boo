MODULE MMult;

FROM Math IMPORT Reduce;

CONST 
  n = 5
END;

TYPE 
  Matrix = SHAPE {n # n} OF INTEGER
END;


    // The sequential version.
FUNCTION MatrixMultiply1 (IN B, C : Matrix) RESULT A : Matrix;

VAR 
  i1, i2, i3 : NATURAL;
END;

BEGIN
    A |= 0;
    ITER i1 OVER n DO
        ITER i2 OVER n DO
            ITER i3 OVER n DO
                A[i1,i2] := A[i1,i2] + B[i1,i3]*C[i3,i2];
            END;
        END;
    END;
END MatrixMultiply1;


    // The breadth-first, in-place parallel version.
FUNCTION MatrixMultiply2 (IN B, C : Matrix) RESULT A : Matrix;

VAR 
  k : NATURAL
END;

BEGIN
    A |= 0;
    ITER k OVER n DO
        A[i:_,j:_] := A[i,j] + B[i,k]*C[k,j];
    END;
END MatrixMultiply2;


    // The space-hungry, computationally very parallel version.
FUNCTION MatrixMultiply3 (IN B, C : Matrix) RESULT A : 
Matrix;

VAR 
  temp : SHAPE {n # n # n} OF INTEGER
END;

BEGIN
    temp[i:_,j:_,k:_] |= B[i,k] * C[k,j];
    A[i:_,j:_] |= Reduce('+', temp[i,j,_]);
END MatrixMultiply3;


VAR 
  A, B, C : Matrix
END;


BEGIN
    B |= 3;
    C |= 4;

    A := MatrixMultiply1(B, C);
    WriteI(StdOut, A[0,0]); WriteS(StdOut, '\n');

    A := MatrixMultiply2(B, C);
    WriteI(StdOut, A[0,0]); WriteS(StdOut, '\n');
    
    A := MatrixMultiply3(B, C);
    WriteI(StdOut, A[0,0]); WriteS(StdOut, '\n');

END MMult.

