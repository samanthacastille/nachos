// threadtest.cc
//
// Copyright (c) 1992-1993,2021 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include <iostream>

using namespace std;


void SimpleThread(int);

// Functions for Tasks 1 and 2
void task1(int);
char* task1GetInput(int);
char* validateInputSize(char*, int);
char* inputIdentification(char*, int);
void task2();
int task2GetInput(int);
void shouter(int);

// global flag to execute a particular task
int taskFlag;

// Constant shouting strings
const char* shout1 = "I should not have said that.";
const char* shout2 = "My father will hear about this.";
const char* shout3 = "But I AM the chosen one.";
const char* shout4 = "Follow the spiders? Why couldn't it be 'follow the butterflies'?";
const char* shout5 = "You've got the emotional range of a teaspoon.";
const char* shout6 = "I suspect Nargles are behind it.";



//----------------------------------------------------------------------
// ThreadTest
// Depending on the value of the taskFlag, which is taken in
// in the terminal command when running nachOS, it will either execute
// task 1, task 2, or print an error if the task is not specified.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering ThreadTest");

    if (taskFlag==1){
      Thread *t = new Thread("forked thread");
      t->Fork(task1, 0);
    } else if (taskFlag==2) {
      task2();
    } else {
      printf("You didn't use the -A command to choose which task to execute. \nExiting ->->->->->->\n");
    }

	  currentThread->Finish();
}


//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE FOR TASK 1
// IT INCLUDES FUNCTIONS <task1>, <task1GetInput>,
// <validateInputSize>, and <inputIdentification>.
//----------------------------------------------------------------------


// This function invokes task 1 with a thread created from the ThreadTest function.
void task1(int thread) {
  char* inputType;
  printf("Please give me an input.\n");

  inputType = task1GetInput(100);
  if (inputType=="Your input was too long. It was discarded.\nPlease rerun and keep your input under:") {
    printf("%s 99.", inputType);
  }
  else {
    printf("\nYour input was of type: %s\n\n", inputType);
  }
}


// This function recieves input from the user.
// It calls validateInputSize to validate the input size.
// If the input size is fine, it calls inputIdentification
// which returns the input type.
char* task1GetInput(int size) {
	char* input;
  char* inputType;
  input = new char[size];

  fgets(input, size, stdin);

  char* validateResult = validateInputSize(input, size);
  if (validateResult=="OK") {
    inputType = inputIdentification(input, size);
  }
  else {
    printf("\n%s\n", validateResult);
    return validateResult;
  }
  return inputType;
}


// This function validates the input size.
// If the input is empty, it asks for it again.
// If the input is too long, it discards it and tells the user to start over.
char* validateInputSize(char* input, int size) {
  bool empty = false;
  bool tooLong;
  char* tooLongInputMessage = "Your input was too long. It was discarded.\nPlease rerun and keep your input under:";
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
      return "OK";
    }
  }
  if ((input[size-1]==0) && ((input[size-2]=='\n') || (input[size-2]==0))) {
    return "OK";
  } else {
    return tooLongInputMessage;
  }
}


// This function receives a character array and identifies the input type.
// It returns a character string stating that type.
// This is a brute-force algorithm and I plan to clean it up in the future.
char* inputIdentification(char* input, int size) {
  int numDecimals = 0;

  // check for single characters, it could be an integer!
  if (input[1]=='\n'){
    if ((input[0]>='0') && (input[0]<='9')){
      return "integer";
    }
    return "character";
  }
  // being looping through character array
  for(int i=0;i<=size;i++){
    // if null terminator, return
    if (input[i]==0) {
      return "How did I get here?";
    }
    // numbers cannot have more than one decimal
    if (numDecimals>1) {
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
      if ((input[i]>='0') && (input[i]<='9')){
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
// IT INCLUDES FUNCTIONS <task2>, <task2GetInput>, and <shouter>.
//----------------------------------------------------------------------


int shout_count;


// Get input from user and calls task2GetInput.
// Create threads and call the shouter function.
void task2(){
  int threadCount;
  int shoutCount;
  char* threadInputType;
  char* shoutInputType;
  int size = 6;

  char* threadInput;
  char* shoutInput;

  printf("Enter number of threads (1-1000): ");
  threadCount = task2GetInput(size);
  if (threadCount==-1) {
    printf("\nYour input was either not a positive integer, or it was too large. Try again.\n\n");
    currentThread->Finish();
  }


  printf("\nEnter number of shouts (1-1000): ");
  shoutCount = task2GetInput(size);
  if (shoutCount==-1) {
    printf("\nYour input was either not a positive integer, or it was too large. Try again.\n\n");
    currentThread->Finish();
  }


  shout_count = shoutCount;
  for(int i=0;i<threadCount;i++){
    Thread *t = new Thread("Shouting Thread");
    t->Fork(shouter, i+1);
  }
}


// Gets user input and check if it's an integer and in the correct range.
// Returns either: the integer value correctly input from the user OR -1
// if the value entered is not valid.
int task2GetInput(int size) {
  char* input = new char[size];
  char* inputType;
  int integerInput;

  fgets(input, size, stdin);

  char* validateResult = validateInputSize(input, size);
  if (validateResult=="OK") {
    inputType = inputIdentification(input, size);
    if (inputType=="integer") {
      integerInput = atoi(input);
      if ((integerInput>0) && (integerInput<1001)) {
        return integerInput;
      } else {
        return -1;
      }
    } else {
      return -1;
    }
  }
  else {
    return -1;
  }
}


// Will decide which phrase to shout at random and keep track of # of shouts.
void shouter(int which){
  int upperBusyLoops = 7;
  int lowerBusyLoops = 3;
  int waitGoal = (Random()%((upperBusyLoops)-lowerBusyLoops))+lowerBusyLoops;
  int waitsDone = 0;
  int shoutsDone = 0;
  int upperShoutStrings = 7;
  int lowerShoutStrings = 1;
  int randomShout = (Random()%((upperShoutStrings)-lowerShoutStrings))+lowerShoutStrings;

  while(shoutsDone<shout_count) {

    while((waitsDone>=0) && (waitsDone<waitGoal)){
      waitsDone++;
      currentThread->Yield();
    }

    switch (randomShout) {
      case(1): {
        printf("Shouter %d: %s\n", which, shout1);
        randomShout = (Random()%((upperShoutStrings)-lowerShoutStrings))+lowerShoutStrings;
        break;
      }
      case(2): {
        printf("Shouter %d: %s\n", which, shout2);
        randomShout = (Random()%((upperShoutStrings)-lowerShoutStrings))+lowerShoutStrings;
        break;
      }
      case(3): {
        printf("Shouter %d: %s\n", which, shout3);
        randomShout = (Random()%((upperShoutStrings)-lowerShoutStrings))+lowerShoutStrings;
        break;
      }
      case(4): {
        printf("Shouter %d: %s\n", which, shout4);
        randomShout = (Random()%((upperShoutStrings)-lowerShoutStrings))+lowerShoutStrings;
        break;
      }
      case(5): {
        printf("Shouter %d: %s\n", which, shout5);
        randomShout = (Random()%((upperShoutStrings)-lowerShoutStrings))+lowerShoutStrings;
        break;
      }
      case(6): {
        printf("Shouter %d: %s\n", which, shout6);
        randomShout = (Random()%((upperShoutStrings)-lowerShoutStrings))+lowerShoutStrings;
        break;
      }
    }
    shoutsDone++;
    waitsDone = 0;
    waitGoal = (Random()%((upperBusyLoops)-lowerBusyLoops))+lowerBusyLoops;
  }
}

