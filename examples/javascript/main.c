#include <string.h>
#include "shell.h"
#include "jerryscript.h"

int shell_script (int argc, char **argv)
{
    if (argc < 2) {
        puts("Usage: script <your script here>\n\nTry for instance: script print('Hello World';)");
        return -1;
    }
 int count = argc-1;
 jerry_char_t  script[2*SHELL_DEFAULT_BUFSIZE+1]; 
 strcpy((char *) script,argv[1]);
 while (count>1) {
     count--;
     strcat((char *)script," ");
     strcat((char *) script,argv[argc-count]);
   }
 size_t script_size = strlen ((char *) script); 
 printf ("Executing script: [%s]\n\n", script); 
 bool ret_value = jerry_run_simple (script, script_size, JERRY_INIT_EMPTY);

  return (ret_value ? 1 : 0);
}

const shell_command_t shell_commands[] = {
  { "script", "Shell scripting ", shell_script },
  { NULL, NULL, NULL }
};

int main (void)
{
  printf ("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
  printf ("This board features a(n) %s MCU.\n", RIOT_MCU);

  /* start the shell */
  char line_buf[2*SHELL_DEFAULT_BUFSIZE];
  shell_run (shell_commands, line_buf, 2*SHELL_DEFAULT_BUFSIZE);

  return 0;
}