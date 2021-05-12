#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

// test name
char *testName;

/* test output files */
#define TESTPF "test_pagefile.bin"

/* prototypes for test functions */
static void testCreateOpenClose(void);
static void testSinglePageContent(void);
static void testMultiplePageContent(void);

/* main function running all tests */
int main (void)
{
  testName = "";
  
  initStorageManager();
  testCreateOpenClose();
  testSinglePageContent();
  testMultiplePageContent();

  return 0;
}


/* check a return code. If it is not RC_OK then output a message, error description, and exit */
/* Try to create, open, and close a page file */
void testCreateOpenClose(void)
{
  SM_FileHandle fh;

  testName = "test create open and close methods";

  TEST_CHECK(createPageFile (TESTPF));
  
  TEST_CHECK(openPageFile (TESTPF, &fh));
  ASSERT_TRUE(strcmp(fh.fileName, TESTPF) == 0, "filename correct");
  ASSERT_TRUE((fh.totalNumPages == 1), "expect 1 page in new file");
  ASSERT_TRUE((fh.curPagePos == 0), "freshly opened file's page position should be 0");

  TEST_CHECK(closePageFile (&fh));
  TEST_CHECK(destroyPageFile (TESTPF));

  // after destruction trying to open the file should cause an error
  ASSERT_TRUE((openPageFile(TESTPF, &fh) != RC_OK), "opening non-existing file should return an error.");

  TEST_DONE();
}

