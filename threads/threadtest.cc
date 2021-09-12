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
char* inputIdentification(char*);
char* validateInputSize(char*);
void getInput(int);

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering ThreadTest");

    Thread *t = new Thread("forked thread");
    t->Fork(getInput, 1);

	  currentThread->Finish();
}


//----------------------------------------------------------------------
// THIS IS THE START OF SAMANTHA CASTILLE'S CODE
// IT INCLUDES FUNCTIONS <getInput>,
// <validateInputSize>, and <inputIdentification>.
//----------------------------------------------------------------------



// This function recieves input from the user.
// It calls validateInputSize to validate the input size.
// If the input size is fine, it calls inputIdentification
// which returns the input type.
void getInput(int thread) {
	char* input;
  input = new char[100];

	printf("Please give me an input.\n");
  fgets(input, 100, stdin);

  char* validate = validateInputSize(input);
  if (validate=="OK") {
    char* inputType = inputIdentification(input);
    printf("\nYour input was: %sThis is of type: %s\n\n", input, inputType);
  }
  else {
    printf("\n%s\n", validate);
  }
  // deallocate the memory for the input char array
  delete input;
}



// This function validates the input size.
// If the input is empty, it asks for it again.
// If the input is too long, it discards it and tells the user to start over.
char* validateInputSize(char* input) {
  bool empty;
  bool tooLong;
  if (input[0]=='\n'){
    empty = true;
  }
  while (empty) {
    printf("You didn't input anything. Try again!\n\n");
    printf("Please give me an input.\n");
    fgets(input, 100, stdin);
    if (input[0]=='\n'){
      empty = true;
    } else {
      empty = false;
      return "OK";
    }
  }
  if ((input[99]==0) && ((input[98]=='\n') || (input[98]==0))) {
    return "OK";
  } else {
    return "The input was too long. Your input was discarded.\nPlease rerun and keep it under 99 characters.\n\n";
  }
}



// This function receives a character array and identifies the input type.
// It returns a character string stating that type.
// This is a brute-force algorithm and I plan to clean it up in the future.
char* inputIdentification(char* input) {
  int numDecimals = 0;

  // check for single characters, it could be an integer!
  if (input[1]=='\n'){
    if ((input[0]>='0') && (input[0]<='9')){
      return "integer";
    }
    return "character";
  }

  bool null = false;
  // being looping through character array
  for(int i=0;i<=100;i++){
    // if null terminator, return
    if (input[i]==0) {
      break;
    }
    // numbers cannot have more than one decimal
    if (numDecimals>1) {
      return "character string";
    }
    // keep track of # of decimals and check placement
    if (input[i]=='.') {
      if ((i>0) && (input[i+1]!='\n') && (input[i-1]!='-')){
        numDecimals++;
        continue;
      } else return "character string";
    }
    // check for negative numbers
    else if ((input[i]=='-') && i>0) {
      return "character string";
    }
    else if (input[0]=='-') {
      if (i==1) {
        continue;
      }
      if ((input[i]>='0') && (input[i]<='9')){
        if ((input[i+1]!='\n')) {
          if ((numDecimals==0)){
            return "negative integer";
          } else if (numDecimals==1) {
            return "negative decimal";
          }
          return "negative integer";
        }
        continue;
      }
      continue;
    }
    // special characters mean this is a character string
    else if (!((input[i]>='0') && (input[i]<='9')) && !((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z')) && !(input[i]=='-')) {
      return "character string";
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

