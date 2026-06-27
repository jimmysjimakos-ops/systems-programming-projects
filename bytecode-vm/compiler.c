#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "vm.c"
/*
parse_statement ->assignmnets , whiles , ifs, etc
    parse_comparison  → handles ==, <, >
        parse_expression → handles +, -
            parse_term      → handles *, /
                parse_factor  → numbers, identifiers

this is cuz the ast will be walked with dfs , meaning leafs are processed first going upwards
*/

typedef enum {
    TOK_NUM, TOK_IDENT, TOK_PLUS, TOK_MINUS,
    TOK_STAR, TOK_SLASH, TOK_EQ, TOK_EQEQ, TOK_GT, TOK_LT,
    TOK_PRINT, TOK_WHILE, TOK_IF, TOK_LBRACE, TOK_RBRACE,
    TOK_LPAREN, TOK_RPAREN, TOK_EOF,
} TokenType;

typedef enum {
    NODE_NUM, NODE_ADD, NODE_SUB, NODE_MUL, NODE_DIV, 
    NODE_GT, NODE_LT, NODE_EQ, NODE_EQEQ, NODE_ASSIGN, NODE_PRINT, 
    NODE_WHILE, NODE_IF, NODE_IDENT
} NodeType;

typedef struct {
    TokenType type;
    int value;        // for TOK_NUM
    char name[32];    // for TOK_IDENT
} Token;
//for creating the token array

typedef struct Node{
    NodeType type;
    char name[32];
    int value;
    struct Node *left;
    struct Node *right;
    struct Node **body;
    int body_count;
}Node;

char var_names[256][32];
int var_count = 0;



Token *lexer(char *source , int *token_count){
    char *current = source;
    char wordBuilder[32];
    char numBuilder[32];
    int wordBuilderPointer = 0;
    int numBuilderPointer = 0;
    Token *tokens = malloc(strlen(source) * sizeof(Token));
    int tokenPointer = 0;
    Token tokenToSend;


    while(*current != '\0'){
        if(isalpha(*current)){
            //consume every char
            wordBuilder[wordBuilderPointer] = *current;
            wordBuilderPointer++;
            current++;
            while(isalnum(*current)){
                wordBuilder[wordBuilderPointer] = *current;
                wordBuilderPointer++;
                current = current + 1;
            }
            wordBuilder[wordBuilderPointer] = '\0';
            //see what it is 
            if (strcmp(wordBuilder, "print") == 0) {
                tokenToSend.type = TOK_PRINT;
                tokens[tokenPointer] = tokenToSend;
                tokenPointer++;
            } else if (strcmp(wordBuilder, "while") == 0) {
                tokenToSend.type = TOK_WHILE;
                tokens[tokenPointer] = tokenToSend;
                tokenPointer++;
            } else if (strcmp(wordBuilder, "if") == 0) {
                tokenToSend.type = TOK_IF;
                tokens[tokenPointer] = tokenToSend;
                tokenPointer++;
            }else{
                tokenToSend.type = TOK_IDENT;
                strcpy(tokenToSend.name, wordBuilder);
                tokens[tokenPointer] = tokenToSend;
                tokenPointer++;
            }
            wordBuilderPointer = 0;
            
        }else if(isdigit(*current)){
            while(isdigit(*current)){ 
                numBuilder[numBuilderPointer] = *current;
                numBuilderPointer++;
                current += 1;
            }
            numBuilder[numBuilderPointer] = '\0';
            numBuilderPointer = 0;
            tokenToSend.type = TOK_NUM;
            tokenToSend.value = atoi(numBuilder);
            tokens[tokenPointer] = tokenToSend;
            tokenPointer++;
        }else if(isspace(*current)){
            current++;
        }else{
            switch (*current) {
                case '+': 
                    tokenToSend.type = TOK_PLUS; 
                    tokens[tokenPointer] = tokenToSend;
                    tokenPointer++;
                    break;
                case '-': 
                    tokenToSend.type = TOK_MINUS; 
                    tokens[tokenPointer] = tokenToSend;
                    tokenPointer++;
                    break;
                case '*': 
                    tokenToSend.type = TOK_STAR; 
                    tokens[tokenPointer] = tokenToSend;
                    tokenPointer++;
                    break;
                case '/': 
                    tokenToSend.type = TOK_SLASH; 
                    tokens[tokenPointer] = tokenToSend;
                    tokenPointer++;
                    break;
                case '=': 
                    if(*(current + 1) == '='){
                        tokenToSend.type = TOK_EQEQ;
                        current++;
                    }else{
                        tokenToSend.type = TOK_EQ; 
                    }
                    
                    tokens[tokenPointer] = tokenToSend;
                    tokenPointer++;
                    break;
                case '>': 
                    tokenToSend.type = TOK_GT; 
                    tokens[tokenPointer] = tokenToSend;
                    tokenPointer++;
                    break;
                case '<': 
                    tokenToSend.type = TOK_LT; 
                    tokens[tokenPointer] = tokenToSend;
                    tokenPointer++;
                    break;
                case '(': 
                    tokenToSend.type = TOK_LPAREN; 
                    tokens[tokenPointer] = tokenToSend;
                    tokenPointer++;
                    break;
                case ')': 
                    tokenToSend.type = TOK_RPAREN; 
                    tokens[tokenPointer] = tokenToSend;
                    tokenPointer++;
                    break;
                case '{': 
                    tokenToSend.type = TOK_LBRACE; 
                    tokens[tokenPointer] = tokenToSend;
                    tokenPointer++;
                    break;
                case '}': 
                    tokenToSend.type = TOK_RBRACE; 
                    tokens[tokenPointer] = tokenToSend;
                    tokenPointer++;
                    break;
                default:
                    fprintf(stderr, "unexpected character: '%c'\n", *current);
                    exit(1);
            }          
            current++;
        }
        
    }
    tokens[tokenPointer].type = TOK_EOF;
    //not needed right? tokenPointer++;
    *token_count = tokenPointer;

    return tokens;

}

