.foreach t ${nonvirtual ${typelist} ${subclasses ${typelist}}}
-function(operator new(r),new_$t(r))
-function(operator new(r),rdup_$t(r))
-function(operator new(r),new_$t(r)_list)
-function(operator new(r),rdup_$t(r)_list)
-function(operator new(r),real_new_$t(r))
-function(operator new(r),real_rdup_$t(r))
-function(operator new(r),real_new_$t(r)_list)
-function(operator new(r),real_rdup_$t(r)_list)
.endforeach
.foreach t ${nonvirtual ${typelist}}
-function(operator delete,rfre_$t)
-function(operator delete,rfre_$t_list)
-function(operator delete,fre_$t)
.endforeach
