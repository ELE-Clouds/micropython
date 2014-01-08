// All Micro Python objects are at least this type
// It must be of pointer size

typedef machine_ptr_t mp_obj_t;
typedef machine_const_ptr_t mp_const_obj_t;

// Integers that fit in a pointer have this type
// (do we need to expose this in the public API?)

typedef machine_int_t mp_small_int_t;

// The machine floating-point type used for float and complex numbers

#if MICROPY_ENABLE_FLOAT
typedef machine_float_t mp_float_t;
#endif

// Anything that wants to be a Micro Python object must have
// mp_obj_base_t as its first member (except NULL and small ints)

struct _mp_obj_type_t;
struct _mp_obj_base_t {
    const struct _mp_obj_type_t *type;
};
typedef struct _mp_obj_base_t mp_obj_base_t;

// The NULL object is used to indicate the absence of an object
// It *cannot* be used when an mp_obj_t is expected, except where explicitly allowed

#define MP_OBJ_NULL ((mp_obj_t)NULL)

// These macros check for small int, qstr or object, and access small int and qstr values
//  - xxxx...xxx1: a small int, bits 1 and above are the value
//  - xxxx...xx10: a qstr, bits 2 and above are the value
//  - xxxx...xx00: a pointer to an mp_obj_base_t

#define MP_OBJ_IS_SMALL_INT(o) ((((mp_small_int_t)(o)) & 1) != 0)
#define MP_OBJ_IS_QSTR(o) ((((mp_small_int_t)(o)) & 3) == 2)
#define MP_OBJ_IS_OBJ(o) ((((mp_small_int_t)(o)) & 3) == 0)
#define MP_OBJ_IS_TYPE(o, t) (MP_OBJ_IS_OBJ(o) && (((mp_obj_base_t*)(o))->type == (t)))

#define MP_OBJ_SMALL_INT_VALUE(o) (((mp_small_int_t)(o)) >> 1)
#define MP_OBJ_NEW_SMALL_INT(small_int) ((mp_obj_t)(((small_int) << 1) | 1))

#define MP_OBJ_QSTR_VALUE(o) (((mp_small_int_t)(o)) >> 2)
#define MP_OBJ_NEW_QSTR(qstr) ((mp_obj_t)((((machine_uint_t)qstr) << 2) | 2))

// These macros are used to declare and define constant function objects
// You can put "static" in front of the definitions to make them local

#define MP_DECLARE_CONST_FUN_OBJ(obj_name) extern const mp_obj_fun_native_t obj_name

#define MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, is_kw, n_args_min, n_args_max, fun_name) const mp_obj_fun_native_t obj_name = {{&fun_native_type}, is_kw, n_args_min, n_args_max, (void *)fun_name}
#define MP_DEFINE_CONST_FUN_OBJ_0(obj_name, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, 0, 0, (mp_fun_0_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_1(obj_name, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, 1, 1, (mp_fun_1_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_2(obj_name, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, 2, 2, (mp_fun_2_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_3(obj_name, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, 3, 3, (mp_fun_3_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_VAR(obj_name, n_args_min, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, n_args_min, (~((machine_uint_t)0)), (mp_fun_var_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(obj_name, n_args_min, n_args_max, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, n_args_min, n_args_max, (mp_fun_var_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_KW(obj_name, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, true, 0, (~((machine_uint_t)0)), (mp_fun_kw_t)fun_name)

// Need to declare this here so we are not dependent on map.h
struct _mp_map_t;

// Type definitions for methods

typedef mp_obj_t (*mp_fun_0_t)(void);
typedef mp_obj_t (*mp_fun_1_t)(mp_obj_t);
typedef mp_obj_t (*mp_fun_2_t)(mp_obj_t, mp_obj_t);
typedef mp_obj_t (*mp_fun_3_t)(mp_obj_t, mp_obj_t, mp_obj_t);
typedef mp_obj_t (*mp_fun_t)(void);
typedef mp_obj_t (*mp_fun_var_t)(int n, const mp_obj_t *);
typedef mp_obj_t (*mp_fun_kw_t)(mp_obj_t, struct _mp_map_t*);