void print_tokens(Token *tokens, int count) {
    for (int i = 0; i <= count; i++) {
        switch (tokens[i].type) {
            case TOK_NUM:    printf("NUM(%d) ", tokens[i].value); break;
            case TOK_IDENT:  printf("IDENT(%s) ", tokens[i].name); break;
            case TOK_PLUS:   printf("PLUS "); break;
            case TOK_MINUS:  printf("MINUS "); break;
            case TOK_STAR:   printf("STAR "); break;
            case TOK_SLASH:  printf("SLASH "); break;
            case TOK_EQ:     printf("EQ "); break;
            case TOK_GT:     printf("GT "); break;
            case TOK_LT:     printf("LT "); break;
            case TOK_PRINT:  printf("PRINT "); break;
            case TOK_WHILE:  printf("WHILE "); break;
            case TOK_IF:     printf("IF "); break;
            case TOK_LBRACE: printf("LBRACE "); break;
            case TOK_RBRACE: printf("RBRACE "); break;
            case TOK_LPAREN: printf("LPAREN "); break;
            case TOK_RPAREN: printf("RPAREN "); break;
            case TOK_EQEQ:  printf("EQEQ "); break;
            case TOK_EOF:    printf("EOF"); break;
        }
    }
    printf("\n");
}

Node *parse_comparison(Token *tokens, int *i);
Node *parse_expression(Token *tokens, int *i);
Node *parse_term(Token *tokens, int *i);
Node *parse_factor(Token *tokens, int *i);
Node *parse_statement(Token *tokens , int *i){

    
    if(tokens[*i].type == TOK_IDENT){
        if(tokens[*i+1].type == TOK_EQ){
            Node *newNode = malloc(sizeof(Node));
            Node *leftNode = malloc(sizeof(Node));
            leftNode->type = NODE_IDENT;
            strcpy(leftNode->name , tokens[*i].name);
            leftNode->left = NULL;
            leftNode->right = NULL;
            newNode->left = leftNode;
            newNode->type = NODE_ASSIGN;
            (*i)++; //consume ident
            (*i)++; //consume eq
            newNode->right =  parse_comparison(tokens , i);
            return newNode;
        }

    }else if(tokens[*i].type == TOK_PRINT){
        Node *newNode = malloc(sizeof(Node));
        newNode->type = NODE_PRINT;
        (*i)++; //consume print
        newNode->right = NULL;
        newNode->left =  parse_comparison(tokens , i);
        return newNode;

    }else if(tokens[*i].type == TOK_IF){
        Node *newNode = malloc(sizeof(Node));
        newNode->type = NODE_IF;
        (*i)++; //consume while
        newNode->left = parse_comparison(tokens , i); //parse condition
        (*i)++; //consume LBRACE
        int body_ptr = 0;
        newNode->body = malloc(128 * sizeof(Node*));
        while(tokens[*i].type != TOK_RBRACE){
            newNode->body[body_ptr] = parse_statement(tokens, i);
            body_ptr++; 
        }
        newNode->body_count = body_ptr;
        (*i)++; //consume RBRACE
        return newNode;
    
    }else if(tokens[*i].type == TOK_WHILE){ //WHILE AND IF CAN PROBABLY BE COMPRESSED , THIS IS BETTER FOR READABILITY
        Node *newNode = malloc(sizeof(Node));
        newNode->type = NODE_WHILE;
        (*i)++; //consume while
        newNode->left = parse_comparison(tokens , i); //parse condition
        (*i)++; //consume LBRACE
        int body_ptr = 0;
        newNode->body = malloc(128 * sizeof(Node*));
        while(tokens[*i].type != TOK_RBRACE){
            newNode->body[body_ptr] = parse_statement(tokens, i);
            body_ptr++; 
        }
        newNode->body_count = body_ptr;
        (*i)++; //consume RBRACE
        return newNode;

    }else{
        fprintf(stderr , "AAAAAAAAAAA");
        exit(1);
    }
}

