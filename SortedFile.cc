#include "SortedFile.h"
#include "HeapFile.h"
#include <string>
#include <cstring>
#include <iostream>

//Constructor for SortedFile class
SortedFile :: SortedFile (OrderMaker *order, int runLength) {
    this->sortedFileOrder = order;
    this->lengthOfRun = runLength;
    this->pageBufferSize = 100;
    this->queryOrder = NULL;
    this->bigq = NULL;
    this->filePointer = new File ();
    this->bufferPagePointer = new Page ();
}

//Destructor for sortedFile class
SortedFile :: ~SortedFile () {
    delete queryOrder;
    delete filePointer;
    delete bufferPagePointer;
}

//Function to create the file type Sorted file
int SortedFile :: Create (const char *fpath) {
    writeIndicatorFlag = false;
    bufferPageIndex = 0;

    this->filePath = new char[100];
    strcpy (this->filePath, fpath);

    bufferPagePointer->EmptyItOut ();
    filePointer->Open (0, this->filePath);
    return 1;
}

//Function to open the SORTED file
int SortedFile :: Open (char *fpath) {
    writeIndicatorFlag = false;
    bufferPageIndex = 0;

    this->filePath = new char[100];
    strcpy (this->filePath, fpath);

    bufferPagePointer->EmptyItOut ();
    filePointer->Open (1, this->filePath);

    if (filePointer->GetLength () > 0)
        filePointer->GetPage (bufferPagePointer, bufferPageIndex);

    return 1;
}

//Function to close the SORTED file
int SortedFile :: Close () {
    if (writeIndicatorFlag)
        InternalMerge();
    filePointer->Close ();
}

//Function to add records in the SORTED file
void SortedFile :: Add (Record &addme) {

    if (!writeIndicatorFlag)
        InitializeBigQ();
    inputPipe->Insert (&addme);
}

//Function to load the schema in the SORTED file type
void SortedFile :: Load (Schema &myschema, const char *loadpath) {

    FILE *sortedFile = fopen(loadpath, "r");
    Record recordsObj;
    bufferPageIndex = 0;

    if (sortedFile == NULL) {
        cout << "Unable to open " << loadpath << " as it is EMPTY!!" << endl;
        exit (0);
    }

    //First empty the page buffer
    bufferPagePointer->EmptyItOut ();

    //While there are records present in the file suck the next record and add it to record object
    while (recordsObj.SuckNextRecord (&myschema, sortedFile)) {
        Add (recordsObj);
    }

    //After all tge records are fetched, close the file
    fclose (sortedFile);
}

//Function to move the pointer to the first record
void SortedFile :: MoveFirst () {
    if (writeIndicatorFlag)
        InternalMerge();
    else {
        bufferPageIndex = 0;
        bufferPagePointer->EmptyItOut ();

        //If the file is not empty, get next page
        if (filePointer->GetLength () > 0)
            filePointer->GetPage (bufferPagePointer, bufferPageIndex);

        //If queryOrder is not empty, delete it
        if (queryOrder)
            delete queryOrder;
    }
}

//function to fetch next record
int SortedFile :: GetNext (Record &fetchme) {
    if (writeIndicatorFlag)
        InternalMerge();

    //return 1 on successful fetching of records
    if (bufferPagePointer->GetFirst (&fetchme))
        return 1;
    else {
        // If record is not present in current page, fetch it from the next page
        bufferPageIndex++;
        if (bufferPageIndex < filePointer->GetLength () - 1) {
            // if end of file not reached
            filePointer->GetPage (bufferPagePointer, bufferPageIndex);
            bufferPagePointer->GetFirst (&fetchme);
            return 1;
        }
        // if end of file is reached
        else
            return 0;
    }
}

//Method overload of GetNext function to fetch the next record from the page
//that is accepted by the selection predicate
int SortedFile :: GetNext (Record &fetchme, CNF &cnf, Record &literal) {

    if (writeIndicatorFlag)
        InternalMerge();

    //If query does not exist
    if (!queryOrder) {
        queryOrder = new OrderMaker;

        if (QueryOrderGen (*queryOrder, *sortedFileOrder, cnf) > 0) {
            queryOrder->Print ();
            if (SearchPerBinary(fetchme, cnf, literal))
                return 1;
            else
                return 0;

        } else
            return fetchNext(fetchme, cnf, literal);
    } else {
        if (queryOrder->numOfAttributes == 0)
            return fetchNext(fetchme, cnf, literal);
        else
            return fetchNextPerQuery(fetchme, cnf, literal);
    }
}

//function to fetch the next record in case of a valid query
int SortedFile :: fetchNextPerQuery (Record &fetchme, CNF &cnf, Record &literal) {

    ComparisonEngine comparerEngineObj;
    //Until next record exists compare
    while (GetNext (fetchme)) {
        if (!comparerEngineObj.Compare (&literal, queryOrder, &fetchme, sortedFileOrder)){
            if (comparerEngineObj.Compare (&fetchme, &literal, &cnf))
                return 1;
        } else
            break;
    }
    return 0;
}

// Get next record in case of invalid query
int SortedFile :: fetchNext (Record &fetchme, CNF &cnf, Record &literal) {

    ComparisonEngine comparerEngineObj;
    while (GetNext (fetchme)) {
        if (comparerEngineObj.Compare (&fetchme, &literal, &cnf))
            return 1;
    }
    return 0;
}