typedef void (*mp_print_fun_t)(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o);
typedef mp_obj_t (*mp_make_new_fun_t)(mp_obj_t type_in, int n_args, const mp_obj_t *args); // args are in reverse order in the array
typedef mp_obj_t (*mp_call_n_fun_t)(mp_obj_t fun, int n_args, const mp_obj_t *args); // args are in reverse order in the array
typedef mp_obj_t (*mp_call_n_kw_fun_t)(mp_obj_t fun, int n_args, int n_kw, const mp_obj_t *args); // args are in reverse order in the array
typedef mp_obj_t (*mp_unary_op_fun_t)(int op, mp_obj_t);
typedef mp_obj_t (*mp_binary_op_fun_t)(int op, mp_obj_t, mp_obj_t);

typedef struct _mp_method_t {
    const char *name;
    mp_const_obj_t fun;
} mp_method_t;

// Buffer protocol
typedef struct _buffer_info_t {
    // if we'd bother to support various versions of structure
    // (with different number of fields), we can distinguish
    // them with ver = sizeof(struct). Cons: overkill for *micro*?
    //int ver; // ?

    void *buf;
    machine_int_t len;

    // Rationale: have array.array and have SIMD operations on them
    // Cons: users can pass item size to processing functions themselves,
    // though that's not "plug&play"
    // int itemsize;

    // Rationale: to load arbitrary-sized sprites directly to LCD
    // Cons: a bit adhoc usecase
    // int stride;
} buffer_info_t;
#define BUFFER_READ  (1)
#define BUFFER_WRITE (2)
#define BUFFER_RW (BUFFER_READ | BUFFER_WRITE)
typedef struct _mp_buffer_p_t {
    machine_int_t (*get_buffer)(mp_obj_t obj, buffer_info_t *bufinfo, int flags);
} mp_buffer_p_t;

// Stream protocol
typedef struct _mp_stream_p_t {
    // On error, functions should return -1 and fill in *errcode (values are
    // implementation-dependent, but will be exposed to user, e.g. via exception).
    machine_int_t (*read)(mp_obj_t obj, void *buf, machine_uint_t size, int *errcode);
    machine_int_t (*write)(mp_obj_t obj, const void *buf, machine_uint_t size, int *errcode);
    // add seek() ?
} mp_stream_p_t;

struct _mp_obj_type_t {
    mp_obj_base_t base;
    const char *name;
    mp_print_fun_t print;
    mp_make_new_fun_t make_new;     // to make an instance of the type

    mp_call_n_fun_t call_n;
    mp_call_n_kw_fun_t call_n_kw;
    mp_unary_op_fun_t unary_op;     // can return NULL if op not supported
    mp_binary_op_fun_t binary_op;   // can return NULL if op not supported

    mp_fun_1_t getiter;
    mp_fun_1_t iternext;

    // Alternatively, pointer(s) to interfaces to save space
    // in mp_obj_type_t at the expense of extra pointer and extra dereference
    // when actually used.
    mp_buffer_p_t buffer_p;
    mp_stream_p_t stream_p;

    const mp_method_t *methods;

    /*
    What we might need to add here:

    dynamic_type    instance

    compare_op
    load_attr       module instance class list
    load_method     instance str gen list user
    store_attr      module instance class
    store_subscr    list dict

    len             str tuple list map
    abs             float complex
    hash            bool int none str
    equal           int str
    less            int
    get_array_n     tuple list

    unpack seq      list tuple
    __next__        gen-instance
    */
};

typedef struct _mp_obj_type_t mp_obj_type_t;

// Constant objects, globally accessible

