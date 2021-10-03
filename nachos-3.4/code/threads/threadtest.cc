// threadtest.cc
//
// Copyright (c) 1992-1993,2021 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.


// Samantha Castille - C00134220
// All of the code in this file is my own work.


#include "copyright.h"
#include "system.h"
#include "synch.h"
#include <iostream>
using namespace std;

// Functions for Project 1 -> Tasks 1 and 2
// Task 1
void identifyInput(int);
int validateInputSize(char*, int);
char* inputIdentification(char*, int);
// Task 2
void project1task2();
void forkThreads();
int randomNumber(int, int);
void shouter(int);

// Functions for Project 2 -> Tasks 1-4
void busyWaitingLoop(int);
// Task 1
void project2task1();
// Task 2
void project2task2();
// Task 3
void project2task3();
// Task 4
void project2task4();


// Functions used in multiple tasks
void invalidInput(char*);
char* getInput(bool, int, int, char*, char*);


// global flag to execute a particular task
int taskFlag;

// Constant shouting strings
const char* shout1 = "I should not have said that.";
const char* shout2 = "My father will hear about this.";
const char* shout3 = "But I AM the chosen one.";
const char* shout4 = "Follow the spiders? Why couldn't it be 'follow the butterflies'?";
const char* shout5 = "You've got the emotional range of a teaspoon.";
const char* shout6 = "I suspect Nargles are behind it.";



//------------------------------------------------------------------------------
// ThreadTest (main function)
// Depending on the value of the taskFlag, which is taken in
// in the terminal command when running nachOS, it will execute one of
// tasks 1-6 or print an error if the task is not specified properly.
//------------------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering ThreadTest");

    if (taskFlag==1){
      Thread *t = new Thread("forked thread");
      t->Fork(identifyInput, 0);
    } else if (taskFlag==2) {
      project1task2();
    } else if (taskFlag==3) {
      project2task1();
    } else if (taskFlag==4) {
      project2task2();
    } else if (taskFlag==5) {
      project2task3();
    } else if (taskFlag==6) {
      project2task4();
    } else {
      printf("You didn't use the -A command, or you didn't use it correctly.\n");
      printf("Please insert '-A' followed by which task you'd like to execute (1/2/3/4/5/6).\n");
      printf("Exiting ->->->->->->->->->->->->->->->->->->->->->->->->->->->\n\n\n\n");
    }

	  currentThread->Finish();
}

// -----------------------------------------------------------------------------
// Functions used by multiple tasks
// -----------------------------------------------------------------------------

// Prints error if input was invalid, and then ends the current thread to exit the program
void invalidInput(char* invalidInputMessage) {
  printf("%s\n", invalidInputMessage);
  currentThread->Finish();
}

// Gets user input and check if it's an integer and in the correct range.
// Returns either: the integer value correctly input from the user OR -1
// if the value entered is not valid.
char* getInput(bool intRequired, int maxInputSize, int maxIntegerSize, char* inputRequest, char* invalidInputMessage) {
  char* input = new char[maxInputSize];
  char* inputType;
  int integerInput;

  printf("%s\n", inputRequest);

  fgets(input, maxInputSize, stdin);

  int validateResult = validateInputSize(input, maxInputSize);
  if (validateResult==1) {
    inputType = inputIdentification(input, maxInputSize);
    if (inputType=="integer") {
      if (intRequired==true) {
        integerInput = atoi(input);
        if ((integerInput>0) && (integerInput<maxIntegerSize)) {
          return input;
        } else {
          invalidInput(invalidInputMessage);
        }
      } else {
        return inputType;
      }
    } else {
      if (intRequired==true) {
        invalidInput(invalidInputMessage);
      } else {
        return inputType;
      }
    }
  } else {
    invalidInput(invalidInputMessage);
  }
}

//----------------------------------------------------------------------
// THIS IS THE START OF CODE FOR TASK 1
// IT INCLUDES FUNCTIONS <identifyInput>,
// <validateInputSize>, and <inputIdentification>.
//----------------------------------------------------------------------


const int maxInputSizeTask1 = 256;

