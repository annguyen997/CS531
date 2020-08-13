//Full Name: An Nguyen (anguye60)
//G-Number: G00953022

/* Header Files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* Constant Declarations */
#define MAX_STRINGS 10
#define MAX_LENGTH 25

/* Function Declarations */
//There are 7 user-defined functions excluding main()
bool isInputEmpty(char*);
bool doesInputAlreadyExist(char[], char[MAX_STRINGS][MAX_LENGTH+1]);
bool doesInputHaveIllegalChars(char*);
char selectSortOption();
void bubbleSort(char[MAX_STRINGS][MAX_LENGTH+1]);
void printStringsSorted(char[MAX_STRINGS][MAX_LENGTH+1], char); 
void determineLowestAndHighestStrings(char[MAX_STRINGS][MAX_LENGTH+1]); 

//Main execution of this program
int main(){
	//The array of 10 strings with MAX_LENGTH + 1 size each.
    char strings[MAX_STRINGS][MAX_LENGTH+1]; 
	//MAX_LENGTH + 1 to accommodate the '\0' character may be available in buffer
    char userInput[MAX_LENGTH+1]; 
	//The use of an input stream buffer is used due to some issues I have encountered early in development.
    char allocationBuffer[100];

    //Inform user the number of strings to enter.
	printf("Enter %d character strings:\n\n", MAX_STRINGS);

    //Count the number of strings inputted and successfully stored in strings[] array.
	int stringCounter = 0;
	while(stringCounter < MAX_STRINGS){
	    //User enters a string, and program retrieves string from buffer via stdin.
		printf("Enter string %d (Max %d characters): ", stringCounter+1, MAX_LENGTH);
		fgets(allocationBuffer, sizeof(allocationBuffer), stdin);

		//If length of the string in allocated buffer is greater than max_num characters.
		int len = strlen(allocationBuffer);
		if (len > MAX_LENGTH) {
            printf("Error: String entered is too long (more than 25 chars). Please re-enter string.\n");
		} else {
		    //Copy the string from allocated buffer to user input array for validation
		    strcpy(userInput, allocationBuffer);
		    //Check if the user entered an empty string.
		    if (isInputEmpty(userInput)){
                printf("Error: Empty inputted string. Please enter a string.\n");
		    }
		    //Check if the string inputted is already inputted; verify uniqueness of string
		    else if (doesInputAlreadyExist(userInput, strings)){
		        printf("Error: Duplicate inputted string. Please re-enter a different string.\n");
		    }
		    //Check if the string contains illegal characters
		    else if (doesInputHaveIllegalChars(userInput)){
		        printf("Please re-enter string.\n");
		    }
		    //The string is unique, and contains no illegal characters
		    else {
			    strcpy(strings[stringCounter], userInput);
			    stringCounter++;
		    }
		}
	}

    //Print the array pre-sort (for verification purposes)
    printf("\nElements in array pre-sort:\n");
    for (int i = 0; i < MAX_STRINGS; i++){
        printf("%s", strings[i]);
    }

	//Sort the character strings either ascending or descending by user choice
	char orderSelection = selectSortOption();
	
    //Sort the array with bubble sort. 
    bubbleSort(strings);
	
    //Present the strings by selected order
	printStringsSorted(strings, orderSelection); 
	
    //Present the two strings with lowest or highest ASCII values using strcmp()
	determineLowestAndHighestStrings(strings); 
	
    //Program execution successful
    return 0;
}

bool isInputEmpty(char *userInput){
    //Given inputted array - if the length of string is less than 1 OR there is a '\n' in the first subscript of the userInput char array.
	 //The * is used to get the char value of the first location of array (deferencing).
    if (strlen(userInput) < 1 || *userInput == '\n'){
        return true;
    }
    //Indicating there is a length of at least 1 (i.e. at least one character). 
    return false;
}