extern const mp_obj_type_t mp_const_type;
extern const mp_obj_t mp_const_none;
extern const mp_obj_t mp_const_false;
extern const mp_obj_t mp_const_true;
extern const mp_obj_t mp_const_empty_tuple;
extern const mp_obj_t mp_const_ellipsis;
extern const mp_obj_t mp_const_stop_iteration; // special object indicating end of iteration (not StopIteration exception!)

// General API for objects

mp_obj_t mp_obj_new_none(void);
mp_obj_t mp_obj_new_bool(bool value);
mp_obj_t mp_obj_new_cell(mp_obj_t obj);
mp_obj_t mp_obj_new_int(machine_int_t value);
mp_obj_t mp_obj_new_str(qstr qstr);
#if MICROPY_ENABLE_FLOAT
mp_obj_t mp_obj_new_float(mp_float_t val);
mp_obj_t mp_obj_new_complex(mp_float_t real, mp_float_t imag);
#endif
mp_obj_t mp_obj_new_exception(qstr id);
mp_obj_t mp_obj_new_exception_msg(qstr id, const char *msg);
mp_obj_t mp_obj_new_exception_msg_1_arg(qstr id, const char *fmt, const char *a1);
mp_obj_t mp_obj_new_exception_msg_2_args(qstr id, const char *fmt, const char *a1, const char *a2);
mp_obj_t mp_obj_new_exception_msg_varg(qstr id, const char *fmt, ...); // counts args by number of % symbols in fmt, excluding %%; can only handle void* sizes (ie no float/double!)
mp_obj_t mp_obj_new_range(int start, int stop, int step);
mp_obj_t mp_obj_new_range_iterator(int cur, int stop, int step);
mp_obj_t mp_obj_new_fun_bc(int n_args, uint n_state, const byte *code);
mp_obj_t mp_obj_new_fun_asm(uint n_args, void *fun);
mp_obj_t mp_obj_new_gen_wrap(uint n_locals, uint n_stack, mp_obj_t fun);
mp_obj_t mp_obj_new_gen_instance(const byte *bytecode, uint n_state, int n_args, const mp_obj_t *args);
mp_obj_t mp_obj_new_closure(mp_obj_t fun, mp_obj_t closure_tuple);
mp_obj_t mp_obj_new_tuple(uint n, const mp_obj_t *items);
mp_obj_t mp_obj_new_tuple_reverse(uint n, const mp_obj_t *items);
mp_obj_t mp_obj_new_list(uint n, mp_obj_t *items);
mp_obj_t mp_obj_new_list_reverse(uint n, mp_obj_t *items);
mp_obj_t mp_obj_new_dict(int n_args);
mp_obj_t mp_obj_new_set(int n_args, mp_obj_t *items);
mp_obj_t mp_obj_new_slice(mp_obj_t start, mp_obj_t stop, mp_obj_t step);
mp_obj_t mp_obj_new_bound_meth(mp_obj_t self, mp_obj_t meth);
mp_obj_t mp_obj_new_class(struct _mp_map_t *class_locals);
mp_obj_t mp_obj_new_instance(mp_obj_t clas);
mp_obj_t mp_obj_new_module(qstr module_name);

mp_obj_t mp_obj_get_type(mp_obj_t o_in);
const char *mp_obj_get_type_str(mp_obj_t o_in);

void mp_obj_print_helper(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in);
void mp_obj_print(mp_obj_t o);

bool mp_obj_is_callable(mp_obj_t o_in);
machine_int_t mp_obj_hash(mp_obj_t o_in);
bool mp_obj_equal(mp_obj_t o1, mp_obj_t o2);
bool mp_obj_less(mp_obj_t o1, mp_obj_t o2);

machine_int_t mp_obj_get_int(mp_obj_t arg);
#if MICROPY_ENABLE_FLOAT
mp_float_t mp_obj_get_float(mp_obj_t self_in);
void mp_obj_get_complex(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag);
#endif
qstr mp_obj_get_qstr(mp_obj_t arg);
mp_obj_t *mp_obj_get_array_fixed_n(mp_obj_t o, machine_int_t n);
uint mp_get_index(const mp_obj_type_t *type, machine_uint_t len, mp_obj_t index);

