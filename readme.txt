								Storage Manager
							-------------------------------

CONTENTS OF THE FILE
----------------------
* Introduction
* Technologies
* Source files
* Functions
* Execution

INTRODUCTION
-------------

Storage Manager is capable of creating, opening and closing a file including some more operatins put below. 
It can be used to Read blocks from a file on to the disk such as reading first block, previous block, current block, last block and next block.
And also can write blocks from memory to a file on disk. This storage manager deals with fixed size blocks.

To visit the implementation of storage manager:
https://bitbucket.org/shek21/group7/src/master/

TECHNOLOGIES
--------------
Language used to implement storage manager is 'C'

SOURCE FILES
--------------
Below are the list of files needed:
C files : storage_mgr.c, db_error.c, test_assign1_1.c
header files : storage_mgr.h, dberror.h, test_helper.h
Make file

storage_mgr.c:

storage_mgr.c contains most implementation of storage manager. All the required functions such as createPageFile, openPageFile,
closePageFile, destroyPageFile, readBlock, getBlockPos, readFirstBlock, readLastBlock, readPreviousBlock, readCurrentBlock,
readNextBlock, writeBlock, writeCurrentBlock, appendEmptyBlock, ensureCapacity defined in the programming assignment are in this file with two data structures 
SM_FileHandle and SM_PageHandle. 

storage_mgr.h:

In this file the two data structures are defined and all the functions to manipulate the files, read blocks from disc, 
writing blocks to a page file are declared.

db_error.c:

This file contains the description of the codes which need to be implemented for both success and error cases.

db_error.h:

In this file block size is defined and all the error codes are defined. 
All the functions will return the integer code defined in the error codes. These values indicates whether an operation is successful.

test_assign1_1.c:

This file contains the test cases which covers all the functions that are implemented.

Make file:

Make file is an executable file that includes all the source files that are to be included.
Also this utility is used to automatically update and create one executable whenever any of the file in the directory is modified.

FUNCTIONS
----------

initStorageManager(void):
This function initialiizes the storage manager.

createPageFile(char * fileName):
This function has one parameter fileName. It creates or overwrites the file in write mode. While creating the file if there is an error it exits and returns error codes based on the codes defined in the error file.
Otherwise, it creates a file with one single page and data with zero bytes. 
If it is successful, it returns the success code RC_OK defined in db_error.h

openPageFile (char *fileName, SM_FileHandle *fHandle):
This function is defined with two parameters filename and a struct. It tries to open the file in read+write mode. If the file not found, it returns 
the error code RC_FILE_NOT_FOUND that file is not found. Otherwise, it moves to end of the file and all the file details will be added to the 
struct SM_FileHandle. It reads the total number of pages in the file and assign it to totalNumPages. Store book keeping information in mgmtInfo
such as address of the file pointer.
If the above process is success, it returns the success code RC_OK

closePageFile (SM_FileHandle *fHandle):
To close the file, first get the file pointer stored in struct and close the file using the fclose() function.
If it is success, returns the success code RC_OK.

destroyPageFile (char *fileName):
Open the file using filename in read mode and destroy the file using remove() function. If the file is destroyed successfully, returns RC_OK.
else returns RC_FILE_NOT_FOUND.

readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage):
Check if the file is less than pageNum then return code RC_READ_NON_EXISTING_PAGE and also check if the file is not present then 
return RC_FILE_NOT_FOUND. Define the offset and made it as file position. Now read the block data using fread() and store it in the memPage struct.
If it is less than the PAGE_SIZE, return RC_READ_NON_EXISTING_PAGE and find out the current page position using ftell() and
store it in curPagePos for future use.
Returns success code, if the block is read correctly.

getBlockPos (SM_FileHandle *fHandle):
By using fhandle and curPagePos, returns current page position of the file.

readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage):
If the address of the file pointer not found, return RC_FILE_NOT_FOUND. 
Call readBlock() function, move to beginning of the file, read the first block into memPage.

readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage):
Check for any errors related to RC_FILE_NOT_FOUND and RC_READ_NON_EXISTING_PAGE. 
Then define the offset as previous block position i.e; curPagePos-1 and call readBlock() function at offset and set the current page position 
to curPagePos using ftell().
Returns success code, if the block is read correctly.

readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage):
Check if file exists and invalid page number then return the codes RC_FILE_NOT_FOUND and RC_READ_NON_EXISTING_PAGE. 
Set the offset to the current page position and call readBlock() function at offset value.
Return RC_OK, if the block is read correctly.

readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage):
Check if file exists and invalid page number then return the codes RC_FILE_NOT_FOUND and RC_READ_NON_EXISTING_PAGE. 
Make the offset as next block position i.e; curPagePos+1 and call readBlock() function at offset and set the current page position 
to curPagePos using ftell().
Returns success code, if the block is read correctly.

readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage):
Check if the file is present or not, return RC_FILE_NOT_FOUND. 
Set the offset value to total number of pages - 1 and call readBlock() at offset and set the current page position 
to curPagePos using ftell().
Returns success code, if the block is read correctly.

writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage):
Check if the file is present or not, return RC_FILE_NOT_FOUND. Open the file in read+write mode and check if the write output 
with the page is not 1 then return code RC_WRITE_FAILED. Define offset and seek to the particular page and set the 
current page position to curPagePos using ftell() then use fseek() again using new value as offset and close the file.
Return RC_OK, if it is success.

writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage):
Check if the file is present or not, return RC_FILE_NOT_FOUND. Open the file in read+write mode and writes the data from the 
buffer to the file.If output is -1 then its writing has failed. Set the offset value to the current pointer of block. Check the 
pointer and write the data to the file and update current page position accordingly and close the file.
Return RC_OK, if it is success.

appendEmptyBlock (SM_FileHandle *fHandle):
Check if the file is present or not, return RC_FILE_NOT_FOUND. Close the file and open in read+write mode.
Move to the end of the file and add the data to the page. Find out the total number of pages in the file and close the file.
Return RC_OK, if it is success.

ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
Try to open the file in read mode and Check if the file is present or not, return RC_FILE_NOT_FOUND.
Set total number of pages in the file as offset. Check if total number of pages is less than number of pages and 
offset is not equal to number of pages then call appendEmptyBlock() function to add data to block.
Close the file and return RC_OK, if it is success.

EXECUTION
----------

make file should be available.
- Open the commandline
- Make sure you are in the correct directory
- Run the below command:
	make

