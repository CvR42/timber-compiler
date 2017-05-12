..
.. Library template file
..
.. These templates are used to generate all kinds of stubs and interfaces
.. in the library.
..
..
.. These are the Vnus types for which read and write functions will
.. be generated.
.set typelist Byte Short Int Long Float Double Complex Boolean Char String
..
..
.. Templates to generate headers and calls for several functions.
.macro gen_read_hdr type prefix
.return "VnusInt $(prefix)read$(type)(VnusInt fileHandle, Vnus$(type) *elt)"
.endmacro
..
.macro gen_read_call type prefix
.return "$(prefix)read$(type)(fileHandle, elt)"
.endmacro
..
.macro gen_write_hdr type prefix
.return "VnusInt $(prefix)write$(type)(VnusInt fileHandle, const Vnus$(type) elt)"
.endmacro
..
.macro gen_write_call type prefix
.return "$(prefix)write$(type)(fileHandle, elt)"
.endmacro
..
.macro gen_open_hdr prefix
.return "VnusInt $(prefix)open(VnusString filename, VnusString access)"
.endmacro
..
.macro gen_open_call prefix
.return "$(prefix)open(filename, access)"
.endmacro
..
.macro gen_close_hdr prefix
.return "VnusInt $(prefix)close(VnusInt fileHandle)"
.endmacro
..
.macro gen_close_call prefix
.return "$(prefix)close(fileHandle)"
.endmacro
..
.macro gen_initialize_hdr prefix
.return "void $(prefix)initialize(void)"
.endmacro
..
.macro gen_initialize_call prefix
.return "$(prefix)initialize()"
.endmacro
..
..
.. Templates for generation of declarations.
.macro gen_typed_decl func type prefix qual
$(qual) ${call gen_$(func)_hdr $(type) $(prefix)};
.endmacro
..
.macro gen_untyped_decl func prefix qual
$(qual) ${call gen_$(func)_hdr $(prefix)};
.endmacro
..
..
.. Generate a declaration block for all functions.
.macro gen_extern_decls prefix
// Opens a file and returns the file handle ( an int )
// Access string is according to the c-standard
.call gen_untyped_decl open $(prefix) extern

// Close a file with handle fileHandle
// ferror result is returned
.call gen_untyped_decl close $(prefix) extern

// Read one item of a certain type from file with handle fileHandle
// Number of elements read or EOF is returned
// If result is not 1 a runtime warning message is generated
.foreach type $(typelist)
.call gen_typed_decl read $(type) $(prefix) extern
.endforeach

// Write one item of a certain type to file with handle fileHandle
// Number of elements written is returned
// If result is not 1 a runtime warning message is generated
.foreach type $(typelist)
.call gen_typed_decl write $(type) $(prefix) extern
.endforeach

// Initialization of stio_file : all files set to not used
.call gen_untyped_decl initialize $(prefix) extern
.endmacro
..
..
.. Add extra processor parameter, but remove void parameter list if present.
.macro gen_old_hdr hdr
.return ${subs (*)\(*void*,(*) \1(\2 ${subs (*)\) "\1, VnusInt /*p*/)" $(hdr)}}
.endmacro
..
..
.. Generate header for spar stubs. -- Leo
.macro gen_spar_hdr hdr
.return ${subs (*)\(*void*,(*) \1(\2 ${subs (*)\) "\1, VnusInt /*p*/)" $(hdr)}}
.endmacro
..
.. Generate header for spar stubs, no processor version -- Leo
.macro gen_spar_no_p_hdr hdr
.return ${subs (*)\(*void*,(*) \1(\2 ${subs (*)\) "\1)" $(hdr)}}
.endmacro
..
..
.. Another filter. This one does nothing.
.macro nop hdr
.return $(hdr)
.endmacro
..
..
.. Templates for generating stubs.
.macro gen_untyped_stub func caller callee qual filter
$(qual)${call $(filter) "${call gen_$(func)_hdr $(caller)}"}
    { return ${call gen_$(func)_call $(callee)}; }
.endmacro
..
.macro gen_untyped_void_stub func caller callee qual filter
$(qual)${call $(filter) "${call gen_$(func)_hdr $(caller)}"}
    { ${call gen_$(func)_call $(callee)}; }
.endmacro
..
.macro gen_typed_stub func type caller callee qual filter
$(qual)${call $(filter) "${call gen_$(func)_hdr $(type) $(caller)}"}
    { return ${call gen_$(func)_call $(type) $(callee)}; }
.endmacro
..
.. Generate a block of stubs.
.macro gen_qual_stubs caller callee qual filter
.call gen_untyped_stub open $(caller) $(callee) "$(qual)" $(filter)
.call gen_untyped_stub close $(caller) $(callee) "$(qual)" $(filter)
.foreach type $(typelist)
.call gen_typed_stub read $(type) $(caller) $(callee) "$(qual)" $(filter)
.endforeach
.foreach type $(typelist)
.call gen_typed_stub write $(type) $(caller) $(callee) "$(qual)" $(filter)
.endforeach
.call gen_untyped_void_stub initialize $(caller) $(callee) "$(qual)" $(filter)
.endmacro
..
.. Generate stubs for inside a .cc file.
.macro gen_stubs caller callee
.call gen_qual_stubs $(caller) $(callee) "" nop
.endmacro
..
.. Generate inline stubs for inside a .h file.
.macro gen_inline_old_stubs caller callee
.call gen_qual_stubs $(caller) $(callee) "inline " gen_old_hdr
.endmacro
..
..
.. Generate inline spar stubs. -- Leo
.macro gen_inline_spar_stubs caller callee
.call gen_qual_stubs $(caller) $(callee) "inline " gen_spar_no_p_hdr
.endmacro
..
.. Generate inline spar backward compatibility stubs. -- Leo
.macro gen_inline_spar_old_stubs caller callee
.call gen_qual_stubs $(caller) $(callee) "inline " gen_spar_hdr
.endmacro
..
..
.. Standard disclaimer
.macro gen_disclaimer
//
// This is code generated through Tm.
//
.endmacro
..
.. Always do it
.call gen_disclaimer
..