// none
extern const mp_obj_type_t none_type;

// bool
extern const mp_obj_type_t bool_type;

// cell
mp_obj_t mp_obj_cell_get(mp_obj_t self_in);
void mp_obj_cell_set(mp_obj_t self_in, mp_obj_t obj);

// int
extern const mp_obj_type_t int_type;

// exception
extern const mp_obj_type_t exception_type;
qstr mp_obj_exception_get_type(mp_obj_t self_in);

// str
extern const mp_obj_type_t str_type;
qstr mp_obj_str_get(mp_obj_t self_in);

#if MICROPY_ENABLE_FLOAT
// float
extern const mp_obj_type_t float_type;
mp_float_t mp_obj_float_get(mp_obj_t self_in);
mp_obj_t mp_obj_float_binary_op(int op, mp_float_t lhs_val, mp_obj_t rhs);

// complex
extern const mp_obj_type_t complex_type;
void mp_obj_complex_get(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag);
mp_obj_t mp_obj_complex_binary_op(int op, mp_float_t lhs_real, mp_float_t lhs_imag, mp_obj_t rhs_in);
#endif

// tuple
extern const mp_obj_type_t tuple_type;
void mp_obj_tuple_get(mp_obj_t self_in, uint *len, mp_obj_t **items);

// list
extern const mp_obj_type_t list_type;
mp_obj_t mp_obj_list_append(mp_obj_t self_in, mp_obj_t arg);
void mp_obj_list_get(mp_obj_t self_in, uint *len, mp_obj_t **items);
void mp_obj_list_store(mp_obj_t self_in, mp_obj_t index, mp_obj_t value);

// dict
extern const mp_obj_type_t dict_type;
uint mp_obj_dict_len(mp_obj_t self_in);
mp_obj_t mp_obj_dict_store(mp_obj_t self_in, mp_obj_t key, mp_obj_t value);

// set
extern const mp_obj_type_t set_type;
void mp_obj_set_store(mp_obj_t self_in, mp_obj_t item);

// slice
extern const mp_obj_type_t slice_type;
void mp_obj_slice_get(mp_obj_t self_in, machine_int_t *start, machine_int_t *stop, machine_int_t *step);

// functions
typedef struct _mp_obj_fun_native_t { // need this so we can define const objects (to go in ROM)
    mp_obj_base_t base;
    bool is_kw : 1;
    machine_uint_t n_args_min : (sizeof(machine_uint_t) - 1); // inclusive
    machine_uint_t n_args_max; // inclusive
    void *fun;
    // TODO add mp_map_t *globals
    // for const function objects, make an empty, const map
    // such functions won't be able to access the global scope, but that's probably okay
} mp_obj_fun_native_t;

extern const mp_obj_type_t fun_native_type;
extern const mp_obj_type_t fun_bc_type;
void mp_obj_fun_bc_get(mp_obj_t self_in, int *n_args, uint *n_state, const byte **code);

// generator
extern const mp_obj_type_t gen_instance_type;

// class
extern const mp_obj_type_t class_type;
extern const mp_obj_t gen_instance_next_obj;
struct _mp_map_t *mp_obj_class_get_locals(mp_obj_t self_in);

// instance
extern const mp_obj_type_t instance_type;
mp_obj_t mp_obj_instance_load_attr(mp_obj_t self_in, qstr attr);
void mp_obj_instance_load_method(mp_obj_t self_in, qstr attr, mp_obj_t *dest);
void mp_obj_instance_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value);

// module
extern const mp_obj_type_t module_type;
mp_obj_t mp_obj_new_module(qstr module_name);
struct _mp_map_t *mp_obj_module_get_globals(mp_obj_t self_in);