// This function invokes task 1 with a thread created from the ThreadTest function.
void identifyInput(int thread) {

  char* inputString;
  char* result;
  char* inputRequestMessage = "Please give me an input.\n";
  char* task1InvalidInputMessage = "Your input was too long. It was discarded.\nPlease rerun and keep your input under 256 characters ";

  result = getInput(false, maxInputSizeTask1, 0, inputRequestMessage, task1InvalidInputMessage);

  printf("\nYour input was of type: %s\n\n", result);
}


// This function validates the input size.
// If the input is empty, it asks for it again.
// If the input is too long, it returns -1, otherwise it returns 1
int validateInputSize(char* input, int size) {
  bool empty = false;
  bool tooLong;
  if (input[0]=='\n'){
    empty = true;
  }
  while (empty) {
    printf("You didn't input anything. Try again!\n\n");
    printf("Please give me an input.\n");
    fgets(input, size, stdin);
    if (input[0]=='\n'){
      empty = true;
    } else {
      empty = false;
      return 1;
    }
  }
  if ((input[size-1]==0) && ((input[size-2]=='\n') || (input[size-2]==0))) {
    return 1;
  } else {
    return -1;
  }
}

// This function receives a character array and identifies the input type.
// It returns a character string stating that type.
char* inputIdentification(char* input, int size) {
  int numDecimals = 0;

  // check for single characters, it could be an integer!
  if (input[1]=='\n'){
    if ((input[0]>='0') && (input[0]<='9')){
      return "integer";
    }
    return "character";
  }
  // begin looping through character array
  for(int i=0;i<=size;i++){
    // if null terminator, return
    if ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z') || input[i]==0 || numDecimals>1) {
      return "character string";
    }
    // keep track of # of decimals and check placement
    if (input[i]=='.') {
      if ((i>0) && (input[i+1]!='\n') && (input[i-1]!='-')){
        numDecimals++;
      } else return "character string";
    }
    // special characters mean this is a character string
    else if (!((input[i]>='0') && (input[i]<='9')) && !((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z')) && !(input[i]=='-')) {
      return "character string";
    }
    // check for negative numbers
    else if ((input[i]=='-') && i>0) {
      return "character string";
    }
    else if (input[0]=='-') {
      if ((input[i]>='0') && (input[i]<='9')) {
        if (input[i+1]=='\n') {
          if ((numDecimals==0)){
            return "negative integer";
          } else if (numDecimals==1) {
            return "negative decimal";
          }
          return "negative integer";
        }
      }
    }
    // this is a number
    else if ((input[i]>='0') && (input[i]<='9')) {
      if (input[i+1]=='\n') {
        if (numDecimals==0){
          return "integer";
        } else if (numDecimals==1){
          return "decimal";
        }
      }
      else continue;
    }
    // if it includes any letter, this is a character string
    else if ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z')) {
      return "character string";
    }
    else return "character string";
  }
}




//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE FOR TASK 2
// IT INCLUDES FUNCTIONS <project1task2>,
// <forkThreads>, <randomNumber>, and <shouter>.
//----------------------------------------------------------------------


int globalShoutCount;
int globalThreadCount;
int maxIntegerSizeTask2 = 1001;
const int maxInputSizeTask2 = 6;


// Get input from user and calls getInput.
// Create threads and call the shouter function.
void project1task2(){
  char* threadCount;
  char* shoutCount;
  char* threadInputType;
  char* shoutInputType;
  char* threadInput;
  char* shoutInput;
  char* invalidInputMessage = "\nYour input was either not a positive integer, or it was too large. Try again.\n\n";
  char* threadRequstMessage = "Enter number of threads (1-1000): ";
  char* shoutRequestMessage = "\nEnter number of shouts (1-1000): ";

  threadCount = getInput(true, maxInputSizeTask2, maxIntegerSizeTask2, threadRequstMessage, invalidInputMessage);
  globalThreadCount = atoi(threadCount);

  shoutCount = getInput(true, maxInputSizeTask2, maxIntegerSizeTask2, shoutRequestMessage, invalidInputMessage);
  globalShoutCount = atoi(shoutCount);

  forkThreads();
}


// Forks specified number of threads to do the shouting
void forkThreads() {
  for(int i=0;i<globalThreadCount;i++){
    Thread *t = new Thread("Shouting Thread");
    t->Fork(shouter, i+1);
  }
}

// Generates a random number in a specific range
int randomNumber(int upperLimit, int lowerLimit) {
  int random = (Random()%((upperLimit)-lowerLimit))+lowerLimit;
}

