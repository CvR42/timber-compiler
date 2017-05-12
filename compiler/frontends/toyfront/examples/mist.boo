MODULE HelloWorld;

FROM StandardIO IMPORT StdOut, WriteS;

BEGIN

  a[1) := 1;
  WriteS(StdOut, 'Hello World');

END HelloWorld;