// return 1 if record successfully found as per query
int SortedFile :: SearchPerBinary(Record &fetchme, CNF &cnf, Record &literal) {

    Page *tempPage = new Page;
    ComparisonEngine comparerEngineObj;

    off_t firstIndex = bufferPageIndex;
    off_t middleIndex;
    off_t lastIndex = filePointer->GetLength () - 1;

    while (true) {
        middleIndex = (firstIndex + lastIndex) / 2;
        filePointer->GetPage (tempPage, middleIndex);

        if (tempPage->GetFirst (&fetchme)) {
            if (comparerEngineObj.Compare (&literal, queryOrder, &fetchme, sortedFileOrder) <= 0) {
                lastIndex = middleIndex - 1;
                if (lastIndex <= firstIndex)
                    break;
            } else {
                firstIndex = middleIndex + 1;
                if (lastIndex <= firstIndex)
                    break;
            }
        } else
            break;
    }

    if (comparerEngineObj.Compare (&fetchme, &literal, &cnf)) {
        delete bufferPagePointer;

        bufferPageIndex = middleIndex;
        bufferPagePointer = tempPage;
        return 1;

    } else {
        delete tempPage;
        return 0;
    }
}

// Intialize the BigQ structure Object
void SortedFile :: InitializeBigQ () {
    writeIndicatorFlag = true;

    inputPipe = new Pipe (pageBufferSize);
    outputPipe = new Pipe (pageBufferSize);

    bigq = new BigQ(*inputPipe, *outputPipe, *sortedFileOrder, lengthOfRun);
}

// Internal merging records in the BigQ
void SortedFile :: InternalMerge () {
    inputPipe->ShutDown ();
    writeIndicatorFlag = false;

    if (filePointer->GetLength () > 0)
        MoveFirst ();

    Record *pipeRecord = new Record;
    Record *fileRecord = new Record;
    HeapFile *heapFile = new HeapFile;
    ComparisonEngine comparerEngineObj;

    heapFile->Create ("bin/temp.bin");
    int pipeFlagIndicator = outputPipe->Remove (pipeRecord);
    int fileFlagIndicator = GetNext (*fileRecord);

    while (fileFlagIndicator && pipeFlagIndicator) {
        if (comparerEngineObj.Compare (pipeRecord, fileRecord, sortedFileOrder) > 0) {
            heapFile->Add (*fileRecord);
            fileFlagIndicator = GetNext (*fileRecord);
        } else {
            heapFile->Add (*pipeRecord);
            pipeFlagIndicator = outputPipe->Remove (pipeRecord);
        }
    }

    while (fileFlagIndicator) {
        heapFile->Add (*fileRecord);
        fileFlagIndicator = GetNext (*fileRecord);
    }

    while (pipeFlagIndicator) {
        heapFile->Add (*pipeRecord);
        pipeFlagIndicator = outputPipe->Remove (pipeRecord);
    }

    outputPipe->ShutDown ();
    heapFile->Close ();
    delete heapFile;
    filePointer->Close ();
    remove (filePath);

    rename ("bin/temp.bin", filePath);
    filePointer->Open (1, filePath);
    MoveFirst ();
}

// Generate the ordermaker as per query
int SortedFile :: QueryOrderGen (OrderMaker &query, OrderMaker &order, CNF &cnf) {

    query.numOfAttributes = 0;
    bool ifFoundFlag = false;

    for (int outerIndex = 0; outerIndex < order.numOfAttributes; ++outerIndex) {
        for (int innerIndex = 0; innerIndex < cnf.numAnds; ++innerIndex) {
            if (cnf.orLens[innerIndex] != 1)
                continue;
            if (cnf.orList[innerIndex][0].op != Equals)
                continue;

            if ((cnf.orList[outerIndex][0].operand1 == Left && cnf.orList[outerIndex][0].operand2 == Left) ||
                (cnf.orList[outerIndex][0].operand2 == Right && cnf.orList[outerIndex][0].operand1 == Right) ||
                (cnf.orList[outerIndex][0].operand1 == Left && cnf.orList[outerIndex][0].operand2 == Right) ||
                (cnf.orList[outerIndex][0].operand1 == Right && cnf.orList[outerIndex][0].operand2 == Left))
                continue;

            if (cnf.orList[innerIndex][0].operand1 == Left &&
                cnf.orList[innerIndex][0].whichAtt1 == order.whichAtts[outerIndex]) {

                query.whichAtts[query.numOfAttributes] = cnf.orList[outerIndex][0].whichAtt2;
                query.whichTypes[query.numOfAttributes] = cnf.orList[outerIndex][0].attType;

                query.numOfAttributes++;
                ifFoundFlag = true;
                break;
            }

            if (cnf.orList[innerIndex][0].operand2 == Left &&
                cnf.orList[innerIndex][0].whichAtt2 == order.whichAtts[outerIndex]) {

                query.whichAtts[query.numOfAttributes] = cnf.orList[outerIndex][0].whichAtt1;
                query.whichTypes[query.numOfAttributes] = cnf.orList[outerIndex][0].attType;

                query.numOfAttributes++;
                ifFoundFlag = true;
                break;
            }
        }

        if (!ifFoundFlag)
            break;
    }
    return query.numOfAttributes;
}

