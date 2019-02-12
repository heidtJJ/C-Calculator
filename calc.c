#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "tree.h"

#define EOS 256 // Want to use this as a marker because it's not ASCII

int getNextToken();
void match();

tree_t* expr();
tree_t* term();
tree_t* factor();

/*
    Calculator Grammar: Language of simple arithmetic (* +) expressions
    Input -> sequence of ASCII characters (ex: "2+3*4")
    Output -> "14"

    Version 1: E -> E+E | E*E | (E) | NUM
               NUM -> [0-9]^+ == (0|1|2...|9)*(0|1|2...|9)     
        E is the only variable, "+*()NUM" are terminal symbols. This grammar is 
        ambiguous. Because consider the following string: NUM + NUM * NUM. This 
        string is in the language. 
            1) E -> E + E
            2) E + E * E
            4) NUM + E * E
            4) NUM + NUM * E
            4) NUM + NUM * NUM
        This is ambigious because there is more than one way we can 
        produce a parse tree. (+ should have lower precedence than *)
        The Fix: precedence and associativity.

    Version 2: Associativity fixed (to the left)
               E -> E+T | T       low precendence      
               T -> T*F | F       med precendence     
               F -> (E) | NUM     high precendence
            Variables = {E, T, F}
            Terminals = {+, -, (, ), NUM}
        Claim: Not ambiguous (removed non-determinism)!

    Idea: Recursive-Descent Parsing (Top-Down method)
        We have grammar 2.
            E -> E+T | T            
            T -> T*F | F            
            F -> (E) | NUM  
        Input-tape (read-only one-way): NUM+NUM*NUM$
    We will not know whether to choose E+T or T

    We need a third grammar. Method to apply:
        - Turn each variable into a function.
            E(){             
                either 
                    E()    <- Dangerous recursive call. Spin forever
                    see(+)
                    T()
                 OR
                    T()   
            }
            
            T(){
                either
                    T()   <- Dangerous recursive call. Spin forever
                    see(*)
                    F()
                OR
                    F()
            }

            F(){
                if token == (
                    see(()
                    E()
                    see())
                else if token == NUM
                    see(NUM)
                else
                    ERROR()
            }      

        Left Recursion Problem. 
            1) A -> Ab | c
            2) E -> E+T | T (Example of 1)

                    A
                  /   \
                A      b
               / \
              A   b
             / ...
        Fix:
            A -> bA'
            A'-> cA' | emptyString   (A' is the auxilary variable)

        E -> TE'
        E -> +TE' | emptyString

    Code:
        E(){            
            T()
            E'()
        }

        E'(){  <- this is tail recursive loop
            if(token == +)
                see(+)
                T()
                E'()
            else return
        }
     Parser has 3 functions. E T F

     Grammar to use:
        E -> T E'
        E' -> + T E' | Empty

*/

/* Channels connecting scanner and parser */
int curToken;
int curAttribute;

int main(){
    tree_t* value;
    /* Initialize the first token. */
    curToken = getNextToken();

    /* Call the parser (start symbol of the grammar). */
    value = expr();
    assert(curToken == EOS);

    /* Semantic evaluation */
    fprintf(stderr, "\nValue = %d\n", tree_eval(value));
    return 0;
}


/*
    E -> T E'
    E' -> + T E' | Empty
*/
tree_t* expr(){
    tree_t* value = term();
    while(curToken == '+' || curToken == '-'){
        if(curToken == '+'){
            match('+');
            value = mktree('+', value, term());
        }
        else{
            match('-');
            value = mktree('-', value, term());
        }
    }
    return value;
}

/*
    T -> F T'
    T' -> * F T' | / F T | Empty
*/
tree_t* term(){
    tree_t* value = factor();
    while(curToken == '*' || curToken == '/'){
        if(curToken == '*'){
            match('*');
            value = mktree('*', value, factor());
        }
        else{
            match('/');
            value = mktree('/', value, factor());
        }
    }
    return value;
}

/* 
    F -> ( E ) | - F | NUM 
*/
tree_t* factor(){
    tree_t* value;
    if(curToken == '('){
        match('(');
        value = expr();
        match(')');
    }
    else if(curToken == '-'){
        match('-');
        value = mktree('-', factor(), NULL);
    }
    else if(curToken == NUM){
        value = mktree(NUM, NULL, NULL);
        value->attribute = curAttribute;
        match(NUM);
    }
    else{
        fprintf(stderr, "Error in factor()\n");
        exit(1);
    }
    return value;
}

void match(int token){
    if(curToken == token){
        curToken = getNextToken();
    }
    else{
        fprintf(stderr, "Bad unexpected token: %d.\n", curToken);
        exit(1);
    }
}

/* Lexical Analyzer */
int getNextToken(){
    int c, value;
    while(1){
        switch(c = getchar()){
            case ' ': /* Ignore Whitespaces*/
            case '\t':
                continue;
            case '\n':
                fprintf(stderr, "[EOS]");
                return EOS;
            case '+':/* process additive operator */
                fprintf(stderr, "[ADDOP:%c]", c);
                return c;
            case '-':/* process subtractive operator */
                fprintf(stderr, "[SUBOP:%c]", c);
                return c;
            case '*':/* process multiplicative operator */
                fprintf(stderr, "[MULOP:%c]", c);
                return c;
            case '/':/* process division operator */
                fprintf(stderr, "[DIVOP:%c]", c);
                return c;
            case '(': case ')': /* Process Parenthesis */
                fprintf(stderr, "[%c]", c);
                return c;
            default:
                if(isdigit(c)){ /* Process a number -> ('0'|'1'|...'9') */
                    value = c - '0';
                    while(isdigit(c = getchar())){
                        value = 10*value + c - '0';
                    }
                    ungetc(c, stdin);

                    curAttribute = value;

                    fprintf(stderr, "[NUM: %d]", value);
                    return NUM;
                }
                else {
                    // This is an inalid symbol (letter, etc;).
                    fprintf(stderr, "{%c} ", c);
                    return c;
                }
        }
    }
}