/* Try to create, open, and close a page file */
void testSinglePageContent(void)
{
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i;

  testName = "test single page content";

  // allocate memory for a page
  ph = (SM_PageHandle) malloc(PAGE_SIZE);

  // create a new page file
  TEST_CHECK(createPageFile (TESTPF));
  TEST_CHECK(openPageFile (TESTPF, &fh));
  printf("created and opened file\n");
  
  // read first page into handle
  TEST_CHECK(readFirstBlock (&fh, ph));
  // the page should be empty (zero bytes)
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
  printf("first block was empty\n");
    
  // change ph to be a string and write that one to disk
  for (i=0; i < PAGE_SIZE; i++)
    ph[i] = (i % 10) + '0';
  TEST_CHECK(writeBlock (0, &fh, ph));
  printf("writing first block\n");

  // read back the page containing the string and check that it is correct
  TEST_CHECK(readFirstBlock (&fh, ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  printf("reading first block\n");

  // destroy new page file
  TEST_CHECK(closePageFile (&fh));
  TEST_CHECK(destroyPageFile (TESTPF));  

  // free page memory
  free(ph);
  
  TEST_DONE();
}

static void testMultiplePageContent(void)
{
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i;

  testName = "testing multiple pages";
  printf("MULTIPLE PAGES TEST:\n");

  // allocate memory for a page
  ph = (SM_PageHandle) malloc(PAGE_SIZE);

  // create a new page file
  TEST_CHECK(createPageFile (TESTPF));

  // open the file
  TEST_CHECK(openPageFile (TESTPF, &fh));
  printf("\nCreated and opened file\n");
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages); // curPagePos = 0, totalNumPages = 1
  printf("Next test: Call readFirstBlock() to check if the first byte is byte zero\n");
  printf("Press enter to continue\n");
  getchar();
  
  TEST_CHECK(readFirstBlock (&fh, ph));
  // Check if the first block is filled with zero byte
  for (i=0; i < PAGE_SIZE; i++)
  {
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
  }
  printf("\nSuccess! First block was filled with zero byte as expected\n");
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages); // curPagePos = 1, totalNumPages = 1
  printf("Next test: Call writeBlock() on pageNum: 0 to fill it with string, then read it with readFirstBlock() to verify\n");
  printf("Press enter to continue\n");
  getchar();

  // Fill ph with a string
  for (i=0; i < PAGE_SIZE; i++)
    ph[i] = (i % 10) + '0';
  
  // Write ph into the disk
  printf("writing first block\n");
  TEST_CHECK(writeBlock (0, &fh, ph));

  // read back the page containing the string and check if that it is correct
  printf("reading the block that we wrote (first block)\n");
  TEST_CHECK(readFirstBlock (&fh, ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  printf("\nSuccess! First block was filled with the string as expected\n");
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages); // curPagePos = 1, totalNumPages = 1
  printf("Next test: Call appendEmptyBlock() to add empty block at the end of the file, then call readNextBlock() to verify appended block contains byte zero\n");
  printf("Press enter to continue\n");
  getchar();

  TEST_CHECK(appendEmptyBlock(&fh));
  printf("Appended zero byte at the end\n");
  // curPagePos = 1, totalNumPages = 2
  

  // Set page position to 0 (Beginning) and test function readNextBlock()
  fh.curPagePos = 0;  // curPagePos = 0, totalNumPages = 2
  printf("Reading Appended block\n");
  
  TEST_CHECK(readNextBlock (&fh, ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in freshly created page");
  // curPagepos should be 1 now
  printf("\nSuccess! Last block was filled with zero byte as expected\n");
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages); // curPagePos = 2, totalNumPages = 2
  printf("Next test: Call appendEmptyBlock() to add another empty block at the end of the file\n");
  printf("Press enter to continue\n");
  getchar();
  
  // Append another empty block
  printf("Appending zero byte\n");
  TEST_CHECK(appendEmptyBlock(&fh));
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages); // curPagePos = 2, totalNumPages = 3
  printf("Next test: Write a string at block 2 using writeCurrentBlock(), then read it using readPreviousBlock()\n");
  printf("Press enter to continue\n");
  getchar();

  // Write a string at page 1 using writeCurrentBlock()
  printf("Write Current block\n");
  for (i=0; i < PAGE_SIZE; i++)
      ph[i] = '0';
  TEST_CHECK(writeCurrentBlock(&fh,ph));
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages); // curPagePos = 3, totalNumPages = 3
  
  printf("Read Previous block\n");
  TEST_CHECK(readPreviousBlock(&fh, ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == '0'), "character in page read from disk is the one we expected.");
  printf("\nSuccess! The block was filled with the string we expected\n");
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages); // curPagePos = 3, totalNumPages = 3
  printf("Next test: Append another empty block using appendEmptyBlock()\n");
  printf("Press enter to continue\n");
  getchar();
  
  printf("Appending Empty block\n");
  TEST_CHECK(appendEmptyBlock(&fh));
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages); // curPagePos = 3, totalNumPages = 4
  printf("Next test: Call readCurrentBlock() to read the last byte, which should be zero byte\n");
  printf("Press enter to continue\n");
  getchar();
  
  TEST_CHECK(readCurrentBlock(&fh, ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in freshly created page");
  printf("\nSuccess! The current block (last) was filled with zero byte as expected\n");
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages); // curPagePos = 4, totalNumPages = 4
  printf("Next test: Call appendEmptyBlock()\n");
  printf("Press enter to continue\n");
  getchar();

  printf("Appending zero byte\n");
  TEST_CHECK(appendEmptyBlock(&fh)); // curPagePos = 4, totalNumPages = 5
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages);
  // Set curPagePos
  printf("Next test: Call writeCurrentBlock() to fill last block with a string, then call readLastBlock() to verify\n");
  printf("Press enter to continue\n");
  getchar();

  // Fill ph with a string
  for (i=0; i < PAGE_SIZE; i++)
    ph[i] = '1';
  
  // Write ph into the disk
  printf("writing string in the last block\n");
  TEST_CHECK(writeCurrentBlock(&fh,ph));

  // Read last block to verify if ph contains the string
  TEST_CHECK(readLastBlock(&fh, ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == '1'), "character in page read from disk is the one we expected.");
  printf("\nSuccess! Last block was filled with the string as expected\n");
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages); // curPagePos = 4, totalNumPages = 4
  printf("Next test: Call ensureCapacity() with numberOfPages = fh.totalNumPages + 1, to add a block with zero byte\n");
  printf("Press enter to continue\n");
  getchar();
  
  TEST_CHECK(ensureCapacity(fh.totalNumPages + 1, &fh));
  printf("\nSuccess! Last block was filled with the string as expected\n");
  printf("Current status: curPagePos: %d\ttotalNumPages %d\n\n", fh.curPagePos, fh.totalNumPages); // curPagePos = 4, totalNumPages = 4
  printf("Next test: Call getBlockPos() and compare the value returned with curPagePos\n");
  printf("Press enter to continue\n");
  getchar();
 
  // Get current position from the function
  i = getBlockPos(&fh);
  if (i == fh.curPagePos)
    printf("\nSuccess! value returned matches with the curPagePos\n");
  else
    printf("\nFailure! value returned does not matches with the curPagePos\n");
  
  // destroy new page file
  TEST_CHECK(destroyPageFile (TESTPF));  
  
  // End of test
  TEST_DONE();
}