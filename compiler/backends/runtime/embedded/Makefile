
.PHONY : default clean empty

default:
	ln -sf ensemble.c ensemble-host-i386.c
	$(MAKE) -f Makefile.host
	$(RM) ensemble-host-i386.c
	ln -sf ensemble.c ensemble-trimedia.c
	$(MAKE) -f Makefile.trimedia
	$(RM) ensemble-trimedia.c

clean :
	$(RM) *.o *~ *.a ensemble-host-i386.c ensemble-trimedia.c

empty : clean
	$(RM) *.a
