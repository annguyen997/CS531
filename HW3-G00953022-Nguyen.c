/*
Student Name: An Nguyen
G-Number: G00953022
Assignment Due Date: April 23, 2020
*/

/* Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h> 

/* Required User-Defined Function Declarations */
int cs531_system(const char *s); /* Run command line s */

//DO NOT MODIFY THIS FUNCTION - This is the main driver.
int main(int argc, char *argv[]){
    int status; //This will hold the overall status of the system command associated with cs531_system(). 
    status = cs531_system(argv[1]);
    sleep(5); //Sleep for 5 seconds upon exit of cs531_system.
    return status; //Return status to environment, end program.
}

//This is the custom system() function used for the purpose of this assignment.
int cs531_system(const char* s){
	//No auxillary functions may be used to accompany this function; the defaults should be used for the signals. 

    pid_t child; //Child process 
    int cstatus; //Exit status of child 
    pid_t c; /* Pid of child to be returned by wait()/waitpid() */ 

    /*
    NOTE FROM INSTRUCTOR: The only problem is that you are not returning SIGINT and SIGQUIT to their previous state following child process termination. 
    Keep in mind (as discussed in class), that SIGINT's previous state is not necessarily SIG_DFL. */

   //Signal handling using signal()
   //Temporarily hold the existing action of SIGINT to a handler by ignoring SIGINT. 
   __sighandler_t prev_sighandler_SIGINT = signal(SIGINT, SIG_IGN); 
   if (prev_sighandler_SIGINT == SIG_ERR){
       //An error occurred while handling SIGINT, and thus no changes should have occurred. 
       printf("Error: SIGINT was not able to be handled.\n"); 
   } else {
       //If SIGINT is sent and child is active, parent process should ignore SIGINT. 
       //Child process should allow SIGINT to behave in its default manner. Once process ends, restore previous handler.
       if (child == 0){
           signal(SIGINT, SIG_DFL); 
           signal(SIGINT, prev_sighandler_SIGINT); 
       }
   }

   //Temporarily hold the existing action of SIGQUIT to a handler by ignoring SIGQUIT.
   __sighandler_t prev_sighandler_SIGQUIT = signal(SIGQUIT, SIG_IGN); 
   if (prev_sighandler_SIGQUIT == SIG_ERR){
       //An error occurred while handling SIGQUIT, and thus no changes should have occurred. 
       printf("Error: SIGQUIT was not able to be handled.\n"); 
   } else {
       //If SIGQUIT is sent and child is active, parent process should ignore SIGQUIT.
       //Child process should allow SIGQUIT to behave in its default manner. Once process ends, restore previous handler.
       if (child == 0){
           signal(SIGQUIT, SIG_DFL); 
           signal(SIGQUIT, prev_sighandler_SIGQUIT); 
       }
   }

   //NOTE SIGCONT and SIGSTOP are handled automatically - This is documented for documentation purposes only.
   /* signal(SIGSTOP, SIG_DFL);
   signal(SIGCONT, SIG_DFL);
   */

   //Attempt to fork the process
   if (child = fork() == 0){
        //This is the child process operating, executing the command indicated in "s".
        execlp("/bin/sh", "/bin/sh", "-c", s, (char *) NULL); 

        //If error occurs at this point, child could not execute excelp()
        fprintf(stderr, "Child could not execute excelp().\n"); 
    } 

    //In the parent process
    else {
        if (child == (pid_t)(-1)){
            //If child is -1, this means the fork did not function properly. 
            printf("Fork failed.");

            //Abnormal exeuction completed. 
            return -1; 
        } else { 
            //This will execute if child is not -1
            do {
                //Wait for the child process to end, and retrieve process ID. 
                //c = wait(&cstatus); 
                c = waitpid(child, &cstatus, WUNTRACED | WCONTINUED); 

                //If process number ID is -1, this means waitpid()/wait() was not used properly.
                if (c == -1) {
                    perror("The waitpid() cannot be used properly.");
                    //Abnormal exeuction completed. 
                    return -1; 
                }

                //Retrieve the status (or statuses) of the child process
                if (WIFEXITED(cstatus)){ 
                    //If the child process ended normally
                    printf("\nChild process %ld exited, status = %d.\n", (long) c, WEXITSTATUS(cstatus));
                    
                    //Parent will sleep (i.e. pause) as indicated in the main call before ending process.
                    printf("Parent is sleeping...\n"); 
                } else if (WIFSIGNALED(cstatus)){
                    //If the child process was terminated abruptly by some signal - i.e. SIGKILL/SIGQUIT
                    kill(c+1, SIGKILL); //Abruptly end associated processes in child process; since there is one command "s", 1 is used.
                    printf("\nChild process %ld killed by signal %d.\n", (long) c, WTERMSIG(cstatus));

                    //Parent will sleep (i.e. pause) as indicated in the main call before ending process.
                    printf("Parent is sleeping...\n"); 
                } else if (WIFSTOPPED(cstatus)){
                    //If the child process was stopped by a signal - i.e. SIGSTOP
                    kill(c+1, SIGSTOP); //Stop associated processes in child process; since there is one command "s", 1 is used. 
                    printf("\nChild process %ld stopped by signal %d.\n", (long) c, WSTOPSIG(cstatus)); 
                } else if (WIFCONTINUED(cstatus)){
                    //If the child process allowed to continue executing - i.e. SIGCONT
                    kill(c+1, SIGCONT); //Continue associated processes in child process; since there is one command "s", 1 is used.
                    printf("\nChild process %ld continued/resumed.\n", (long) c); 
                }
            } while(!WIFEXITED(cstatus) && !WIFSIGNALED(cstatus)); //Continue checking if cstatus continues to change. 

        //Return status of the process
        return cstatus; 
        }
    }

    //Normal execution completed.
    return 0;
}