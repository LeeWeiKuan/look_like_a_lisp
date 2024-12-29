#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    assert(x->kind != CONS);
    return x->value.cons.car;
}

Object* cdr(Object *x){
    assert(x->kind != CONS);
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
  for (putchar('('); ; putchar(' ')) {
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
  putchar(')');
}

Object Read() {
    int c = getchar();
    if (c == EOF) 
        exit(0);
}


const char s[] = "a---- b c (+ 1 1))";
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


void read_all()
{
    printf("hello\n");
    while(s[s_p]!='\0')
    {
        if (lex_parse()) 
        {
            // if(buffer[0] == '(')
            // printf(buffer);
            // printf("\n");
        }
    }
    // printf("%d\n", s_p);
}

int main(int argc, char* argv[])
{
    NIL_OBJECT = make_nil();
    Object* o1 = atom("x");
    Object* o2 = atom("y");
    Object* o3 = atom("z");
    Object* p1 = cons((cons(o1, o2)), cons(o2, cons(o3, NIL_OBJECT)));
    print(p1);
    return 0;
}