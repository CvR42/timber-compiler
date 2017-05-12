#include <stdlib.h>
#include <stdio.h>
#include <booster/kernel.h>

/*
	The translated output hopefully...
*/

extern void main(int argc, char **argv);
extern void user_main_proc(DATA **result, DATA **arg1, DATA **arg2);
extern void main_proc(DST **dst, DATA **result, DATA **arg1, DATA **arg2);
extern DATA *user_main_func(DATA **arg);
extern DATA *main_func(DST **dst, DATA **result, DATA **arg);
extern DATA *user_main_const(DATA **arg);
extern DATA *main_const(DST **dst, DATA **result, DATA **arg);

void main(int argc, char *argv[])
{
	DST **dst;
	DATA *ap, *bp, *cp;

	dst = make_dst(NULL, 0);
	ap = make_data(dst, 0);
	bp = make_data(dst, 1);
	cp = make_data(dst, 2);

	printf("%d\n", cp -> value -> integer);
	user_main_proc(&cp, &ap, &bp);
	printf("%d\n", cp -> value -> integer);
	cp = user_main_func(&cp);
	printf("%d\n", cp -> value -> integer);
	cp = user_main_const(&cp);
	printf("%d\n", cp -> value -> integer);

	destroy_data(ap);
	destroy_data(bp);
	destroy_data(cp);
	destroy_dst(dst);
	exit(0);
}

void user_main_proc(DATA **result, DATA **arg1, DATA **arg2)
/*
	PROCEDURE proc (
		OUT result: SOMETHING;
		INOUT arg1: SOMETHING;
		IN arg2: SOMETHING
	);
*/
{
	DST **dst;
	DATA *local2;

	dst = make_dst(NULL, 1);
	if (is_volatile(*arg2) == TRUE)
		local2 = *arg2;
	else {
		local2 = create_data();
		local2 = copy_data(local2, *arg2);
	}
	destroy_data(*result);
	*result = make_data(dst, 3);
	main_proc(dst, result, arg1, &local2);
	destroy_data(local2);
	destroy_dst(dst);
}

void main_proc(DST **dst, DATA **result, DATA **arg1, DATA **arg2)
{
	(*result) -> value -> integer =
		(*arg1) -> value -> integer + (*arg2) -> value -> integer;
}

DATA *user_main_func(DATA **arg)
/*
	FUNCTION func ( IN arg: SOMETHING ) RESULT result: SOMETHING;
*/
{
	DST **dst;
	DATA *result;
	DATA *local;

	dst = make_dst(NULL, 2);
	if (is_volatile(*arg) == TRUE)
		local = *arg;
	else {
		local = create_data();
		local = copy_data(local, *arg);
	}
	result = make_data(dst, 3);
	result = main_func(dst, &result, &local);
	destroy_dst(dst);
	return result;
}

DATA *main_func(DST **dst, DATA **result, DATA **arg)
{
	(*result) -> value -> integer = - (*arg) -> value -> integer;
	return *result;
}

DATA *user_main_const(DATA **arg)
/*
	FUNCTION const ( IN arg: SHAPE {CONST n} OF SOMETHING )
	RESULT result: SHAPE {n} OF SOMETHING;
*/
{
	DST **dst;
	DATA *result;
	DATA *local;
	DATA *var[1];

	var[0] = *arg;
	dst = make_dst(var, 3);
	if (is_volatile(*arg) == TRUE)
		local = *arg;
	else {
		local = create_data();
		local = copy_data(local, *arg);
	}
	result = make_data(dst, 4);
	result = main_const(dst, &result, &local);
	destroy_dst(dst);
	return result;
}

DATA *main_const(DST **dst, DATA **result, DATA **arg)
{
	DATA *n;

	n = make_const(dst, 0);
	(*result) -> value -> integer =
		(*arg) -> value -> integer - n -> value -> natural;
	destroy_data(n);
	return *result;
}

DATA *make_data(DST **dst, int index)
{
	DATA *result;

	result = create_data();
	switch (index) {
		case 0:
			result = create_data_details(1, 1, result);
			result -> value -> integer = 5;
			*(result -> mapping) = DONT_CARE;
			result -> type -> dimension = 1;
			result -> type -> cardinality = 1;
			result -> type -> form -> type = FORM_NATURAL;
			result -> type -> form -> value.natural = 1;
			result -> type -> base_type = TYPE_INTEGER;
			*(result -> info) = DONT_CARE;
			break;
		case 1:
			result = create_data_details(1, 1, result);
			result -> value -> integer = 3;
			*(result -> mapping) = DONT_CARE;
			result -> type -> dimension = 1;
			result -> type -> cardinality = 1;
			result -> type -> form -> type = FORM_NATURAL;
			result -> type -> form -> value.natural = 1;
			result -> type -> base_type = TYPE_INTEGER;
			*(result -> info) = DONT_CARE;
			break;
		case 2:
			result = create_data_details(1, 1, result);
			result -> value -> integer = 9;
			*(result -> mapping) = DONT_CARE;
			result -> type -> dimension = 1;
			result -> type -> cardinality = 1;
			result -> type -> form -> type = FORM_NATURAL;
			result -> type -> form -> value.natural = 1;
			result -> type -> base_type = TYPE_INTEGER;
			*(result -> info) = DONT_CARE;
			break;
		case 3:
			result = create_data_details(1, 1, result);
			result -> value -> integer = 0;
			*(result -> mapping) = DONT_CARE;
			result -> type -> dimension = 1;
			result -> type -> cardinality = 1;
			result -> type -> form -> type = FORM_NATURAL;
			result -> type -> form -> value.natural = 1;
			result -> type -> base_type = TYPE_INTEGER;
			*(result -> info) = DONT_CARE;
			break;
		case 4:
			result = create_data_details(1, 1, result);
			result -> value -> integer = 0;
			*(result -> mapping) = DONT_CARE;
			result -> type -> dimension = 1;
			result -> type -> cardinality = 1;
			result -> type -> form -> type = FORM_CONSTANT;
			result -> type -> form -> value.natural = 0;
			result -> type -> base_type = TYPE_INTEGER;
			*(result -> info) = DONT_CARE;
			break;
		default:
			runtime_error("impossible index %d in make_data\n");
	}
	return result;
}

DST **make_dst(DATA **arg, int index)
{
	DST **dst;
	DATA *data;

	switch(index) {
		case 0:
		case 1:
		case 2:
			dst = create_dst(0);
			break;
		case 3:
			dst = create_dst(1);
			data = create_data_details(0, 1, *dst);
			data -> value -> natural =
				(*arg) -> type -> form -> value.natural;
			*(data -> mapping) = DONT_CARE;
			data -> type -> dimension = 0;
			data -> type -> cardinality = 1;
			data -> type -> form = NULL;
			data -> type -> base_type = TYPE_NATURAL;
			*(data -> info) = DONT_CARE;
			break;
		default:
			runtime_error("impossible index in make_literal\n");
	}
	return dst;
}
