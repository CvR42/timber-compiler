parser.o: parser.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h error.h lex.h parser.h check.h global.h symbol_table.h \
  prettyprint.h service.h
tmadmin.o: tmadmin.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h
constfold.o: constfold.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h error.h constfold.h analyze.h service.h dump.h
mangle.o: mangle.c /Users/reeuwijk/lab/include/tmc.h defs.h config.h \
  tmadmin.h vnus_types.h error.h symbol_table.h global.h mangle.h \
  collect.h typederive.h typename.h exprname.h service.h doimports.h \
  analyze.h isconstant.h iszero.h constfold.h substitute.h regtype.h \
  inliner.h ispure.h codesize.h dump.h pragma.h markfinal.h \
  collectassigned.h getunbounds.h countrefs.h
check.o: check.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h error.h symbol_table.h global.h check.h collect.h \
  typederive.h service.h typename.h exprname.h analyze.h
rewrite.o: rewrite.c /Users/reeuwijk/lab/include/tmc.h defs.h config.h \
  tmadmin.h vnus_types.h global.h symbol_table.h typederive.h rewrite.h \
  service.h error.h analyze.h pragma.h typename.h inliner.h codesize.h \
  getscopes.h markadmin.h countrefs.h isgcsetting.h isgc.h
lower.o: lower.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h global.h symbol_table.h typederive.h lower.h service.h \
  error.h analyze.h pragma.h dump.h
fwdregtype.o: fwdregtype.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h error.h symbol_table.h global.h fwdregtype.h \
  collect.h typederive.h typename.h service.h doimports.h
regtype.o: regtype.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h error.h symbol_table.h global.h regtype.h collect.h \
  typederive.h typename.h service.h doimports.h dump.h
substitute.o: substitute.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h global.h symbol_table.h typederive.h \
  substitute.h service.h error.h typename.h exprname.h
markfinal.o: markfinal.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h global.h markfinal.h service.h
collectassigned.o: collectassigned.c /Users/reeuwijk/lab/include/tmc.h \
  defs.h tmadmin.h vnus_types.h global.h collectassigned.h service.h
isgc.o: isgc.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h symbol_table.h pragma.h global.h isgc.h
addgc.o: addgc.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h addgc.h service.h isgc.h isgcsetting.h analyze.h \
  symbol_table.h pragma.h global.h error.h
codesize.o: codesize.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h symbol_table.h pragma.h global.h codesize.h
getscopes.o: getscopes.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h getscopes.h service.h
getunbounds.o: getunbounds.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h getunbounds.h service.h
isconstant.o: isconstant.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h isconstant.h
iszero.o: iszero.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h symbol_table.h pragma.h global.h analyze.h iszero.h
markused.o: markused.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h service.h markused.h symbol_table.h
noreturn.o: noreturn.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h global.h symbol_table.h typederive.h service.h error.h \
  typename.h exprname.h
isgcsetting.o: isgcsetting.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h symbol_table.h pragma.h global.h service.h \
  isgcsetting.h
countrefs.o: countrefs.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h countrefs.h symbol_table.h
ispure.o: ispure.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h ispure.h
analyze.o: analyze.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h analyze.h service.h global.h symbol_table.h error.h \
  constfold.h
collect.o: collect.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h symbol_table.h collect.h typederive.h service.h error.h
constant.o: constant.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h error.h
doimports.o: doimports.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h doimports.h symbol_table.h service.h error.h \
  global.h parser.h fwdregtype.h regtype.h lower.h config.h
dump.o: dump.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h dump.h
error.o: error.c /Users/reeuwijk/lab/include/tmc.h defs.h config.h \
  tmadmin.h vnus_types.h error.h service.h
exprname.o: exprname.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h exprname.h typename.h error.h service.h
global.o: global.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h global.h config.h
lex.o: lex.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h error.h global.h lex.h parser-tokens.h
main.o: main.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h error.h global.h parser.h lex.h check.h mangle.h \
  fwdregtype.h regtype.h rewrite.h lower.h prettyprint.h version.h \
  service.h config.h constfold.h doimports.h markused.h addgc.h \
  markadmin.h
pragma.o: pragma.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h service.h global.h pragma.h
prettyprint.o: prettyprint.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h global.h error.h prettyprint.h symbol_table.h \
  constant.h collect.h typederive.h typename.h service.h pragma.h dump.h \
  markadmin.h iszero.h
markadmin.o: markadmin.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h symbol_table.h markadmin.h
inliner.o: inliner.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h global.h service.h error.h inliner.h analyze.h \
  isconstant.h substitute.h symbol_table.h getscopes.h dump.h noreturn.h
service.o: service.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h global.h service.h error.h typederive.h analyze.h \
  isconstant.h typename.h constfold.h dump.h
symbol_table.o: symbol_table.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h global.h error.h prettyprint.h symbol_table.h \
  typederive.h typename.h doimports.h service.h collect.h dump.h
typederive.o: typederive.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  tmadmin.h vnus_types.h error.h symbol_table.h typederive.h service.h \
  collect.h
typename.o: typename.c /Users/reeuwijk/lab/include/tmc.h defs.h tmadmin.h \
  vnus_types.h typename.h exprname.h service.h typederive.h error.h
vnus_types.o: vnus_types.c /Users/reeuwijk/lab/include/tmc.h defs.h \
  vnus_types.h tmadmin.h error.h service.h
