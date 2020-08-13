/*
Student Name: An Nguyen
G-Number: G00953022
Assignment Due: May 07, 2020
*/

/* Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/* Constant Declarations */
#define MAX_LEN 35
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
   char alias[MAX_LEN - MAX_IP_LENGTH];

   /* Pointer to the next IP address and its alias on this node's left */
   struct address_t *leftChild;

   /* Pointer to the next IP address and its alias on this node's right */
   struct address_t *rightChild;
};

/* Required User-Defined Function Declarations */
void addAddress();
void searchAddress();
void updateAddress();
void deleteAddress();
void displayList(); //This function calls inOrder(), which prints in aliases in inorder traversal of the BST (i.e. alphabetical order)
void displayListByLocation();
int saveListToFile();
void quitProgram();

/* Other User-Defined Function Declarations */
void openFileAndReadInfo(); //This function handles the creation of a binary search tree.
int getMenuSelection(); //This function handles the menus selection inputted by the user.
bool displayAliases(struct address_t* node, int IPLocalityChunks[2]);
int writeAddressesToFile(FILE *fileToSave, struct address_t* node);

/* Other User-Defined Declarations for Standard Binary Search Tree (BST) Functions */
struct address_t* newNode(int IPChunks[4], char *alias);
struct address_t* insertNode(struct address_t* node, struct address_t* newNode);
struct address_t* addNodeToTree(int IPChunks[4], char *alias);
struct address_t* updateNodeInTree(int IPChunks[4], struct address_t* node, char *alias);
struct address_t* deleteNodeFromTree(struct address_t* node, char *alias);
struct address_t* BSTSearchByAlias(struct address_t* node, char *alias);
void inOrder(struct address_t *node);
struct address_t* minValueNode(struct address_t* node);

/* External/Global Variables */
struct address_t* root = NULL; //Pointer to the first address_t in the binary tree, the root.
bool treeChanged = false; //Check if the list has been modified for purposes to export file when closing application.

