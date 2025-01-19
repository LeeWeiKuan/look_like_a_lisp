#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_LINE_LENGTH 1024
#define N 1024

typedef struct _object{
    enum {NUMB, ATOM, STRG, PRIM, CONS, CLOS, C_FUNCTION, NIL} kind;
    union {
        double number;
        const char *atom;
        const char *string;
        struct _object* (*fn)(struct _object*, struct _object*);
        struct {
            struct _object *car;
            struct _object *cdr;
        }cons;
        struct {
            struct _object *args;
            struct _object *body;
            struct _object *env;
        }clos;
    } value;
} Object;


// region global value
Object cell[N];
int base = 0;
int top = 0;
Object* NIL_OBJECT;
// endregion


Object* new_object(){
    Object* p = &cell[top];
    top++;
    return p;
}

Object* make_nil(){
    Object* x = new_object();
    x->kind = NIL;
    return x;
}

Object* atom(char* str){
    Object* x = new_object();
    x->kind = ATOM;
    x->value.atom = str;
    return x;
}

Object* cons(Object* p1, Object* p2){
    Object* x = new_object();
    x->kind = CONS;
    x->value.cons.car = p1;
    x->value.cons.cdr = p2;
    return x;
}

Object* car(Object *x){
    assert(x->kind == CONS);
    return x->value.cons.car;
}

Object* cdr(Object *x){
    assert(x->kind == CONS);
    return x->value.cons.cdr;
}

// Object* quote(Object *x){

// }


void print_list();
void print(Object* x){
    if (x->kind == NIL) 
        printf("()");
    else if (x->kind == ATOM)
    {
        printf("%s", x->value.atom);
    }
    else if (x->kind == NUMB)
        printf("%f", x->value.number);
    else if (x->kind == CONS)
        print_list(x);
    else if (x->kind == CLOS)
        printf("closure\n");
    else
        printf("unknown value\n");
}

void print_list(Object* x){
  for (putchar('['); ; putchar(' ')) {
    print(car(x));
    x = cdr(x);
    if (x->kind == NIL)
      break;
    if (x->kind != CONS) {
      printf(" . ");
      print(x);
      break;
    }
  }
  putchar(']');
}


// step 1
const char *g_input_str;
int g_input_str_p = 0;
char buffer[40];
int buffer_p;


int lex_parse()
{
    buffer_p = 0;
    buffer[buffer_p] = '\0';
    while(g_input_str[g_input_str_p] == ' ')g_input_str_p++;
    if (g_input_str[g_input_str_p] == '\0') return 0;
    if (g_input_str[g_input_str_p] == '(') 
    {
        buffer[0] = g_input_str[g_input_str_p];
        buffer[1] = '\0';
        g_input_str_p++;
        return 1;
    }
    if (g_input_str[g_input_str_p] == ')')
    {
        buffer[0] = g_input_str[g_input_str_p];
        buffer[1] = '\0';
        g_input_str_p++;
        return 1;
    }
    if (g_input_str[g_input_str_p] == '\'')
    {
        buffer[0] = g_input_str[g_input_str_p];
        buffer[1] = '\0';
        g_input_str_p++;
        return 1; 
    }
    while (buffer_p<39 && g_input_str[g_input_str_p]!='\0' && g_input_str[g_input_str_p]!=' ' && g_input_str[g_input_str_p]!='(' && g_input_str[g_input_str_p] != ')' && g_input_str[g_input_str_p] != '\'' )
    {
        buffer[buffer_p] = g_input_str[g_input_str_p];
        buffer[buffer_p+1] = '\0';
        buffer_p++;
        g_input_str_p++;
    }
    return 1;
}

int is_equal_atom(Object *x, Object *y)
{
    if (x->kind != y -> kind) return 0;
    if (x->kind != ATOM) return 0;
    return strcmp(x->value.atom, y->value.atom) == 0;
}


Object* read_all(const char *input_str)
{
    Object *list = NIL_OBJECT;
    Object *head = NIL_OBJECT;
    g_input_str = input_str;
    g_input_str_p = 0;
    while(g_input_str[g_input_str_p]!='\0')
    {
        if (lex_parse()) 
        {
            char *copy_str = malloc(strlen(buffer)+1);
            strcpy(copy_str, buffer);
            Object *new_obj = atom(copy_str);
            Object *new_cons = cons(new_obj, NIL_OBJECT);
            if (list == NIL_OBJECT){
                list = new_cons;
                head = list;
            }
            else
            {
                list->value.cons.cdr = new_cons;
                list = list->value.cons.cdr;
            }
        }
    }
    return head;
}

// step 2
Object* tokens;
Object* get_token()
{
    return car(tokens);
}

void eat_token()
{
    tokens = cdr(tokens);
}


Object* parse();
Object* parse_list()
{
    eat_token();
    Object *list = NIL_OBJECT;
    Object *p = NIL_OBJECT;
    while (!is_equal_atom(get_token(), atom(")")))
    {
        Object *new_cons = cons(parse(), NIL_OBJECT);
        if (list == NIL_OBJECT)
        {
            p = new_cons;
            list = p;
        }
        else
        {
            p->value.cons.cdr = new_cons;
            p = p->value.cons.cdr;
        }
    }
    eat_token();
    return list;
}

