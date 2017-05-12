/* File: vnus_types.h
 *
 * Representation of the Vnus types in the backend.
 */

typedef tmstring vnus_string;
typedef bool vnus_boolean;
typedef long vnus_int;
typedef double vnus_real;
typedef struct str_complex { double re; double im; } vnus_complex;

#define rfre_vnus_natural(e)
#define rdup_vnus_natural(e) (e)
#define print_vnus_natural(st,e) print_uint(st,e)
#define rfre_vnus_boolean(e)
#define rdup_vnus_boolean(e) (e)
#define rfre_vnus_integer(e)
#define rdup_vnus_integer(e) (e)
#define rfre_vnus_real(e)
#define rdup_vnus_real(e) (e)
#define print_vnus_real(st,e) print_double(st,e)
#define rfre_vnus_complex(e)
#define rdup_vnus_complex(e) (e)
#define rfre_vnus_string(e) rfre_tmstring(e)
#define rdup_vnus_string(e) rdup_tmstring(e)
#define print_vnus_string(st,e) print_tmstring(st,e)
