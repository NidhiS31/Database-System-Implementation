#include "HeapFile.h"
#include <string>
#include <cstring>
#include <iostream>

//Constructor of Heap class
HeapFile :: HeapFile () {
    filePointer = new File ();
    bufferPagePointer = new Page ();
}

//Destructor of Heap class
HeapFile :: ~HeapFile () {
    delete bufferPagePointer;
    delete filePointer;
}

//Function to create a file of the heap type
int HeapFile :: Create (const char *fpath) {
    writeIndicatorFlag = true;
    bufferPageIndex = 0;

    //Create a new file at the given file path
    this->filePath = new char[100];
    strcpy (this->filePath, fpath);

    bufferPagePointer->EmptyItOut ();
    filePointer->Open (0, this->filePath);

    return 1;
}

//Function to open a Heap file
int HeapFile :: Open (char *fpath) {
    bufferPageIndex = 0;

    //Open the heap file at the given file path
    this->filePath = new char[100];
    strcpy (this->filePath, fpath);

    bufferPagePointer->EmptyItOut ();
    filePointer->Open (1, this->filePath);
    return 1;
}

//Function to load the heap file instance from the text file
void HeapFile :: Load (Schema &f_schema, const char *loadpath) {

    FILE *heapFile = fopen(loadpath, "r");
    Record recordObj;

    //If the heap File is empty, display an error message and exit
    if (heapFile == NULL) {
        cout << "Unable to open file " << loadpath << " as it is EMPTY!!" << endl;
        exit (0);
    }

    bufferPageIndex = 0;
    bufferPagePointer->EmptyItOut ();

    //While there are records in the file add it to the record object
    while (recordObj.SuckNextRecord (&f_schema, heapFile) == 1)
        Add (recordObj);

    filePointer->AddPage (bufferPagePointer, bufferPageIndex);
    bufferPagePointer->EmptyItOut ();

    //After taking every record from the file, close it
    fclose (heapFile);
}

//Function to move the pointer to the first record of the heap file
void HeapFile :: MoveFirst () {

    if (writeIndicatorFlag && bufferPagePointer->GetNumRecs () > 0) {
        filePointer->AddPage (bufferPagePointer, bufferPageIndex++);
        writeIndicatorFlag = false;
    }

    bufferPagePointer->EmptyItOut ();
    bufferPageIndex = 0;

    filePointer->GetPage (bufferPagePointer, bufferPageIndex);
}

//Function to add the next record to the end of the heap file
void HeapFile :: Add (Record &rec) {

    if (! (bufferPagePointer->Append (&rec))) {
        filePointer->AddPage (bufferPagePointer, bufferPageIndex++);

        bufferPagePointer->EmptyItOut ();
        bufferPagePointer->Append (&rec);
    }
}

//Function to fetch the next record from the page
int HeapFile :: GetNext (Record &fetchme) {

    //If record fetched successfully
    if (bufferPagePointer->GetFirst (&fetchme))
        return 1;
    else {
        // Fetch record from the immediate next page if end of file is not reached
        bufferPageIndex++;

        if (bufferPageIndex < filePointer->GetLength () - 1) {

            filePointer->GetPage (bufferPagePointer, bufferPageIndex);
            bufferPagePointer->GetFirst (&fetchme);

            return 1;
        }
        //If end of file reached
        else
            return 0;
    }
}

//Method overload of GetNext function to fetch the next record from the page
//that is accepted by the selection predicate
int HeapFile :: GetNext (Record &fetchme, CNF &cnf, Record &literal) {

    ComparisonEngine compEngObj;

    while (GetNext (fetchme)) {
        if (compEngObj.Compare (&fetchme, &literal, &cnf))
            return 1;
    }
    return 0;
}

//Function to close the heap file
int HeapFile :: Close () {

    if (writeIndicatorFlag && bufferPagePointer->GetNumRecs () > 0) {
        filePointer->AddPage (bufferPagePointer, bufferPageIndex++);

        bufferPagePointer->EmptyItOut ();
        writeIndicatorFlag = false;
    }

    filePointer->Close ();
    return 1;
}
