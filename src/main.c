#include <stdio.h>

// buffer for user input
#define INPUT_SIZE 2048
static char input[INPUT_SIZE];

/*
 * Prints version and exit informations
 */
void print_header()
{
  puts("Lisp Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");
}

/*
 * Prints prompt
 */
void prompt()
{
  fputs("lispy> ", stdout);
}

/*
 * Reads data from user to input variable
 */
void read()
{
  fgets(input, INPUT_SIZE, stdin);
}

int main()
{
  // Print header
  print_header();

  // Main never ending loop
  while(1)
  {
    // Show prompt
    prompt();

    // Read input
    read();

    // Echo input back to user
    printf("%s", input);
  }

  return 0;
}
