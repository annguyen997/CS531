/*
Student Name: An Nguyen
G-Number: G00953022
*/

/* Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/* Constant Declarations */
#define MAX_LEN 100
#define MAX_IP_LENGTH 24
#define MAX_LOCALITY 7  //3 digits for first byte, 1 character for ".", and another 3 digits for second byte.

/* Struct address_t to contain IP addresses and their aliases */
struct address_t {
   /* The IPv4 address componentsByAddress
   Note: The firstByte and secondByte members represent the locality of the IP address */
   int firstByte;
   int secondByte;
   int thirdByte;
   int fourthByte;
   /* The alias name of the IP address */
   char alias[MAX_LEN-MAX_IP_LENGTH];
   /* Pointer to the next IP address and its alias */
   struct address_t *next;
};

/* Required User-Defined Function Declarations */
void addAddress();
void searchAddress();
void updateAddress();
void deleteAddress();
void displayList(); //This function calls printList(), which prints the list of IP addresses
void displayListByLocation();
int saveListToFile();
void quitProgram();

/* Other User-Defined Function Declarations */
void openFileAndReadInfo(); //This function handles the creation of a linked list, and thus a separate createList() function is not written.
int getMenuSelection();
void displayAliasesList(int IPLocalityChunks[2]);

/* Other User-Defined Declarations for Standard Linked List Functions */
struct address_t* addNodeToList(int IPChunks[4], char *alias);
struct address_t* searchNodeInListByAlias(char *alias);
struct address_t* searchNodeInListByAddress(int IPChunks[4]);
int deleteNodeFromList(char *alias);
void printList(void); //This is the helper function for displayList().

/* External/Global Variables */
struct address_t *head = NULL; //Pointer to the first address_t in the linked list.
bool listChanged = false; //Check if the list has been modified for purposes to export file when closing application.

//Main function of this program.
int main() {
   int optionNumber = 0; //The user-selected option from the display menu
   bool programActive = true; //Boolean value to indicate active state of this program; user is actively using application and not exiting

   //Open and read file containing IP addresses. If the file is not found, exit the program with error.
   openFileAndReadInfo();

   while (programActive){
      optionNumber = getMenuSelection();
      if (optionNumber == 1){
         //If user wishes to add an address to the list
         addAddress();
      } else if (optionNumber == 2){
         //If user wishes to search an address by an alias name to the list
         searchAddress();
      } else if (optionNumber == 3){
         //If user wishes to update an address by an alias name to the list
         updateAddress();
      } else if (optionNumber == 4){
         //If user wishes to delete an address by an alias name to the list
         deleteAddress();
      } else if (optionNumber == 5){
         //If user wishes to display list of all addresses and aliases in program
         displayList();
      } else if (optionNumber == 6){
         //If user wishes to display list of aliases by locality
         displayListByLocation();
      } else if (optionNumber == 7){
         //If user wishes to save the list in program to an external (output) file.
         saveListToFile();
      } else if (optionNumber == 8){
         //If user wishes to exit the program
         quitProgram();
         //This indicates the program that user has exited, ending this while loop.
         programActive = false;
      } else {
         //An invalid or unknown option is selected.
         printf("Invalid or unknown option selected. Please try again.");
      }
   }

   return 0;
}

