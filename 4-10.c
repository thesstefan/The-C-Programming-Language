#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define PRINT 11
#define DUPLICATE 22
#define CLEAR 33
#define SWITCH 44

#define GET_RESULT '\n'

#define ERROR 1
#define ARITH 2
#define MATH 3
#define STACK 4

#define VARIABLE 5
#define GET_LAST_VARIABLE 'x'
#define CHANGE_VARIABLE 'c'

#define MAX_OPERATORS 100
#define NUMBER '0'
#define MAX_STACK_SIZE 100
#define BUFFER_SIZE 100

int variable;
int stack_index = 0;
double stack[MAX_STACK_SIZE];
char buffer[BUFFER_SIZE];
int buffer_index = 0;
int var_gets_value = 0;
int change_variable = 0;

// Function for pushing into stack
void push(double new_stack_number) {
    if (stack_index < MAX_STACK_SIZE)
        stack[stack_index++] = new_stack_number;
    else
        printf("Error : Stack full\n");
}

// Function for poping from stack
double pop() {
    if (stack_index > 0)
        return stack[--stack_index];
    else {
        printf("Error : Stack empty\n");
        return 0;
    }
}

// Function for getting input characters
int get_character() {
    if (buffer_index > 0)
        return buffer[--buffer_index];
    else
        return getchar();
}

// Function to "undo" get_character()
void unget_character(int character) {
    if (buffer_index >= BUFFER_SIZE)
        printf("Ungetch : Too many characters\n");
    else
        buffer[buffer_index++] = character;
}

// Function to get the digits of a number
void get_digits(char string[]) {
    int string_index = 0;

    do {
        string[++string_index] = get_character();
    } while (isdigit(string[string_index]) || string[string_index] == '.');

    if (string[string_index] != EOF)
        unget_character(string[string_index]);

    string[string_index] = '\0';
}

// Function to get the type of input : Number / Operators
int get_type(char input_string[]) {
    int character;
    static int last_character = 0;

    if (last_character == 0)
        character = get_character();
    else {
        character = last_character;
        last_character = 0;
    }

    change_variable = 0;
    // Ignore the spaces beetween characters
    while (isspace(character) && character != GET_RESULT)
        character = get_character();

    if (isalpha(character)) {
        int string_index = 0;

        while (isalpha(character)) {
            input_string[string_index++] = character;
            character = get_character();
        }

        input_string[string_index] = '\0';

        if (character != EOF)
            last_character = character;

        if (strlen(input_string) > 1) {
            return MATH;
        } else if ((isalpha(input_string[0]) && input_string[0] != CHANGE_VARIABLE) || input_string[0] == GET_LAST_VARIABLE)
            return VARIABLE;
        else if (input_string[0] == CHANGE_VARIABLE) {
            change_variable = 1;
            
            do {
                input_string[0] = get_character();
            } while (input_string[0] == ' ');

                if (isalpha(input_string[0])) {
                    return VARIABLE;
                } else 
                    printf("Invalid change of variable\n");
        } else 
            return STACK;
        
    }
    if (character == GET_RESULT)
        return GET_RESULT;

    if ((character != '-' && isdigit(character)) || (character == '-' && stack_index < 2)) {
        input_string[0] = character;
        get_digits(input_string);
        return NUMBER;
    }

    if (!isdigit(character) && !isalpha(character) && character != '.') {
        input_string[0] = character;
        return ARITH;
    }
    // If something goes wrong
    return ERROR;
}

double top() {
    if (stack_index > 0) {
        return stack[stack_index - 1];
    }
    else {
        printf("Error : Stack empty\n");
        return 0;
    }
}

// Default values of variables
double var[30];

void clear_stack() {
    stack_index = 0;
}

char last_variable = 0;

void get_variables(char variable) {
    if (isupper(variable)) {
        for (int index = 0; index < 30; index++) 
                if (index == (int)variable - 65) {
                        if (change_variable)
                                var[index] = top();
                        last_variable = variable;
                }
    }  else     
        switch (variable) {
                case GET_LAST_VARIABLE:
                        if (last_variable)
                                printf("The last variable : %c\n", last_variable);
                        else 
                                printf("There is not a variable\n");
                        break;
                case CHANGE_VARIABLE:
                         break;
                default:
                        printf("Not a variable : %c\n", variable);
    }
}

void do_math_stuff(char function[]) {
    if (strcmp(function, "sin") == 0)
        push(sin(pop()));
    else if (strcmp(function, "cos") == 0)
        push(cos(pop()));
    else if (strcmp(function, "exp") == 0)
        push(exp(pop()));
    else if (strcmp(function, "pow") == 0) {
        int power = pop();
        int base = pop();
        push(pow(base, power));
    } else
        printf("Error : Function unsupported : %s\n", function);
}

void do_arith_stuff(char operator) {
    double second_operand = pop();
    double first_operand = pop();
    switch (operator) {
        case '+':
            push(first_operand + second_operand);
            break;
        case '*':
            push(first_operand * second_operand);
            break;
        case '-':
            push(first_operand - second_operand);
            break;
        case '/':
            if (second_operand != 0)
                push(first_operand / second_operand);
            else
                printf("Error : 0 divisor\n");
            break;
        case '%':
            if (second_operand != 0)
                push((int)first_operand % (int)second_operand);
            else
                printf("Error : 0 divisor\n");
            break;
        }
}

void do_stack_stuff(char command) {
    double first_number;
    double second_number;

    switch (command) {
        case PRINT:
            printf("\t%g\n", top());
            break;
        case DUPLICATE:
            push(top());
            break;
        case SWITCH:
            first_number = pop();
            second_number = pop();
            push(first_number);
            push(second_number);
            break;
        case CLEAR:
            clear_stack();
            break;
    }
}

int main() {
    int type;

    do {
        char input_string[MAX_OPERATORS];

        type = get_type(input_string);
        switch (type) {
            // Push the number into the stack
            case NUMBER:
                push(atof(input_string));
                break;
            case VARIABLE:
                get_variables(input_string[0]);
                break;
            case MATH:
                do_math_stuff(input_string);
                break;
            case ARITH:
                do_arith_stuff(input_string[0]);
                break;
            case STACK:
                do_stack_stuff(input_string[0]);
                break;
            case GET_RESULT:
                printf("\t%g\n", pop());
                break;
            default:
                printf("Error : Unknown command %s\n", input_string);
                break;
        }
    } while (type != EOF);

    return 0;
}
