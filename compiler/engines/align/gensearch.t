.. File: gensearch.t
..
.. Tree walker for searching operations.
.. TRUE or FALSE is returned depending on user actions using 'sym'
..
.. Return 'walk' or 'action', depending on the contents of 'actors'
.macro walkername t
.if ${member $t $(actors)}
.return action
.else
.return walker
.endif
.endmacro
..
.. Given an indent 'indent', the name 'var' of the variable that holds
.. the datastructure we're walking on, and a type 't', of that variable,
.. generate a return statement/
.macro generate_walker_return indent var t
$(indent)return result;
.endmacro
..
.. Given an indent 'ident' and a type 't', generate a list of local
.. declarations for the walker function of that type.
.macro generate_walker_locals indent var t
$(indent) int result = FALSE;
.if ${member ${stemname $t} ${typelist}}
$(indent)if( $(var) == 0 ){
$(indent)    return result;
$(indent)}
.endif
.endmacro
..
.. Given the name 'var' of the variable that holds the datastructure
.. we`re walking on, and the type 't' of that variable, generate a
.. signature for the walker function of the given type.
.macro generate_walker_signature var t
static int rewrite_$t_walker(
  $t $(var), tmsymbol sym )
.endmacro
..
.. Given the name 'var' of the variable that holds the datastructure
.. we're walking on, and the type 't' of that variable, generate a forward
.. declaration for the walker function of the given type.
.macro generate_walker_declaration var t
static int rewrite_$t_walker(
  $t $(var), tmsymbol sym );
.endmacro
..
.. Given an indent 'indent', a variable 'var' to walk into, the type of
.. the variable 'type', and the current type of the variable 'nowtype',
.. generate an invokation to an action.
.macro generate_action_call indent var type nowtype
.if ${eq $(type) $(nowtype)}
$(indent)result = result || rewrite_$(type)_action( $(var), sym );
.else
$(indent)result = result || rewrite_$(type)_action( to_$(type)( $(var) ), sym );
.endif
.endmacro
..
.. Given an indent 'indent', a variable 'var' to walk into, the type of
.. the variable 'type', and the current type of the variable 'nowtype',
.. generate an invokation to a walker.
.macro generate_walker_call indent var type nowtype
.if ${eq $(type) $(nowtype)}
$(indent)result = result || rewrite_$(type)_walker( $(var), sym );
.else
$(indent)result = result || rewrite_$(type)_walker( to_$(type)( $(var) ), sym );
.endif
.endmacro
..
.. Given an indent 'indent', a variable 'var' to walk into, the type of the
.. variable 'type', and the current type of the variable 'nowtype', generate
.. an invocation to an action or walker.
.macro generate_descent_call indent var type nowtype
.if ${member $(type) $(actors)}
.call generate_action_call "$(indent)" "$(var)" "$(type)" "$(nowtype)"
.else
.call generate_walker_call "$(indent)" "$(var)" "$(type)" "$(nowtype)"
.endif
.endmacro
..
.. Given an indent 'indent', a variable 'var', and a type 'type', generate
.. the body of an empty walker function.
.macro generate_empty_walker_body indent var type
$(indent)(void) $(var);
$(indent)(void) sym;
.endmacro