//Open the specified file to retrieve IP addresses
void openFileAndReadInfo(){
   //FILE *ptr_file; //Pointer to the opened file containing IP addresses.
   char sfname[MAX_LEN]; //Source file name to open and read IP addresses and associated aliases
   char buffer[MAX_LEN]; //Line buffer when reading each IP address and its alias
   char *str; //The character string to be read from buffer for parsing.
   struct address_t *previous = NULL; //Pointer to the previous node of struct address_t.

   int IPChunks[4] = {0, 0, 0, 0};
   char alias[MAX_LEN - MAX_IP_LENGTH];

   //Open the file containing the IP addresses
   printf("Enter source file name: ");
   scanf("%s", sfname);
   FILE *ptr_file = fopen(sfname, "r"); //Pointer to the opened file containing IP addresses.

   //If file does not exist, exit the function.
   if(ptr_file == NULL){
      printf("Error opening file. Exiting application.\n");
      exit(1);
   }

   //Read each line (containing IP address and alias) in the file
   while ((fgets(buffer, MAX_LEN, ptr_file)) != NULL){
      //Parse the IP address and the alias
      str = buffer;
      sscanf(str, "%d.%d.%d.%d %s", IPChunks, IPChunks+1, IPChunks+2, IPChunks+3, alias);

      //Creates a struct address_t node
      struct address_t *node = (struct address_t*) malloc(sizeof(struct address_t));
      if (NULL == node){
         printf("\n Creating node failed\n");
         return;
      }
      node->firstByte = IPChunks[0];
      node->secondByte = IPChunks[1];
      node->thirdByte = IPChunks[2];
      node->fourthByte = IPChunks[3];
      strcpy(node->alias, alias);
      node->next = NULL;

      //Set the pointer addresses for the linked list. Each node is placed in order presented in the text file.
      //If this is the first element in the list, set head to node (i.e. create the list). Otherwise, set the pointer of previous node to the newly created node.
      if (head == NULL){
         head = node;
      } else {
         previous->next = node;
      }
      previous = node;

      //Indicate that IP address and its alias has been successfully added.
      printf("Address Added: %d.%d.%d.%d\tAlias: %s\n", node->firstByte, node->secondByte, node->thirdByte, node->fourthByte, node->alias);
   }

   //Close the file.
   fclose(ptr_file);
}

//Get the menu selection choice from the user
int getMenuSelection(){
   int optionNumber;
   bool validOptionReceived = false;
   char *menuHeader = "\n\nIPv4 Address Program Menu\n";
   char *menuOptions = "\n \
      1) Add address\n \
      2) Look up address\n \
      3) Update address\n \
      4) Delete address\n \
      5) Display list\n \
      6) Display aliases for location\n \
      7) Save to file\n \
      8) Quit program\n\n";
   char *instructions = "Select a menu option by typing its number: ";

   while(!validOptionReceived){
      printf("%s %s %s", menuHeader, menuOptions, instructions);
      scanf("%d", &optionNumber);
      //If user enters an invalid menu option, reprompt the instructions.
      if (optionNumber < 1 || optionNumber > 8){
         printf("\nError: Menu selection is not valid. Please try again.\n");
      } else {
         validOptionReceived = true;
      }
   }
   return optionNumber;
}

//Add a unique IP address and alias pair to the list.
void addAddress(){
   char buffer[MAX_LEN]; //Line buffer when reading IP address and its alias from user
   char IPBuffer[MAX_LEN]; //Line buffer when reading IP address from user
   int IPChunks[4] = {0, 0, 0, 0}; //Values to store each byte of the new IP address.
   char alias[MAX_LEN - MAX_IP_LENGTH]; //Line buffer when passing alias from user.
   bool newIPAddressValid = false; //Assume inputted IP address is not yet validated until confirmed.
   bool newAliasValid = false; //Assume inputted IP alias is not yet validated until confirmed.

   struct address_t *aliasDuplicate = NULL; //Check if there is an existing alias in the list.
   struct address_t *IPAddressDuplicate = NULL; //Check if there is an existing IP address in the list.

   //FOUND DURING DEVELOPMENT: Flushes the standard input buffer (clears the input buffer).
   while ((getchar()) != '\n');

   //Get the IP address from user input.
   while (!newIPAddressValid){
         //Prompt user to update the address
         printf("\nEnter an IP address, separated by a space (IP format XXX.XXX.XXX.XXX - from 0 to 255 inclusively per byte): ");
         scanf("%s", buffer);
         strcpy(IPBuffer, buffer);
         sscanf(IPBuffer, "%d.%d.%d.%d", IPChunks, IPChunks+1, IPChunks+2, IPChunks+3);

         //Check if the inputted values are within range.
         bool newIPValid = true; //Assume inputted IP address is valid until found otherwise that requires reprompt.
         for (int i = 0; i < 4; i++){
            if(IPChunks[i] < 0 || IPChunks[i] > 255){
               printf("\nByte #%d of new IP address is not within 0 and 255 inclusively.", i+1);
               //If found to be invalid for at least one byte chunk, set the validity to false.
               if (newIPValid == true){
                  newIPValid  = false;
               }
            }
         }

         //If IP address is valid, mark updated IP Address is valid.
         if (newIPValid == true){
            newIPAddressValid = true;
         } else {
            printf("\nPlease enter a valid IP address.\n");
         }
      }

   //FOUND DURING DEVELOPMENT: Flushes the standard input buffer (clears the input buffer).
   while ((getchar()) != '\n');

   //Get the alias from user input.
   do {
      printf("\nEnter an alias for the new IP address %d.%d.%d.%d (alias cannot be blank): ", IPChunks[0], IPChunks[1], IPChunks[2], IPChunks[3]);
      scanf("%s", buffer);
      strcpy(alias, buffer);

      //Check if the inputted alias is not blank
      if (alias[0] == '\0'){
         printf("\nPleaes enter an alias name.");
      } else {
         newAliasValid = true;
      }
   } while (!newAliasValid);

   //Check for duplicated IP address and/or alias
   aliasDuplicate = searchNodeInListByAlias(alias);
   IPAddressDuplicate = searchNodeInListByAddress(IPChunks);
   //If the alias or IP address or both already exists in the linked list (i.e. either or both is not NULL), indicate so.
   if (aliasDuplicate != NULL || IPAddressDuplicate != NULL){
      //If the inputted alias already exists in the linked list, indicate so.
      if(aliasDuplicate != NULL){
         printf("\nError: There is already an IP address that contains this alias name. Alias not added.");
      }
      //If the inputted IP address already exists in the linked list, indicate so.
      if (IPAddressDuplicate != NULL){
         printf("\nError: There is already an IP address of the same values. IP address not added.");
      }
   }
   //If both the inputted alias and the IP address do not exist in the list, it is unique address and alias and thus add to the list.
   else {
      addNodeToList(IPChunks, alias);
      printf("\nNew IP Address Added: %d %d %d %d\nNew Alias: %s\n", IPChunks[0], IPChunks[1], IPChunks[2], IPChunks[3], alias);
      listChanged = true;
   }
}