// Will decide thread phrase to shout at random and keep track of # of shouts.
void shouter(int thread){
  int waitsDone = 0;
  int shoutsDone = 0;
  int upperBusyLoops = 7;
  int lowerBusyLoops = 3;
  int waitGoal = randomNumber(upperBusyLoops, lowerBusyLoops);
  int upperShoutStrings = 7;
  int lowerShoutStrings = 1;
  int randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);

  while(shoutsDone<globalShoutCount) {
    while((waitsDone>=0) && (waitsDone<waitGoal)){
      waitsDone++;
      currentThread->Yield();
    }
    switch (randomShout) {
      case(1): {
        printf("Shouter %d: %s\n", thread, shout1);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(2): {
        printf("Shouter %d: %s\n", thread, shout2);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(3): {
        printf("Shouter %d: %s\n", thread, shout3);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(4): {
        printf("Shouter %d: %s\n", thread, shout4);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(5): {
        printf("Shouter %d: %s\n", thread, shout5);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
      case(6): {
        printf("Shouter %d: %s\n", thread, shout6);
        randomShout = randomNumber(upperShoutStrings, lowerShoutStrings);
        break;
      }
    }
    shoutsDone++;
    waitsDone = 0;
    waitGoal = randomNumber(upperBusyLoops, lowerBusyLoops);
  }
}


//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE FOR TASK 3
// IT INCLUDES FUNCTIONS <project2task1>
//----------------------------------------------------------------------

int maxInputSizeProject2 = 5;
int maxIntegerSizeProject2 = 256;
char* philospherRequestMessage = "\nPlease input the number of philosophers you would like.\n";
char* mealsRequestMessage = "\nPlease input the number of meals you would like.\n";
char* invalidInputMessage = "\nYou didn't input properly. Please input an integer greater than zero and less than 256.\n\n";


void project2task1() {
  printf("You have selected task 3. Congrats.\n");
  char* inputString;
  int philosophers, meals;
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, philospherRequestMessage, invalidInputMessage);
  philosophers = atoi(inputString);
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, mealsRequestMessage, invalidInputMessage);
  meals = atoi(inputString);
}


//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE FOR TASK 4
// IT INCLUDES FUNCTIONS <project2task2>
//----------------------------------------------------------------------

void project2task2() {
  printf("You have selected task 4. Congrats.\n");
  char* inputString;

  int philosophers, meals;
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, philospherRequestMessage, invalidInputMessage);
  philosophers = atoi(inputString);
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, mealsRequestMessage, invalidInputMessage);
  meals = atoi(inputString);
}


//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE FOR TASK 5
// IT INCLUDES FUNCTIONS <project2task3>
//----------------------------------------------------------------------

void project2task3() {
  printf("You have selected task 5. Congrats.\n");

  char* inputString;
  char* peopleRequestMessage = "\nPlease input the number of people: \n";
  char* storageRequestMessage = "\nPlease input the amount of storage each person has: \n";
  char* numMessagesRequestMessage = "\nPlease input the number of total messages: \n";

  int people, storage, numMessages;
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, peopleRequestMessage, invalidInputMessage);
  people = atoi(inputString);
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, storageRequestMessage, invalidInputMessage);
  storage = atoi(inputString);
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, numMessagesRequestMessage, invalidInputMessage);
  numMessages = atoi(inputString);

}


//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE FOR TASK 6
// IT INCLUDES FUNCTIONS <project2task4>
//----------------------------------------------------------------------

void project2task4() {
  printf("You have selected task 6. Congrats.\n");

  char* inputString;
  char* readersRequestMessage = "\nPlease input the number of readers: \n";
  char* writersRequestMessage = "\nPlease input the number of writers: \n";
  char* maxReadersRequestMessage = "\nPlease input the maximum number of readers at a time: \n";

  int readers, writers, maxReaders;
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, readersRequestMessage, invalidInputMessage);
  readers = atoi(inputString);
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, writersRequestMessage, invalidInputMessage);
  writers = atoi(inputString);
  inputString = getInput(true, maxInputSizeProject2, maxIntegerSizeProject2, maxReadersRequestMessage, invalidInputMessage);
  maxReaders = atoi(inputString);

}