Node *parse_comparison(Token *tokens , int *i){
    Node *left = parse_expression(tokens, i);
    while(tokens[*i].type == TOK_EQEQ || tokens[*i].type == TOK_GT || tokens[*i].type == TOK_LT){
        Node *newNodeComp = malloc(sizeof(Node));
        if(tokens[*i].type == TOK_EQEQ){
            newNodeComp->type = NODE_EQEQ;
            (*i)++; //consume comp
            newNodeComp->left = left;
            newNodeComp->right = parse_expression(tokens, i);
        }else if(tokens[*i].type == TOK_GT){
            newNodeComp->type = NODE_GT;
            (*i)++; //consume comp
            newNodeComp->left = left;
            newNodeComp->right = parse_expression(tokens, i);
        }else if(tokens[*i].type == TOK_LT){
            newNodeComp->type = NODE_LT;
            (*i)++; //consume comp
            newNodeComp->left = left;
            newNodeComp->right = parse_expression(tokens, i);
        }

        left = newNodeComp;
    }
    return left;
}

Node *parse_expression(Token *tokens, int *i){
    Node *left = parse_term(tokens, i);
    while(tokens[*i].type == TOK_PLUS || tokens[*i].type == TOK_MINUS){
        Node *newNodeComp = malloc(sizeof(Node));
        if(tokens[*i].type == TOK_PLUS){
            newNodeComp->type = NODE_ADD;
            (*i)++; //consume comp
            newNodeComp->left = left;
            newNodeComp->right = parse_term(tokens, i);
        }else if(tokens[*i].type == TOK_MINUS){
            newNodeComp->type = NODE_SUB;
            (*i)++; //consume comp
            newNodeComp->left = left;
            newNodeComp->right = parse_term(tokens, i);
        }

        left = newNodeComp;
    }
    return left;
}

Node *parse_term(Token *tokens, int *i){
    Node *left = parse_factor(tokens, i);
    while(tokens[*i].type == TOK_SLASH || tokens[*i].type == TOK_STAR){
        Node *newNodeComp = malloc(sizeof(Node));
        if(tokens[*i].type == TOK_SLASH){
            newNodeComp->type = NODE_DIV;
            (*i)++; //consume comp
            newNodeComp->left = left;
            newNodeComp->right = parse_expression(tokens, i);
        }else if(tokens[*i].type == TOK_STAR){
            newNodeComp->type = NODE_MUL;
            (*i)++; //consume comp
            newNodeComp->left = left;
            newNodeComp->right = parse_expression(tokens, i);
        }

        left = newNodeComp;
    }
    return left;
}

Node *parse_factor(Token *tokens, int *i){
    Node *leafNode = malloc(sizeof(Node)); 
    if(tokens[*i].type == TOK_IDENT){
        leafNode->type = NODE_IDENT;
        strcpy(leafNode->name , tokens[*i].name);
        leafNode->left = NULL;
        leafNode->left = NULL;
        (*i)++; //consume ident
    }else if(tokens[*i].type == TOK_NUM){
        leafNode->type = NODE_NUM;
        leafNode->value = tokens[*i].value;
        leafNode->left = NULL;
        leafNode->right = NULL;
        (*i)++; //consume NUM
    }else if(tokens[*i].type == TOK_LPAREN){
        (*i)++; //consume LPAREN
        Node *node = parse_comparison(tokens, i);
        (*i)++; //consume RPAREN
        return node;
    }

    return leafNode;
}

Node **parser(Token *tokens, int *statement_count){
    int capacity = 128;
    int count = 0;
    int i=0;
    Node **statements = malloc(capacity * sizeof(Node*));

    while (tokens[i].type != TOK_EOF) {
        if (count == capacity) {
            capacity *= 2;
            statements = realloc(statements, capacity * sizeof(Node*));
        }
        statements[count++] = parse_statement(tokens, &i);
    }
    *statement_count = count; //this will be same as lexers count , to use in the ast
    
    return statements;
}