//Search for an IP address by an alias name inputted from user
void searchAddress(){
   char buffer[MAX_LEN - MAX_IP_LENGTH]; //Line buffer when reading alias from user
   char alias[MAX_LEN - MAX_IP_LENGTH]; //The character string to be read from buffer for parsing.

   //Prompt user to enter an alias of an IP address, and use buffer to stream input
   printf("Type in an address alias to display its IP address: ");
   scanf("%s", buffer);
   strcpy(alias, buffer);

   //Search the alias name in the list, and display alias name and its corresponding IP address.
   //Note IP address are presented within the numbers 0-255.
   struct address_t *found = searchNodeInListByAlias(alias);
   if (found == NULL){
      printf("\nError: Inputted alias does not exist in list; no IP address found.\n");
   } else {
      printf("\nIP Address Alias: %s\nIP Address: %d.%d.%d.%d\n", found->alias, found->firstByte, found->secondByte, found->thirdByte, found->fourthByte);
   }
}

//Search for an IP address by an alias name inputted from user, and update the IP address
void updateAddress(){
   char buffer[MAX_LEN - MAX_IP_LENGTH]; //Line buffer when reading alias from user
   char alias[MAX_LEN - MAX_IP_LENGTH]; //The character string to be read from buffer for parsing.
   char IPBuffer[MAX_LEN]; //Buffer for the IP address when updating.
   int IPChunks[4] = {0, 0, 0, 0};
   bool updatedIPAddressValid = false; //Assume inputted IP address is not yet validated until confirmed.

   //Prompt user to enter an alias of an IP address, and use buffer to stream input
   printf("Type in an address alias to update its IP address: ");
   scanf("%s", buffer);
   strcpy(alias, buffer);

   //Search the alias name in the list, and display alias name and its corresponding IP address.
   //Note IP address are presented within the numbers 0-255.
   struct address_t *found = searchNodeInListByAlias(alias);
   if (found == NULL){
      printf("\nError: Inputted alias does not exist in list; no IP address found.\n");
   } else {
      printf("\nIP Address Alias: %s\nIP Address: %d.%d.%d.%d\n", found->alias, found->firstByte, found->secondByte, found->thirdByte, found->fourthByte);

      while (!updatedIPAddressValid){
         //Prompt user to update the address
         printf("\nEnter an IP address for that alias, separated by a space (IP format XXX.XXX.XXX.XXX - from 0 to 255 inclusively per byte): ");
         scanf("%s", buffer);
         strcpy(IPBuffer, buffer);
         sscanf(IPBuffer, "%d.%d.%d.%d", IPChunks, IPChunks+1, IPChunks+2, IPChunks+3);

         //Check if the inputted values are within range.
         bool inputtedIPAddressToUpdateValid = true; //Assume inputted IP address is valid until found otherwise that requires reprompt.
         for (int i = 0; i < 4; i++){
            if(IPChunks[i] < 0 || IPChunks[i] > 255){
               printf("\nByte #%d of requested IP address is not within 0 and 255 inclusively.", i+1);
               //If found to be invalid for at least one byte chunk, set the validity to false.
               if (inputtedIPAddressToUpdateValid == true){
                  inputtedIPAddressToUpdateValid = false;
               }
            }
         }

         //If IP address is valid, mark updated IP Address is valid.
         if (inputtedIPAddressToUpdateValid == true){
            updatedIPAddressValid = true;
         } else {
            printf("\nError: Please enter a valid IP address.");
         }
      }

      struct address_t *IPAddressDuplicate = searchNodeInListByAddress(IPChunks);
      //If the IP address inputted for change already exists in the linked list (i.e. duplicate), indicate so.
      if (IPAddressDuplicate != NULL){
         printf("\nError: There is already an IP address of the same values as the input. Aborting address change; no changes to the list.\n");
      }
      //If the IP address inputted for change is not a duplicate in the list, it is unique address and thus update.
      else {
         //Update the IP address based on user input
         found->firstByte = IPChunks[0];
         found->secondByte = IPChunks[1];
         found->thirdByte = IPChunks[2];
         found->fourthByte = IPChunks[3];

         //Display the updated IP address as confirmation.
         printf("\nIP Address Updated to: %d.%d.%d.%d", found->firstByte, found->secondByte, found->thirdByte, found->fourthByte);
         listChanged = true;
      }
   }
}

