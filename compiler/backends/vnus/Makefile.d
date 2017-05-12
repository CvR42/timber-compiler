symbol_table.o: symbol_table.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h ../vnuslib/vnusctl.h \
  symbol_table.h global.h
main.o: main.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h ../vnuslib/vnusctl.h error.h \
  global.h parser.h lex.h symbol_table.h symbol_support.h check.h \
  generate.h rewrite.h pragma.h service.h type.h
error.o: error.c /Users/reeuwijk/lab/include/tmc.h error.h
global.o: global.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h global.h
generate.o: generate.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h ../vnuslib/vnusctl.h global.h \
  error.h generate.h symbol_table.h symbol_support.h type.h service.h \
  pragma.h
collect.o: collect.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h error.h symbol_table.h global.h \
  collect.h
pragma.o: pragma.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h global.h service.h \
  ../vnuslib/vnusctl.h pragma.h
service.o: service.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h ../vnuslib/vnusctl.h global.h \
  service.h error.h symbol_table.h
type.o: type.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h ../vnuslib/vnusctl.h type.h \
  symbol_table.h symbol_support.h error.h service.h
lex.o: lex.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h ../vnuslib/vnusctl.h error.h \
  global.h lex.h tokens.h
typename.o: typename.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h typename.h type.h
analysis.o: analysis.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h analysis.h global.h \
  ../vnuslib/vnusctl.h pragma.h
symbol_support.o: symbol_support.c /Users/reeuwijk/lab/include/tmc.h \
  defs.h ../vnuslib/vnusbasetypes.h tmadmin.h ../vnuslib/vnusctl.h \
  global.h error.h generate.h symbol_table.h symbol_support.h pragma.h \
  type.h typename.h
parser.o: parser.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h ../vnuslib/vnusctl.h error.h lex.h \
  parser.h global.h symbol_table.h generate.h service.h type.h
tmadmin.o: tmadmin.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h
rewrite.o: rewrite.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h ../vnuslib/vnusctl.h rewrite.h \
  analysis.h pragma.h global.h error.h ../vnuslib/collectscopes.h \
  collectassigned.h type.h ispure.h service.h symbol_table.h \
  symbol_support.h
check.o: check.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h ../vnuslib/vnusctl.h error.h \
  symbol_table.h global.h collect.h check.h type.h service.h typename.h
collectassigned.o: collectassigned.c /Users/reeuwijk/lab/include/tmc.h \
  defs.h ../vnuslib/vnusbasetypes.h tmadmin.h global.h collectassigned.h \
  service.h ../vnuslib/vnusctl.h
ispure.o: ispure.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  ../vnuslib/vnusbasetypes.h tmadmin.h ispure.h