void walk(Node *statement , uint8_t *byte_array, int *byte_array_count){
    if(statement == NULL){
        return;
    }

    if(statement->type == NODE_NUM){
        byte_array[(*byte_array_count)++] = OP_PUSH;
        byte_array[(*byte_array_count)++] = statement->value;
        
    }else if(statement->type == NODE_IDENT){
        int found = 0;
        byte_array[(*byte_array_count)++] = OP_LOAD;
        for(int i=0; i < 256; i++){
            if(strcmp(var_names[i] , statement->name) == 0){
                byte_array[(*byte_array_count)++] = i;
                found = 1;
                break;
            }
        }
        if(found == 0){
            //error
        }
    }else if(statement->type == NODE_ADD){
        walk(statement->left, byte_array, byte_array_count);
        walk(statement->right, byte_array, byte_array_count);
        byte_array[(*byte_array_count)++] = OP_ADD;
        
    }else if(statement->type == NODE_SUB){
        walk(statement->left, byte_array, byte_array_count);
        walk(statement->right, byte_array, byte_array_count);
        byte_array[(*byte_array_count)++] = OP_SUB;
        
    }else if(statement->type == NODE_MUL){
        walk(statement->left, byte_array, byte_array_count);
        walk(statement->right, byte_array, byte_array_count);
        byte_array[(*byte_array_count)++] = OP_MUL;
        
    }else if(statement->type == NODE_DIV){
        walk(statement->left, byte_array, byte_array_count);
        walk(statement->right, byte_array, byte_array_count);
        byte_array[(*byte_array_count)++] = OP_DIV;
        
    }else if(statement->type == NODE_EQEQ){
        walk(statement->left, byte_array, byte_array_count);
        walk(statement->right, byte_array, byte_array_count);
        byte_array[(*byte_array_count)++] = OP_EQ;
        
    }else if(statement->type == NODE_GT){
        walk(statement->left, byte_array, byte_array_count);
        walk(statement->right, byte_array, byte_array_count);
        byte_array[(*byte_array_count)++] = OP_GT;
        
    }else if(statement->type == NODE_LT){
        walk(statement->left, byte_array, byte_array_count);
        walk(statement->right, byte_array, byte_array_count);
        byte_array[(*byte_array_count)++] = OP_LT;
        
    }else if(statement->type == NODE_ASSIGN){
        walk(statement->right, byte_array, byte_array_count); //left node is just ident , need to calculate right node which is an expression
        int found = 0;
        byte_array[(*byte_array_count)++] = OP_STORE;
        for(int i=0; i < 256; i++){
            if(strcmp(var_names[i] , statement->left->name) == 0){
                byte_array[(*byte_array_count)++] = i;
                found = 1;
                break;
            }
        }
        if(found == 0){
            strcpy(var_names[var_count], statement->left->name);
            byte_array[(*byte_array_count)++] = var_count;
            var_count++;
        }
        
    }else if(statement->type == NODE_PRINT){
        walk(statement->left, byte_array, byte_array_count); //expression first
        byte_array[(*byte_array_count)++] = OP_PRINT;
        
    }else if(statement->type == NODE_IF){
        walk(statement->left, byte_array, byte_array_count);
        byte_array[(*byte_array_count)++] = OP_JMP_IF_FALSE;
        byte_array[(*byte_array_count)] = 0x00;//placeholder byte
        int placeholder_index = *byte_array_count;
        (*byte_array_count)++;
        for(int i=0; i < statement->body_count; i++){
            walk(statement->body[i], byte_array, byte_array_count);
        }
        byte_array[placeholder_index] = *byte_array_count; 
    }else if(statement->type == NODE_WHILE){
        int loopback = *byte_array_count;
        walk(statement->left, byte_array, byte_array_count);
        byte_array[(*byte_array_count)++] = OP_JMP_IF_FALSE;
        byte_array[(*byte_array_count)] = 0x00;//placeholder byte for loopback
        int placeholder_index = *byte_array_count;
        (*byte_array_count)++;
        for(int i=0; i < statement->body_count; i++){
            walk(statement->body[i], byte_array, byte_array_count);
        }
        byte_array[(*byte_array_count)++] = OP_JMP;
        byte_array[(*byte_array_count)++] = loopback; 
        byte_array[placeholder_index] = *byte_array_count; 
    }

    return;
}

uint8_t *generate_byte(Node **statements , int len){
    uint8_t *byte_array = malloc(4096 * sizeof(uint8_t));
    int byte_array_count = 0;
    for(int i=0; i < len; i++){
        walk(statements[i] , byte_array, &byte_array_count);
    }
    byte_array[byte_array_count] = OP_HALT;
    return byte_array;
}

int main(){
    Node **statements;
    int count;
    int statement_count = 0;
    Token *tokens = lexer("x = 3\nwhile x > 0 {\nprint x\nx = x - 1\n}", &count);    
    print_tokens(tokens, count);
    statements = parser(tokens, &statement_count);
    uint8_t *bytecode = generate_byte(statements, statement_count); //this goes to the vm
    free(tokens);
    // Expected: IDENT(x) EQ NUM(3) PLUS NUM(4) STAR NUM(2) EOF   
    VM vm;
    vm_init(&vm, bytecode);
    vm_run(&vm);
    return 0;
}