//Delete an IP address from the list by an alias name inputted from user
void deleteAddress(){
   char buffer[MAX_LEN - MAX_IP_LENGTH]; //Line buffer when reading alias from user
   char alias[MAX_LEN - MAX_IP_LENGTH]; //The character string to be read from buffer for parsing.
   int deleteOption; //Character to hold delete option from user. Default is NULL, which for purposes of this program is equivalent to No (i.e. user does not wish to delete the IP address).

   //Prompt user to enter an alias of an IP address, and use buffer to stream input
   printf("Type in an address alias to delete from the list: ");
   scanf("%s", buffer);
   strcpy(alias, buffer);

   //Search the alias name in the list, and display alias name and its corresponding IP address.
   //Note IP address are presented within the numbers 0-255.
   struct address_t *found = searchNodeInListByAlias(alias);
   if (found == NULL){
      printf("Error: Inputted alias does not exist in list; no IP address found.");
   } else {
      printf("IP Address Alias: %s\nIP Address: %d.%d.%d.%d\n", found->alias, found->firstByte, found->secondByte, found->thirdByte, found->fourthByte);

      //FOUND DURING DEVELOPMENT: Flushes the standard input buffer (clears the input buffer from previous input request).
      while ((getchar()) != '\n');

      //Request if user wishes to delete the IP address and its alias from the list.
      printf("Do you wish to delete this address? ('Y' to confirm, 'N' to cancel): ");
      deleteOption = getchar();

      //If the delete option is typed from the user, do actions based on the input. If NULL or none inputted, assume no changes.
      while (deleteOption = toupper(deleteOption)){
         //If user wishes to delete the IP address
         if (deleteOption == 'Y'){
            if ((deleteNodeFromList(alias)) == 0){
               printf("IP address and its alias were deleted successfully.\n");
               break;
            } else {
               printf("Error: IP address and its alias were not deleted successfully.\n");
               break;
            }
         } //If user types in an invalid delete option
         else if (deleteOption != 'N'){
            printf("\nInvalid character typed. Please try again.\n");

            //FOUND DURING DEVELOPMENT: Flushes the standard input buffer (clears the input buffer).
            while ((getchar()) != '\n');

            printf("Do you wish to delete this address? ('Y' to confirm, 'N' to cancel): ");
            deleteOption = getchar();
         } //If user wishes to not delete the IP address, indicate so.
         else if (deleteOption == 'N'){
            printf("IP address and its alias are not deleted from the list.");
            break;
         }
      }
      //Although a IP address may not have been removed, this is being used during saving to output file as a safety mechanism.
      listChanged = true;
   }
}

//Display the list of IP addresses and accompanying aliases
void displayList(){
   printList();
}

