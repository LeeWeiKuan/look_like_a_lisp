#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct _object{
    enum {NUMB, ATOM, STRG, PRIM, CONS, CLOS, NIL} kind;
    union {
        double number;
        const char *atom;
        const char *string;
        struct _object (*fn)(struct _object, struct _object);
        struct {
            struct _object *car;
            struct _object *cdr;
        }cons;
        struct _object *closure;
    } value;
} Object;


#define N 1024

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
        printf("%s", x->value.atom);
    else if (x->kind == NUMB)
        printf("%f", x->value.number);
    else if (x->kind == CONS)
        print_list(x);
    else
        printf("%p", x);
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


const char s[] = "(+ (+ 2 2) (+ 1 1))";
int s_p = 0;
char buffer[40];
int buffer_p;


int lex_parse()
{
    buffer_p = 0;
    buffer[buffer_p] = '\0';
    // 第一个格子
    while(s[s_p] == ' ')s_p++;
    if (s[s_p] == '\0') return 0;
    if (s[s_p] == '(') 
    {
        buffer[0] = s[s_p];
        buffer[1] = '\0';
        s_p++;
        return 1;
    }
    if (s[s_p] == ')')
    {
        buffer[0] = s[s_p];
        buffer[1] = '\0';
        s_p++;
        return 1;
    }
    if (s[s_p] == '\'')
    {
        buffer[0] = s[s_p];
        buffer[1] = '\0';
        s_p++;
        return 1; 
    }
    while (buffer_p<39 && s[s_p]!=' ' && s[s_p]!='(' && s[s_p] != ')' && s[s_p] != '\'' )
    {
        buffer[buffer_p] = s[s_p];
        buffer[buffer_p+1] = '\0';
        buffer_p++;
        s_p++;
    }
    return 1;
}

int is_equal_atom(Object *x, Object *y)
{
    if (x->kind != y -> kind) return 0;
    if (x->kind != ATOM) return 0;
    return strcmp(x->value.atom, y->value.atom) == 0;
}


Object* read_all()
{
    Object *list = NULL;
    Object *head = NULL;
    while(s[s_p]!='\0')
    {
        if (lex_parse()) 
        {
            char *copy_str = malloc(strlen(buffer)+1);
            strcpy(copy_str, buffer);
            Object *new_obj = atom(copy_str);
            Object *new_cons = cons(new_obj, NIL_OBJECT);
            if (list == NULL){
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


Object* parse()
{
    
}

int main(int argc, char* argv[])
{
    NIL_OBJECT = make_nil();
    Object *p = read_all();
    print(p);
    printf("\n");
    return 0;
}