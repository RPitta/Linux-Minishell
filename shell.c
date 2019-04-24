#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

char *remove_quotes(char *s)
{
     char *new_arg = (char *) malloc(strlen(s));
     char c;
     int i = 0;

     while ((c = *s++) != '\0')
     {
          if (c != '"')
          {
               new_arg[i] = c;
               i++;
          }          
     }

     new_arg[i] = '\0';
     return new_arg;
}

/*  ---------------------------------------------------------  */
/*  This function replaces all white spaces with zeros         */
/*  until it sees a non-white space character which indicates  */
/*  the beginning of an argument. It then skips over all       */
/*  non-white space characters which constitute the argument.  */
/*  ---------------------------------------------------------  */
int parse(char *line, char **argv)
{
     bool no_args = true;

     if (*line == 0)
          return -1;

     while (*line != '\0') 
     {       
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     
          if (*line != ' ' && *line && '\t' && *line != '\n')
          {
               *argv++ = line;          
               no_args = false;
          }
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n')
               line++;             
     }
     *argv = '\0';                 
     
     if (no_args)  
          return -1;
}

/*  -----------------------------------------------------------------  */
/*  This function receives command line arguments and forks a          */
/*  child process to execute the commands using system call execvp().  */
/*  -----------------------------------------------------------------  */
void execute(char **argv)
{
     pid_t  pid;
     int    status;
     char *c = "";
     char *filename;
     bool bg = false;
     bool redirect = false;

     if (argv[1] != NULL)
          if (strcmp(argv[1], c) == 0) argv[1] = NULL;

     for (int i = 0; i < 700; i++)
     {
          if (argv[i] == NULL)
          {    
               if (strchr(argv[i-1], '&'))
               {
               		// Background process
                    argv[i-1][strlen(argv[i-1])-1] = 0;     
                    bg = true;
                    break;
               }
               else
                    break;
          }
          else if (strcmp(argv[i], ">") == 0)
          {
               redirect = true;

               if (argv[i+1][strlen(argv[i+1])-1] == '&')
               {    
               		// Background process
                    bg = true;
                    argv[i+1][strlen(argv[i+1])-1] = 0;
               }

               filename = argv[i+1];
               argv[i] = NULL;
               argv[i+1] = NULL;
               break;
          }

          if (strchr(argv[i], '"'))
          {
               char *new_arg = remove_quotes(argv[i]);
               argv[i] = new_arg;
          }
     }

     if ((pid = fork()) < 0) 
     {     
          fprintf(stderr, "forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) 
     {          
          if (redirect)
          {
               int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

               if (ftruncate(fd, 0) != 0)
                    fprintf(stderr, "ftruncate() error\n");

               dup2(fd, 1);
               close(fd);
          }

          if (execvp(*argv, argv) < 0) 
          {     
               fprintf(stderr, "exec failed\n");
               exit(1);
          }
     }

     if (bg) return;

     else 
     {                                  
          while (wait(&status) != pid)       
               ;
     }
}
     
int main( int argc, char *argv[])
{
     char  line[10240];                            
     char  *args[700];                             

     if (argc > 2)
     {
          fprintf(stderr, "Incorrect number of command line arguments\n");
          exit(1);
     }

     if (argc == 2)
     {    
     	  // Batch mode

          char *filename = argv[1];

          long fsize;
          FILE *fp = fopen(filename, "rb");

          if (!fp)
          {
               fprintf(stderr, "File does not exist\n");
               exit(1);
          }

          char line[10240];
          fseek(fp, 0, SEEK_END);
          fsize = ftell(fp);
          rewind(fp);

          int i = 0;

          while (fgets(line, 10240, fp) != NULL)
          {    
               size_t length = strlen(line);
               printf("%s\n", line);

               if (line[length - 1] == '\n')
                    line[length - 1] = '\0';

               if (parse(line, args) == -1)
                    continue;       

               if (strcmp(args[0], "quit") == 0)
               {
                    while(wait(NULL) != -1)
                         ;
                    exit(0);
               }

               if (strcmp(args[0], "barrier") == 0 || strcmp(args[0], "barrier&") == 0)
               {
                    while(wait(NULL) != -1)
                         ;
                    continue;
               }

               execute(args);
          }

          fclose(fp);
          exit(0);
     }
     
     while (1) 
     {    
     	  // Interactive mode

          printf("$> ");                    
          fgets(line, 10240, stdin);

          size_t length = strlen(line);
          if (line[length - 1] == '\n')
               line[length - 1] = '\0';

          if (parse(line, args) == -1)
               continue;                          

          if (strcmp(args[0], "quit") == 0)
          {
               while(wait(NULL) != -1)
                    ;
               exit(0);                           
          }
          if (strcmp(args[0], "barrier") == 0 || strcmp(args[0], "barrier&") == 0)
          {
               while(wait(NULL) != -1)
                    ;
               continue;
          }

          execute(args);                          
     }
}