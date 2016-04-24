#include <stdio.h>
#include <stdlib.h>
#include <signal.h> // for Ctrl+C handling

#include "mpc/mpc.h"


#define PROMPT "lisp>"

#ifdef _WIN32
/*
 * Windows only section
 */

#include <string.h>

// buffer for user input
#define _WIN32_READLINE_BUFFER_SIZE 2048
static char _win32_readline_buffer[_WIN32_READLINE_BUFFER_SIZE];

/*
 * Imitation of readline function from editline library.
 */
char* readline(char* prompt) {
  char* result = NULL;

  printf("%s", prompt);
  fgets(_win32_readline_buffer, _WIN32_READLINE_BUFFER_SIZE, stdin);

  result = malloc(strlen(_win32_readline_buffer) + 1);
  strcpy(result, _win32_readline_buffer);

  return result;
}

/*
 * Imitation of add_history function from editline library.
 */
#define add_history(foo)

#else
/*
 * For other platforms (linux, mac) use editline library.
 */
#include <editline/readline.h>
#include <editline/history.h>

#endif

/*
 * Polish Notation parser
 */
static mpc_parser_t* Number;
static mpc_parser_t* Operator;
static mpc_parser_t* Expr;
static mpc_parser_t* Lisp;

void create_parser()
{
  Number   = mpc_new("number");
  Operator = mpc_new("operator");
  Expr     = mpc_new("expr");
  Lisp     = mpc_new("lisp");

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                     \
        number   : /-?[0-9]+(!?\\.[0-9]+)?/ ;               \
        operator : '+' | '-' | '*' | '/' ;                  \
        expr     : <number> | '(' <operator> <expr>+ ')' ;  \
        lisp     : /^/ <operator> <expr>+ /$/ ;             \
      ",
      Number, Operator, Expr, Lisp);
}

void cleanup_parser()
{
  mpc_cleanup(4, Number, Operator, Expr, Lisp);
}

/*
 * Ctrl+C handling
 *
 * To handle this signal the signal.h include
 * is necessary as well as this function subscription
 * at the beginning of the program:
 *   signal(SIGINT, int_handler);
 */
void int_handler(int sig)
{
  // produce new line
  puts("");

  // pass signal
  signal(sig, SIG_IGN);

  // cleanup parser
  cleanup_parser();

  // exit the program
  exit(0);
}


/*
 * Result evaluation
 */
float evaluate_operand(float x, char operand, float y)
{
  switch (operand)
  {
    case '+': return x + y;
    case '-': return x - y;
    case '*': return x * y;
    case '/': return x / y;
  }

  return 0.0f;
}

float evaluate_parsed_input(mpc_ast_t* node)
{
  // if node is a number just parse and return it
  if (strstr(node->tag, "number"))
    return strtof(node->contents, NULL);

  // if node is not a number then list of child nodes
  // should contain operand at the second position
  char operand = node->children[1]->contents[0];

  // then the rest of the nodes will be expressions
  // get the first one
  float x = evaluate_parsed_input(node->children[2]);

  // and iterate through rest of them
  int i = 2;
  while(++i, strstr(node->children[i]->tag, "expr"))
    x = evaluate_operand(x, operand,
        evaluate_parsed_input(node->children[i]));

  return x;
}


/*
 * Prints version and exit informations
 */
void print_header()
{
  puts("Lisp Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");
}


int main()
{
  // variables for parsing result
  mpc_result_t parsing_result;
  mpc_ast_t* parsed_input;
  float result = 0.0f;

  // Ctrl+C handling
  signal(SIGINT, int_handler);

  // print header
  print_header();

  // create parser
  create_parser();

  // main never ending loop
  while(1)
  {
    // read input and save it in history
    char* input = readline(PROMPT);
    add_history(input);

    // if exit the close the prompt
    if (strcmp(input, "exit\n") == 0)
    {
      // cleanup parser
      cleanup_parser();

      // break from the main loop - exit
      break;
    }

    if (mpc_parse("<stdin>", input, Lisp, &parsing_result))
    {
      // parsing ok
      parsed_input = parsing_result.output;

      // print Abstract Syntax Tree
      /*
        mpc_ast_print(parsed_input);
        mpc_ast_delete(parsed_input);
      */

      // calculate result
      result = evaluate_parsed_input(parsed_input);

      // print result
      printf("%f\n", result);
    }
    else
    {
      // parsing error
      // print error
      mpc_err_print(parsing_result.error);
      mpc_err_delete(parsing_result.error);
    }

    // free memory stored for input
    free(input);
  }

  return 0;
}
