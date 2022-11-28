// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
void printPids(int *pidarr, int index);
void printHistory(char *commandList[15], int index);
#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line
#define MAX_COMMAND_SIZE 255    // The maximum command-line size
#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports four arguments
int main()
{
    char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
    int isProcess;
    pid_t process;
    int tempPid = 0;
    int status = 0;
    int *pidarr;
    char *commandList[15] = {};
    int i = 0;
    int index = 0;
    int counter = 0;
    int z = 0;
    int index1 = 0;
  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");
    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );
    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];
    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_string  = strdup( command_string );                
    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;
    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }
    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality
    if(token[0] != NULL) {
        if(token[0][0] == '!') {
            if(token[0][2] == '\0') {
                token[0] = commandList[(int)token[0][1] - 48];
            }
            else if(token[0][3] == '\0') {
                token[0] = commandList[(int)token[0][2] + 10 - 48];
            }
        }
        if(!strcmp(token[0], "exit") || !strcmp(token[0], "quit"))
            exit(0);
        commandList[index1] = token[0];
            index1++;
        if(index1 == 15)
            index1 = 0;
        process = fork();
        if(process == 0) {
            isProcess = execvp(token[0], token);
            if(!strcmp(token[0], "cd")) {
                chdir(token[1]);
                isProcess = 0;
            }
            else if(!strcmp(token[0], "listpids") || (!strcmp(token[0], "history"))) {
                isProcess = 0;
            }
            else if(isProcess == -1) {
                printf("Not a command\n");
            }
            exit(0);
        }
        waitpid(process, &status, 0);
        pidarr = (int*)realloc(pidarr, (index + 1) * sizeof(int));
        pidarr[index] = process;
        index++;
        if(!strcmp(token[0], "listpids")) {
                printPids(pidarr, index);
                isProcess = 0;
        }
        else if(!strcmp(token[0], "history")) {
            printHistory(commandList, index);
            isProcess = 0;
        }   
    }
  }
  free(pidarr);
  return 0;
  // e2520ca2-76f3-90d6-0242ac120003
}
void printPids(int *pidarr, int index) {
    int start = index - 20;
    int counter = 0;
    if(start < 0)
        start = 0;
    for(int i = start; i < index; i++) {
        printf("%3d %d\n", counter, pidarr[i]);
        counter++;
    }
}
void printHistory(char *commandList[15], int index) {
    int start = index - 15;
    int counter = 0;
    if(start < 0)
        start = 0;
    for(int i = 0; i < 15; i++) {
        printf("%3d %s\n", i, commandList[i]);
        counter++;
    }
}