//Main function of this program.
int main() {
   int optionNumber = 0; //The user-selected option from the display menu
   bool programActive = true; //Boolean value to indicate active state of this program; user is actively using application and not exiting

   //Open and read file containing IP addresses. If the file is not found, exit the program with error.
   openFileAndReadInfo();

   while (programActive){ //The program remains active until user quits the program
      //Get the menu selection from the user
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

   //Denote normal execution of the program
   return 0;
}




/* Required User-Defined Function Declarations */

//Add a unique IP address and alias pair to the list.
void addAddress(){
   char buffer[MAX_LEN]; //Line buffer when reading IP address and its alias from user
   char IPBuffer[MAX_LEN]; //Line buffer when reading IP address from user
   int IPChunks[4] = {0, 0, 0, 0}; //Values to store each byte of the new IP address.
   char alias[MAX_LEN - MAX_IP_LENGTH]; //Line buffer when passing alias from user.
   bool newIPAddressValid = false; //Assume inputted IP address is not yet validated until confirmed.
   bool newAliasValid = false; //Assume inputted IP alias is not yet validated until confirmed.

   struct address_t *aliasDuplicate = NULL; //Check if there is an existing alias in the list.

   //Flushes the standard input buffer (clears the input buffer).
   while ((getchar()) != '\n');

   //Get the IP address from user input.
   while (!newIPAddressValid){
         //Prompt user to update the address
         printf("\nEnter an IP address, separated by a space (IP format XXX.XXX.XXX.XXX - from 0 to 255 inclusively per byte): ");
         scanf("%s", buffer);
         strcpy(IPBuffer, buffer);
         //Store the new IP address into the IP chunks array.
         sscanf(IPBuffer, "%d.%d.%d.%d", IPChunks, IPChunks+1, IPChunks+2, IPChunks+3);

         //Check if the inputted values are within range.
         bool newIPValid = true; //Assume inputted IP address is valid until found otherwise that requires reprompt.
         for (int i = 0; i < 4; i++){
            if(IPChunks[i] < 0 || IPChunks[i] > 255){
               printf("\nByte #%d of new IP address, %d, is not within 0 and 255 inclusively.", i+1, IPChunks[i]);
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

   //Flushes the standard input buffer (clears the input buffer).
   while ((getchar()) != '\n');

   //Get the alias from user input.
   do {
      printf("\nEnter an alias for the new IP address %d.%d.%d.%d (alias cannot be blank): ", IPChunks[0], IPChunks[1], IPChunks[2], IPChunks[3]);
      scanf("%s", buffer);
      strcpy(alias, buffer);

      //Check if the inputted alias is not blank
      if (alias[0] == '\0'){
         printf("\nPlease enter an alias name.");
      } else {
         newAliasValid = true;
      }
   } while (!newAliasValid);

   //Check for duplicated alias in the tree (note the same IP addresses may be in the tree as long as the aliases names are different.)
   aliasDuplicate = BSTSearchByAlias(root, alias);

   //If the alias name already exists in the tree, indicate so.
   if (aliasDuplicate != NULL){
      printf("\nError: There is already an IP address that contains this alias name '%s'. Alias not added.", alias);
   }

   //If both the inputted alias do not exist in the list, it is unique address and alias and thus add to the list.
   else {
      addNodeToTree(IPChunks, alias);
      printf("\nNew IP Address Added: %d.%d.%d.%d\nNew Alias: %s\n", IPChunks[0], IPChunks[1], IPChunks[2], IPChunks[3], alias);
      treeChanged = true;
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
   struct address_t* found = BSTSearchByAlias(root, alias);
   if (found == NULL){
      printf("\nError: Inputted alias '%s' does not exist in list; no IP address found.\n", alias);
   } else {
      printf("\nAddress Found:\nIP Address Alias: %s\nIP Address: %d.%d.%d.%d\n", found->alias, found->firstByte, found->secondByte, found->thirdByte, found->fourthByte);
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
   struct address_t* found = BSTSearchByAlias(root, alias);
   if (found == NULL){
      printf("\nError: Inputted alias '%s' does not exist in list; no IP address found.\n", alias);
   } else {
      printf("\nFound:\nIP Address Alias: %s\nIP Address: %d.%d.%d.%d\n", found->alias, found->firstByte, found->secondByte, found->thirdByte, found->fourthByte);

      while (!updatedIPAddressValid){
         //Prompt user to update the address
         printf("\nEnter an IP address for that alias, '%s', separated by a space (IP format XXX.XXX.XXX.XXX - from 0 to 255 inclusively per byte): ", alias);
         scanf("%s", buffer);
         strcpy(IPBuffer, buffer);
         //Store the new IP address into the IP chunks array.
         sscanf(IPBuffer, "%d.%d.%d.%d", IPChunks, IPChunks+1, IPChunks+2, IPChunks+3);

         //Check if the inputted values are within range (check each chunk at a time).
         bool inputtedIPAddressToUpdateValid = true; //Assume inputted IP address is valid until found otherwise that requires reprompt.
         for (int i = 0; i < 4; i++){
            if(IPChunks[i] < 0 || IPChunks[i] > 255){
               printf("\nByte #%d of requested IP address, %d, is not within 0 and 255 inclusively.", i+1, IPChunks[i]);
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

      //Update the IP address based on user input
      struct address_t* updatedNode = updateNodeInTree(IPChunks, root, alias);

      if (updatedNode == NULL){
         //If the address was not able to be updated (i.e. returned node is NULL).
         printf("Error: Address was not able to be updated. Aborting changes.");
      } else {
         //Display the updated IP address as confirmation.
         printf("\nIP Address Updated to: %d.%d.%d.%d", updatedNode->firstByte, updatedNode->secondByte, updatedNode->thirdByte, updatedNode->fourthByte);
         treeChanged = true;
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
   struct address_t *found = BSTSearchByAlias(root, alias);
   if (found == NULL){
      //If there are no aliases of such name in the tree, indicate so.
      printf("Error: Inputted alias '%s' does not exist in list; no IP address found.", alias);
   } else {
      printf("IP Address Alias: %s\nIP Address: %d.%d.%d.%d\n", found->alias, found->firstByte, found->secondByte, found->thirdByte, found->fourthByte);

      //Flushes the standard input buffer (clears the input buffer from previous input request).
      while ((getchar()) != '\n');

      //Request if user wishes to delete the IP address and its alias from the list.
      printf("Do you wish to delete this address? ('Y' to confirm, 'N' to cancel): ");
      deleteOption = getchar();

      //If the delete option is typed from the user, do actions based on the input. If NULL or none inputted, assume no changes.
      while (deleteOption = toupper(deleteOption)){
         //If user wishes to delete the IP address
         if (deleteOption == 'Y'){
            if ((deleteNodeFromTree(root, alias)) != NULL){
               printf("IP address and its alias were deleted successfully.\n");
               break;
            } else {
               printf("Error: IP address and its alias were not deleted successfully.\n");
               break;
            }
         } //If user wishes to not delete the IP address, indicate so.
         else if (deleteOption == 'N'){
            printf("IP address and its alias are not deleted from the list.");
            break;
         } //If user types in an invalid delete option
         else if (deleteOption != 'N'){
            printf("\nInvalid character typed. Please try again.\n");

            //Flushes the standard input buffer (clears the input buffer).
            while ((getchar()) != '\n');

            printf("Do you wish to delete this address? ('Y' to confirm, 'N' to cancel): ");
            deleteOption = getchar();
         }
      }

      //Although a IP address may not have been removed, this is being used during saving to output file as a safety mechanism.
      treeChanged = true;
   }
}

//Display all IP addresses and accompanying aliases by traversing through the BST.
void displayList(){
   inOrder(root);
}

//Display the aliases that map to the inputted location values (i.e. first two bytes of the IP address) by traversing through the BST.
void displayListByLocation(){
   int numCount = 0; //Count the number of IP addresses containing the locality inputted from user.
   char buffer[MAX_LOCALITY]; //Line buffer when reading locality numbers from user.
   char *str; //The character string to be read from buffer for parsing.
   int IPLocalityChunks[2] = {0, 0};
   bool firstByteValid = false;
   bool secondByteValid = false;

   while (firstByteValid != true){
      //Enter the first chunk of locality of an IP address
      printf("\nEnter first IP address chunk, value 0 to 255 inclusively: ");
      scanf("%d", &IPLocalityChunks[0]);

      //If the first IP chunk is not in range, indicate so, and re-request. Otherwise, go to the next chunk for locality.
      if ((IPLocalityChunks[0]) < 0 || (IPLocalityChunks[0]) > 255){
         printf("\nError: Number '%d' is not valid; must be within 0 and 255 inclusively.\n", IPLocalityChunks[0]);
      } else {
         firstByteValid = true; //First byte is valid
      }
   }

   while (secondByteValid != true){
      //Enter the second chunk of locality of an IP address
      printf("\nEnter second IP address chunk, value between 0 to 255 inclusively: ");
      scanf("%d", &IPLocalityChunks[1]);

      //If the first IP chunk is not in range, indicate so, and re-request. Otherwise, the second byte is valid.
      if ((IPLocalityChunks[1]) < 0 || (IPLocalityChunks[1]) > 255){
         printf("\nError: Number '%d' is not valid; must be within 0 and 255 inclusively.\n", IPLocalityChunks[1]);
      } else {
         secondByteValid = true; //Second byte is valid.
      }
   }

   //Search for the aliases that contain IP addresses that match the IP locality.
   printf("\nAliases for Location %d.%d:\n", IPLocalityChunks[0], IPLocalityChunks[1]);
   bool found = displayAliases(root, IPLocalityChunks);
   if (found != true){
      //If no aliases in the given locality are found, indicate so.
      printf("Error: Locality does not exist in the list. No aliases found in the specified location.");
   }
   //Otherwise, the aliases will be displayed given the locality.

}

//Save the list of IP addresses currently in this program to an external file
int saveListToFile(){
   FILE *fileToSave; //Pointer to the destination file to save IP addresses and associated aliases
   char dfname[MAX_LEN]; //Destination file to save IP addresses and associated aliases

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
   int numCount = writeAddressesToFile(fileToSave, root);

   //Close the file, and indicate list of IP addresses were saved to output file.
   fclose(fileToSave);
   printf("%d IP addresses and associated aliases were saved to the specified output file '%s'.\n", numCount, dfname);
   treeChanged = false;

   //Return indicating successful execution
   return 0;
}



//Quit the program, and save the file if necessary.
void quitProgram(){
   int fileSaved = 0; //Integer signal to check if addresses and alises were able to be saved to export file.

   //If user has not yet saved the list to the file, save the list to the file. Otherwise, exit the program directly.
   if (treeChanged == true){
      fileSaved = saveListToFile();
      //If IP addresses and alises from list were able to be saved to an export file, close the program.
      if (fileSaved == 0){
         //Deallocate memory.
         while(root != NULL){
            root = deleteNodeFromTree(root, root->alias);
         }
         free(root);
         printf("Exiting program. Goodbye!\n");
      }
      //If IP addresses and aliases from list were not able to be saved to an export file, indicate so.
      else {
         printf("List of IP addresses and aliases cannot be saved to the export file.\n");
      }
   } else {
      //If user did save the list to the file before requesting to exit program, exit the program directly.

      //Deallocate memory.
      while(root != NULL){
         root = deleteNodeFromTree(root, root->alias);
      }
      free(root);
      printf("Exiting program. Goodbye!\n");
   }
}









/* Other User-Defined Function Declarations */

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
      6) Display names for location\n \
      7) Save to file\n \
      8) Quit program\n\n";
   char *instructions = "Select a menu option by typing its number: ";

   while(!validOptionReceived){
      //Display the menu options
      printf("%s %s %s", menuHeader, menuOptions, instructions);
      scanf("%d", &optionNumber);
      //If user enters an invalid menu option, reprompt the instructions.
      if (optionNumber < 1 || optionNumber > 8){
         printf("\nError: Menu selection %d is not valid. Please try again.\n", optionNumber);
      } else {
         //User selected a valid menu option.
         validOptionReceived = true;
      }
   }
   //Return the menu option for program to process the user request.
   return optionNumber;
}

//Read the opened file, if successful, and add the addresses in file to the BST.
void openFileAndReadInfo(){
   char sfname[MAX_LEN]; //Source file name to open and read IP addresses and associated aliases
   char buffer[MAX_LEN]; //Line buffer when reading each IP address and its alias
   char *str; //The character string to be read from buffer for parsing.

   //int IP1, IP2, IP3, IP4; //For reading purposes
   int IPChunks[4];
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
      sscanf(str, "%u.%u.%u.%u %s", IPChunks, IPChunks+1, IPChunks+2, IPChunks+3, alias);

      //Add the new address, and insert that new address to the tree.
      struct address_t* newAddress = addNodeToTree(IPChunks, alias);

      //Indicate that IP address and its alias has been successfully added.
      printf("Address Added: %d.%d.%d.%d\tAlias: %s\n", newAddress->firstByte, newAddress->secondByte, newAddress->thirdByte, newAddress->fourthByte, newAddress->alias);
   }

   //Close the file.
   fclose(ptr_file);
}

/* An adaptation of searchNodeInList() - Search and display IP aliases based on locality in the existing linked list and display address.
Helper function of displayListByLocation() */
bool displayAliases(struct address_t* node, int IPLocalityChunks[2]){

   //If node is NULL, return false.
   if (node == NULL){
      return false;
   }

    /* Recur on left subtree */
    bool found = displayAliases(node->leftChild, IPLocalityChunks);

    /* Check the current node for locality */
    if (node->firstByte == IPLocalityChunks[0] && node->secondByte == IPLocalityChunks[1]){
       printf("%s\n", node->alias);
       if (found == false){
          found = true;
       }
    }

    /* Recur on right subtree */
    bool found2 = displayAliases(node->rightChild, IPLocalityChunks);
    if (found == false){
       found = found2;
    }

   // Return if there are aliases found
   return found;
}

//Helper function write all addresses and its aliases to the external file.
int writeAddressesToFile(FILE *fileToSave, struct address_t* node){
   //The number of addresses/aliases saved
   int num = 0;

   //If node is null, return 0 (meaning no nodes).
   if (node == NULL){
      return 0;
   }

   /* Recur on left subtree */
   num = writeAddressesToFile(fileToSave, node->leftChild);

   /* Print the current node */
   fprintf(fileToSave, "%d.%d.%d.%d %s\n", node->firstByte, node->secondByte, node->thirdByte, node->fourthByte, node->alias);
   num += 1;

   /* Recur on right subtree */
   num += writeAddressesToFile(fileToSave, node->rightChild);

   //Return the number of nodes found.
   return num;
}










/*  Functions for Standard Binary Search Tree */

//Create a new node for the BST
struct address_t* newNode(int IPChunks[4], char *alias){

   //Creates a struct address_t node
   struct address_t* node = (struct address_t*) malloc(sizeof(struct address_t));

   //If the node is null, this means there was no memory allocated for this node.
   if (NULL == node){
      printf("\n Creating node failed\n");
      return NULL;
   }

   //Assign the values the nodes.
   node->firstByte = IPChunks[0];
   node->secondByte = IPChunks[1];
   node->thirdByte = IPChunks[2];
   node->fourthByte = IPChunks[3];
   strcpy(node->alias, alias);
   node->leftChild = NULL;
   node->rightChild = NULL;

   //Returns the node
   return node;
}

//Insert a (new) node to the BST
struct address_t* insertNode(struct address_t* node, struct address_t* newNode){

   //If tree is empty, set the new node as the root.
   if (node == NULL) {
      node = newNode;
      return node;
   }

   //Otherwise, recur down the tree
   if (strcmp(newNode->alias, node->alias) < 0) {
      //If string of new node is less than the (existing) node being checked, go to existing node's left child
      node->leftChild = insertNode(node->leftChild, newNode);
   } else if (strcmp(newNode->alias, node->alias) > 0) {
      //If string of new node is greater than the (existing) node being checked, go to existing node's right child
      node->rightChild = insertNode(node->rightChild, newNode);
   }

   //Return the unchanged node pointer
   return node;
}

//Add a new node to the BST by creating and inserting to tree.
struct address_t* addNodeToTree(int IPChunks[4], char *alias){
   struct address_t* newAddress = newNode(IPChunks, alias);
   root = insertNode(root, newAddress);

   return newAddress;
}

//Update an address of an existing node in the BST.
struct address_t* updateNodeInTree(int IPChunks[4], struct address_t* node, char *alias){

   // If NULL, return with no updates to tree.
   if (node == NULL){
      return NULL;
   }

   // If node found, update the address.
   if (strcmp(alias, node->alias) == 0){ 
      node->firstByte = IPChunks[0];
      node->secondByte = IPChunks[1];
      node->thirdByte = IPChunks[2];
      node->fourthByte = IPChunks[3];
      return node;
   }

   // Go to the left subtree if the alias to update IP address is smaller than the node's alias.
   if (strcmp(alias, node->alias) < 0){
      return updateNodeInTree(IPChunks, node->leftChild, alias);
   }

    // Go to the right subtree if the alias to update IP address is larger than the node's alias.
   return updateNodeInTree(IPChunks, node->rightChild, alias);
}

// Delete an alias and its IP address from the tree
struct address_t* deleteNodeFromTree(struct address_t* node, char *alias){
   // Base case - If NULL, return with no updates to tree.
   if (node == NULL){
      return NULL;
   }

   // If node found, delete the alias and its address from the BST.
   if (strcmp(alias, node->alias) == 0){

      //If node with only one child or no child
      if (node->leftChild == NULL){
         //If node's left child is NULL, get the right child.
         struct address_t *temp = node->rightChild;
         free(node);
         return temp;
      } else if (node->rightChild == NULL){
         //If node's right child is NULL, get the left child.
         struct address_t *temp = node->leftChild;
         free(node);
         return temp;
      }

      //Node with two children: Get the in-order successor (smallest in the right subtree).
      struct address_t* temp = minValueNode(node->rightChild);

      //Copy the in-order successor's content to the root node.
      node->firstByte = temp->firstByte;
      node->secondByte = temp->secondByte;
      node->thirdByte = temp->thirdByte;
      node->fourthByte = temp->fourthByte;
      strcpy(node->alias, temp->alias);

      //Delete the in-order successor
      node->rightChild = deleteNodeFromTree(node->rightChild, temp->alias);

      return node;
   }

   // Go to left subtree if alias to be deleted is smaller than the node's alias.
   if (strcmp(alias, node->alias) < 0){
      node->leftChild = deleteNodeFromTree(node->leftChild, alias);
   } // Go to right subtree if alias to be deleted is larger than the node's alias.
   else if (strcmp(alias, node->alias) > 0){
      node->rightChild = deleteNodeFromTree(node->rightChild, alias);
   } //The alias and the associated IP address to delete was not found, or some error occurred.
   else {
      return NULL;
   }
}

//Traverse through the BST (via depth-first-search) using in-order traversal.
void inOrder(struct address_t *node){
   //If node is empty, return NULL to indicate no node.
    if (node == NULL){
        return;
    }

    /* Recur on left subtree */
    inOrder(node->leftChild);

    /* Print the current node */
    printf("\nIP Address: %d.%d.%d.%d \t Address Alias: %s", node->firstByte, node->secondByte, node->thirdByte, node->fourthByte, node->alias);

    /* Recur on right subtree */
    inOrder(node->rightChild);
}

// Search an existing alias in the BST
struct address_t* BSTSearchByAlias(struct address_t* node, char *alias){

   // Base Case - if node is NULL or the alias matches the existing node's alias. 
   if (node == NULL || strcmp(alias, node->alias) == 0){
      return node;
   }

   // Go and search in the left subtree if alias is smaller than the node's alias.
   if (strcmp(alias, node->alias) < 0){
      return BSTSearchByAlias(node->leftChild, alias);
   }

   // Go and search in the right subtree if alias is larger than the node's alias.
   return BSTSearchByAlias(node->rightChild, alias);
}

// Given a non-empty binary search tree, return the node with the minimum key value found in that tree.
struct address_t* minValueNode(struct address_t* node){
   //Start the node supplied (which is often the root).
   struct address_t* current = node;

   //Loop down to find the leftmost leaf
   while (current && current->leftChild != NULL){
      current = current->leftChild;
   }

   //Return the node with the minimum key value.
   return current;
}