Object* parse()
{
    if (is_equal_atom(get_token(), atom("(")))
        return parse_list();
    else
    {
        Object *r = get_token();
        eat_token();
        return r;
    }
}

Object *g_env;
// step 3
// eval
Object* eval(Object*, Object*);
Object* call_closure(Object *clo, Object* s_args);
Object* eval_list(Object *s_list, Object *env)
{
    Object *result = NIL_OBJECT;
    Object* fn = eval(car(s_list), env);
    // call fn
    if (fn->kind == CLOS)
    {
        result = call_closure(fn, cdr(s_list));
    }
    else if (fn->kind == C_FUNCTION)
    {
        result = fn->value.fn(cdr(s_list), env);
    }
    return result;
}


Object* get_value(Object *name, Object *env);

Object* eval_prim(Object *s, Object *env)
{
    double number;
    int result;
    Object *o = NIL_OBJECT;
    // parse number;
    if (sscanf(s->value.atom, "%lf", &number)==1)
    {
        o = new_object();
        o->kind = NUMB;
        o->value.number = number;
        return o;
    }
    // get env value
    return get_value(s, env);
}


Object* eval(Object *s, Object *env)
{
    switch (s->kind)
    {
    case CONS:
        return eval_list(s, env);
    case ATOM:
        return eval_prim(s, env);
    default:
        return NIL_OBJECT;
    }
}


void set_value(Object *name, Object *value, Object *env)
{
    Object *p = env;
    // 看一下能不能找到已有的name
    int is_found = 0;
    for(p = env; cdr(p) != NIL_OBJECT; p = cdr(p))
    {
        Object *item = car(cdr(p));
        if (strcmp(name->value.atom, car(item)->value.atom) == 0)
        {
            item->value.cons.cdr = value;
            is_found = 1;
            break;
        }
    }
    if (!is_found)
    {
        Object *new_item = cons(name, value);
        p->value.cons.cdr = cons(new_item, NIL_OBJECT);
    }
}


Object* get_value(Object *name, Object *env)
{
    // env 
    Object *p;
    Object *o = NIL_OBJECT;
    for(p = cdr(env); p != NIL_OBJECT; p = cdr(p))
    {
        Object *item = car(p);
        if (strcmp(name->value.atom, car(item)->value.atom) == 0)
        {
            o = cdr(item);
            break;
        }
    }
    return o;
}


Object* call_closure(Object *clo, Object* s_args)
{
    Object *local_env = cons(NIL_OBJECT, NIL_OBJECT);
    Object *p_clo_args = clo->value.clos.args;
    Object *p_s_args = s_args;
    Object *p_local = local_env;
    Object *env = clo->value.clos.env;
    while(p_clo_args != NIL_OBJECT && p_s_args != NIL_OBJECT)
    {
        Object *local_name = car(p_clo_args);
        Object *local_value = eval(car(p_s_args), env);
        set_value(local_name, local_value, local_env);
        p_clo_args = cdr(p_clo_args);
        p_s_args = cdr(p_s_args);
    }
    // cons local_env and upper env;
    while(cdr(p_local)!=NIL_OBJECT)
    {
        p_local = cdr(p_local);
    }
    p_local->value.cons.cdr = cdr(env);
    // eval
    Object *result;
    result = eval(clo->value.clos.body, local_env);
    return result;
}


Object* define(Object *s_args, Object *env)
{
    Object *name = car(s_args);
    Object *val = eval(car(cdr(s_args)), env);
    set_value(name, val, g_env);
    return val;
}

Object* add(Object *s_args, Object *env)
{
    Object *p = s_args;
    double result = 0;
    for(p= s_args; p != NIL_OBJECT; p = cdr(p))
    {
        Object *value = eval(car(p), env);
        result += value->value.number;
    }
    Object *result_obj = new_object();
    result_obj->kind = NUMB;
    result_obj->value.number = result;
    return result_obj;
}

Object* lambda(Object *s_args, Object *env)
{
    Object *func_obj = new_object();
    func_obj->kind = CLOS;
    func_obj->value.clos.args = car(s_args);
    func_obj->value.clos.body = car(cdr(s_args));
    func_obj->value.clos.env = env;
    return func_obj;
}


void register_c_function(const char *name, Object* (*fn)(Object*, Object*))
{
    Object *f_obj = new_object();
    Object *name_obj = atom((char*)name);
    f_obj->kind = C_FUNCTION;
    f_obj->value.fn = fn;
    set_value(name_obj, f_obj, g_env);
}


int main(int argc, char* argv[])
{
    // init
    NIL_OBJECT = make_nil();
    g_env = cons(NIL_OBJECT, NIL_OBJECT);
    register_c_function("define", define);
    register_c_function("+", add);
    register_c_function("lambda", lambda);
    // step1 read
    FILE *file = fopen("main.lisp", "r");
    if (!file) {
        perror("无法打开文件");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        printf("-------------start------------\n");
        Object *p = read_all(line);
        print(p);
        printf("\n");
        printf("parse tree\n");
        // step2 parse
        tokens = p;
        Object *tree = parse();
        print(tree);
        printf("\n");
        printf("done!\n");
        // step3 eval
        print(eval(tree, g_env));
        printf("\n-------------end------------\n");
    }
    return 0;
}


