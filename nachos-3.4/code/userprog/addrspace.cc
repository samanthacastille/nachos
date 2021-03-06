// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------
// taslk
static void
SwapHeader(NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable, char *filename, int thread_id)
{
	NoffHeader noffH;
	unsigned int i, size;
	// Code additions by Ethan Bruce
	// Copy the executable file we passed into parameter executable into the executableFile field
	// We use this parameter instead of argv + 1 because we don't know whether
	// this addrspace is being created for the initial program or because of a program's exec call.
	executableFile = fileSystem->Open(filename);

	executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
	if ((noffH.noffMagic != NOFFMAGIC) && (WordToHost(noffH.noffMagic) == NOFFMAGIC))
	{
		SwapHeader(&noffH);
	}

	if (noffH.noffMagic == NOFFMAGIC)
	{

		// how big is address space?
		size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize; // we need to increase the size
																							  // to leave room for the stack

		numPages = divRoundUp(size, PageSize);
		size = numPages * PageSize;

		// Code changes by Ethan Bruce
		swapFileName = new char[100];
		sprintf(swapFileName, "%d.swap", threadID);
		bool success = fileSystem->Create(swapFileName, size); // create thread swapfile
		if (!success)
		{
			// handle error with swapfile creation
		}

		OpenFile *swapFile = fileSystem->Open(swapFileName); // open swapfile to use.

		if (swapFile == NULL)
		{
			// handle error withg swapfile opening
		}

		// Copy contents of executable into swapfile.
		char *buffer = new char[size];
		executable->ReadAt(buffer, size, sizeof(noffH));
		swapFile->WriteAt(buffer, size, 0);
		delete[] buffer;
		delete swapFile;

		// code changes by Samantha Castille
		printf("bitmap BEFORE allocation\n");
		memoryBitMap->Print();
		if (numPages > NumPhysPages)
		{
			printf("\nThis program is too large to run until we have virtual memory.\n");
			printf("\nExiting ----------------->\n");
			currentThread->killNewChild = true;
			return;
		} // check we're not trying
		// to run anything too big --
		// at least until we have
		// virtual memory
		if (numPages > memoryBitMap->NumClear())
		{
			printf("\nThere isn't enough room left in physical memory for this program.\n");
			printf("\nExiting ----------------->\n");
			currentThread->killNewChild = true;
			return;
		}

		DEBUG('a', "Initializing address space, num pages %d, size %d\n",
			  numPages, size);
		// first, set up the translation
		pageTable = new TranslationEntry[numPages];
		int start_physicalPageIndex;
		for (i = 0; i < numPages; i++)
		{
			//int freePhysicalPage = memoryBitMap->Find();
			//if(!i) start_physicalPageIndex=freePhysicalPage;
			pageTable[i].virtualPage = i; // for now, virtual page # = phys page #
										  //	pageTable[i].physicalPage = freePhysicalPage;
			pageTable[i].valid = FALSE;	  // Set valid bit to false because we are not loading any content into memory
			pageTable[i].use = FALSE;
			pageTable[i].dirty = FALSE;
			pageTable[i].readOnly = FALSE; // if the code segment was entirely on
										   // a separate page, we could set its
										   // pages to be read-only
		// end code by Samantha Castille
	}
	}
}

// code by Samantha Castille
void AddrSpace::copyIntoMemory(int badVPage, int freePhysicalPage)
{
	executableFile->ReadAt(&(machine->mainMemory[freePhysicalPage * PageSize]), PageSize,
					   badVPage * PageSize);
	pageTable[badVPage].valid = TRUE;
	pageTable[badVPage].physicalPage = freePhysicalPage;
}
// end code by Samantha Castille

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
	delete pageTable;
	// Code changes by Ethan Bruce

	// Delete the swapfile.
	fileSystem->Remove(swapFileName);
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void AddrSpace::InitRegisters()
{
	int i;

	for (i = 0; i < NumTotalRegs; i++)
		machine->WriteRegister(i, 0);

	// Initial program counter -- must be location of "Start"
	machine->WriteRegister(PCReg, 0);

	// Need to also tell MIPS where next instruction is, because
	// of branch delay possibility
	machine->WriteRegister(NextPCReg, 4);

	// Set the stack register to the end of the address space, where we
	// allocated the stack; but subtract off a bit, to make sure we don't
	// accidentally reference off the end!
	machine->WriteRegister(StackReg, numPages * PageSize - 16);
	DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
	machine->pageTable = pageTable;
	machine->pageTableSize = numPages;
}