bool doesInputAlreadyExist(char userInput[], char strings[MAX_STRINGS][MAX_LENGTH+1]){
    //Get the string in strings 2D array for comparison purposes
    char currentStringComp[MAX_LENGTH+1];

	//Since size of strings[] array is static and known beforehand, the MAX_STRING constant is used.
	for (int i = 0; i < MAX_STRINGS; i++){
		//If user-inputed string matches exactly an existing string, return true indicating duplicate.
		//Use strings[i] to get the length of a string in the memory location of strings[i].
		strcpy(currentStringComp, strings[i]);
		if(strcmp(currentStringComp, userInput) == 0){
		    printf("Duplicate found at strings[%d]\n", i);
			return true;
		}
	}
	return false;
}

bool doesInputHaveIllegalChars(char *userInput){
	//Get size of input string
	size_t sizeUserInput = strlen(userInput);
	//Boolean value if illegal char is present
	bool haveIllegalChar = false;
	//Get current character in input to check for illegal char
	char checkChar;

	//Loop through the entire user-inputted string to find illegal characters
	for (int i = 0; i < sizeUserInput; i++){
		//Get the character at index i 
		checkChar = userInput[i];
		//If checkChar contains an illegal character
		if (checkChar == '!' || checkChar == '@' || checkChar == '#' || checkChar == '$' || checkChar == '%' || checkChar == '^' || checkChar == '(' || checkChar == ')'){
			//Print the illegal character provided in inputted string; a string may have more than one illegal char
			printf("Error: %c is an illegal character.\n", checkChar);
			haveIllegalChar = true;
		}
	}
	return haveIllegalChar;
}

char selectSortOption(){
	//Variable used to hold the char value of user's selected sorting option.
    char option; 

    while(option != 'A' || option != 'D'){
        printf("\nPrint character strings in Ascending or Descending order.\nType 'A' for Ascending, 'D' for Descending: ");
	    option = getchar();
	    //If user enters a lowercase letter 'A' or 'D', shift ASCII left by 32.
	    if (option == 'a' || option == 'd'){
            option -= 32;
        }

        //If option is valid, break the while loop. Otherwise, re-prompt option choice.
        //This is a little verbose, but this is used as apparently (option != 'A' || option != 'D') was not recognized by compiler.
        if (option == 'A' || option == 'D'){
            break;
        } else {
            printf("\nInvalid order selection. Please try again.\n");
        }
    }
    return option;
}

void bubbleSort(char strings[MAX_STRINGS][MAX_LENGTH+1]){
	//The temp char array for swapping if string1 < string2
    char temp[MAX_LENGTH+1];
	//Counter variables for the looping.
    int inner, outer; 

    for (outer = 0; outer < MAX_STRINGS; outer++){
        for (inner = 0; inner < MAX_STRINGS - 1; inner++){
            if (strcmp(strings[outer], strings[inner]) < 0){
                //Perform the swapping with the temp char array.
                strcpy(temp, strings[outer]);
                strcpy(strings[outer], strings[inner]);
                strcpy(strings[inner], temp);
            }
        }
    }
}

void printStringsSorted(char strings[MAX_STRINGS][MAX_LENGTH+1], char orderSelection){
	//If user selected for ascending order of the sorted strings
	if (orderSelection == 'A'){
        printf("\nAscending Order:\n");
        for (int i = 0; i < MAX_STRINGS; i++){
            printf("%s", strings[i]);
        }
    } 
	//If user selected for descending order of the sorted strings
	else if (orderSelection == 'D'){
        printf("\nDescending Order:\n");
        for (int i = MAX_STRINGS - 1; i >= 0; i--){
            printf("%s", strings[i]);
        }
    }
}

void determineLowestAndHighestStrings(char strings[MAX_STRINGS][MAX_LENGTH+1]){
	//If strings[0] is less than strings[MAX_STRINGS-1]
	if (strcmp(strings[0], strings[MAX_STRINGS-1]) < 0){ 
        printf("\nString with the lowest ASCII value: %s", strings[0]);
        printf("String with the highest ASCII value: %s", strings[MAX_STRINGS-1]);
    } 
	//Otherwise (i.e. strings[0] is greater than to strings[MAX_STRINGS-1])
	else { 
        printf("\nString with the lowest ASCII value: %s", strings[MAX_STRINGS-1]);
        printf("String with the highest ASCII value: %s", strings[0]);
    }
}
