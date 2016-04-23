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
 * Prints version and exit informations
 */
void print_header()
{
  puts("Lisp Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");
}


int main()
{
  // Ctrl+C handling
  signal(SIGINT, int_handler);

  // print header
  print_header();

  // create parser
  create_parser();

  // variable for parsing result
  mpc_result_t parsing_result;

  // main never ending loop
  while(1)
  {
    // read input and save it in history
    char* input = readline(PROMPT);
    add_history(input);

    // echo input back to user
    //printf("%s", input);

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
      // print AST
      mpc_ast_print(parsing_result.output);
      mpc_ast_delete(parsing_result.output);
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