//Display the list of aliases that map to the inputted location values (i.e. first two bytes of the IP address)
void displayListByLocation(){
   int numCount = 0; //Count the number of IP addresses containing the locality inputted from user.
   char buffer[MAX_LOCALITY]; //Line buffer when reading locality numbers from user.
   char *str; //The character string to be read from buffer for parsing.
   int IPLocalityChunks[2] = {0, 0};
   bool firstByteValid = false;
   bool secondByteValid = false;

   while (firstByteValid != true){
      printf("\nEnter first IP address chunk, value 0 to 255 inclusively: ");
      scanf("%d", &IPLocalityChunks[0]);

      if ((IPLocalityChunks[0]) < 0 || (IPLocalityChunks[0]) > 255){
         printf("\nError: Number is not valid; must be within 0 and 255 inclusively.\n");
      } else {
         firstByteValid = true;
      }
   }

   while (secondByteValid != true){
      printf("\nEnter second IP address chunk, value between 0 to 255 inclusively: ");
      scanf("%d", &IPLocalityChunks[1]);

      if ((IPLocalityChunks[1]) < 0 || (IPLocalityChunks[1]) > 255){
         printf("\nError: Number is not valid; must be within 0 and 255 inclusively.\n");
      } else {
         secondByteValid = true;
      }
   }

   displayAliasesList(IPLocalityChunks);
}

/* An adaptation of searchNodeInList() - Search and display IP aliases based on locality in the existing linked list - and display address. Helper function of displayListByLocation()*/
void displayAliasesList(int IPLocalityChunks[2]){
   struct address_t *ptr = head; //Start at the head of the linked list; cycles through each node in the list.
   bool found = false; //Indicates if at least one alias for the inputted locality is found.

   //Starting from the head of the list, continuously find IP addresses/aliases that match locality, up to the end of the list.
   while(ptr != NULL){
      //If first and second bytes of a node in the list matches the locality, print the alias associated for that locality.
      if (ptr->firstByte == IPLocalityChunks[0] && ptr->secondByte == IPLocalityChunks[1]){
         //If a locality is matched for the first time, indicate the header of found aliases (one time only).
         if (found == false){
            found = true;
            printf("\nAliases Found for Locality %d.%d:\n", IPLocalityChunks[0], IPLocalityChunks[1]);
         }
         printf("%s\n", ptr->alias);
      }
      ptr = ptr->next;
   }
   //This statement is here for formatting purposes on standard output.
   printf("\n");

   if (found == false){
      printf("Locality does not exist in the list. No aliases found in the specified location.");
   }
}

//Save the list of IP addresses currently in this program to an external file
int saveListToFile(){
   FILE *fileToSave; //Pointer to the destination file to save IP addresses and associated aliases
   char dfname[MAX_LEN]; //Destination file to save IP addresses and associated aliases
   int numCount = 0; //The number of IP addresses and aliases that were able to be exported

   //Open the file containing the IP addresses
   printf("Enter destination file name: ");
   scanf("%s", dfname);
   fileToSave = fopen(dfname, "w");

   //Return indicating file was not created or some error occurred.
   if (fileToSave == NULL){
      printf("Error opening destination file.");
      return 1;
   }

   //Export linked list (IP addresses and aliases) to the output file named by the user
   struct address_t *ptr = head;
   while (ptr != NULL){
      fprintf(fileToSave, "%d.%d.%d.%d %s\n", ptr->firstByte, ptr->secondByte, ptr->thirdByte, ptr->fourthByte, ptr->alias);
      ptr = ptr->next;
      numCount++;
   }

   //Close the file, and indicate list of IP addresses were saved to output file.
   fclose(fileToSave);
   printf("%d IP addresses and associated aliases were saved to the specified output file '%s'.\n", numCount, dfname);
   listChanged = false;

   //Return indicating successful execution
   return 0;
}

//Quit the program, and save the file if necessary.
void quitProgram(){
   int fileSaved = 0;

   //If user has not yet saved the list to the file, save the list to the file. Otherwise, exit the program directly.
   if (listChanged == true){
      fileSaved = saveListToFile();
      //If IP addresses and alises from list were able to be saved to an export file, close the program.
      if (fileSaved == 0){
         //Deallocate memory.
         struct address_t *ptr = head;
         struct address_t *remove= NULL;
         while (ptr != NULL){
            remove = ptr;
            ptr = ptr->next;
            free(remove);
         }
         printf("Exiting program.");
      }
      //If IP addresses and aliases from list were not able to be saved to an export file, indicate so.
      else {
         printf("List of IP addresses and aliases cannot be saved to the export file.\n");
      }
   } else {
      //If user did save the list to the file before requesting to exit program, exit the program directly.

      //Deallocate memory.
      struct address_t *ptr = head;
      struct address_t *remove= NULL;
      while (ptr != NULL){
         remove = ptr;
         ptr = ptr->next;
         free(remove);
      }
      printf("Exiting program.\n");
   }
}

