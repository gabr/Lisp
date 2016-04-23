#include <stdio.h>
#include <stdlib.h>

#include "mpc/mpc.h"


#define PROMPT "perl>"

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

  fputs(prompt, stdout);
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
 * Prints version and exit informations
 */
void print_header()
{
  puts("Lisp Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");
}


int main()
{
  // print header
  print_header();

  // main never ending loop
  while(1)
  {
    // read input and save it in history
    char* input = readline(PROMPT);
    add_history(input);

    // echo input back to user
    printf("%s", input);

    free(input);
  }

  return 0;
}