/* Standard Linked List Functions

NOTE to GRADER: The openFileAndReadInfo() function defined earlier in this file covers the functionality of creating a linked list. To prevent inadverent code changes given the high number of pointers, no createList() function definition is defined in this area.
*/

/* Add IP address and alias to the beginning of the existing linked list */
struct address_t* addNodeToList(int IPChunks[4], char *alias){
   //Creates a struct address_t node
   struct address_t *node = (struct address_t*) malloc(sizeof(struct address_t));
   if (NULL == node){
      printf("\n Creating node failed\n");
      return NULL;
   }

   //Assign the IP address and the alias to members of the node.
   node->firstByte = IPChunks[0];
   node->secondByte = IPChunks[1];
   node->thirdByte = IPChunks[2];
   node->fourthByte = IPChunks[3];
   strcpy(node->alias, alias);
   node->next = NULL;

   //Set the pointer addresses of new IP address struct for the linked list, placed at the beginning of the list
   if (head == NULL){
      head = node;
   } else {
      node->next = head;
      head = node;
   }
}

/* Search an IP address by its alias in the existing linked list */
struct address_t* searchNodeInListByAlias(char *alias){
   struct address_t *ptr = head;
   struct address_t *foundNode = NULL;
   bool found = false;

   while(ptr != NULL){
      if ((strcmp(ptr->alias, alias)) == 0){
         found = true;
         break;
      }
      ptr = ptr->next;
   }

   if (found == true){
      foundNode = ptr;
      return foundNode;
   } else {
      return NULL;
   }
}

/* Search an IP address (or its byte chunks) in the existing linked list */
struct address_t* searchNodeInListByAddress(int IPChunks[4]){
   struct address_t *ptr = head;
   struct address_t *foundNode = NULL;
   bool found = false;

   while(ptr != NULL){
      if (ptr->firstByte == IPChunks[0] && ptr->secondByte == IPChunks[1] && ptr->thirdByte == IPChunks[2] && ptr->fourthByte == IPChunks[3]){
         found = true;
         break;
      }
      ptr = ptr->next;
   }

   if (found == true){
      foundNode = ptr;
      return foundNode;
   } else {
      return NULL;
   }
}

/* Delete an IP address and its alias from the existing linked list */
int deleteNodeFromList(char *alias){
   struct address_t *ptr = head;
   struct address_t *prev = NULL;
   bool found = false;

   //Find the IP address and its alias in the linked list
   while(ptr != NULL){
      if ((strcmp(ptr->alias, alias)) == 0){
         found = true;
         break;
      }
      prev = ptr;
      ptr = ptr->next;
   }

   //If the IP address to delete has been found, delete the address by reassigning the pointer variables of other IP addresses.
   if (found == true){
      //If the address to delete is at the head.
      if (ptr == head) {
         //If address being deleted is the only one in the list
         if (ptr->next == NULL){
            head = NULL;
         } else {
            head = ptr->next;
         }
      } else {
         //If the address to delete is somewhere in the middle of list OR at the end of the list
         prev->next = ptr->next;
      }
   } else {
      //IP address to delete was not found, or some error occurred.
      return -1;
   }

   //Deallocate the struct containing IP address and its alias
   free(ptr);
   ptr = NULL;
   return 0;
}

//Print the linked list of IP addresss and aliases.
void printList(){
   struct address_t *ptr = head;
   int numCount = 0; //Count of the number of IP addresses

   printf("\n ------- Start of list of IP addresses ------- \n");

   while(ptr != NULL) {
      printf("\n #%d\t IP Address: %d.%d.%d.%d \t Address Alias: %s", numCount+1, ptr->firstByte, ptr->secondByte, ptr->thirdByte, ptr->fourthByte, ptr->alias);
      ptr = ptr->next;
      numCount++;
   }
   printf("\n\n ------ End of list of IP addresses ------- \n");
}

