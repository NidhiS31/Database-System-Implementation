#include "BigQ.h"

//Constructor for BigQ class
BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
    inputPipe = &in;
    outputPipe = &out;
    this->sortOrder = &sortorder;
    lengthOfRuns = runlen;
    totalPages = 1;
    pthread_create(&worker, NULL, StartMainThread, (void *)this);
}

//function to write runs to file
bool BigQ ::fileWriter() {
    int pageCounter = 1;
    Page* tempPage = new Page();
    int firstPageOffset = totalPages;
    int recordListSize = listOfRecords.size();

    //run until the sizeOfJoin of the records list
    for (int index = 0; index < recordListSize; index++) {
        Record* tempRecord = listOfRecords[index];
        if ((tempPage->Append (tempRecord)) == 0) {
            pageCounter++;
            fileForRuns.AddPage (tempPage, totalPages);
            totalPages++;

            tempPage->EmptyItOut ();
            tempPage->Append (tempRecord);
        }
        delete tempRecord;
		
    }
	
    fileForRuns.AddPage(tempPage, totalPages);
    totalPages++;

    tempPage->EmptyItOut();
    listOfRecords.clear();
    delete tempPage;

    UpdateQueue(recordListSize, firstPageOffset);
    return true;
	
}

//Function to add runs to priority Queue
void BigQ :: UpdateQueue (int sizeOfRuns, int pageOffset) {

    RunsSorter* tempRun = new RunsSorter(sizeOfRuns, pageOffset, &fileForRuns, sortOrder);
    priorityQueueForRuns.push(tempRun);
}

//Function to sort the list of records
void BigQ :: ListOfRecordsSorter() {
	
    Page* tempPage = new Page();
    Record* record = new Record();

    int pageIndex = 0;
    int recordIndex = 0;
    //int runIndex = 0;

	
    srand (time(NULL));
    fileNamePointer = new char[100];
    sprintf (fileNamePointer, "%d.txt", (int)worker);
    fileForRuns.Open (0, fileNamePointer);

    //Until input pipe has records
    while (inputPipe->Remove(record)) {
		
        Record* tempRecord = new Record ();
		tempRecord->Copy (record);
        
		recordIndex++;

		//for pages of run length
        if (tempPage->Append (record) == 0) {
            pageIndex++;
            if (pageIndex == lengthOfRuns) {
                sort (listOfRecords.begin (), listOfRecords.end (), RecordsComparer (sortOrder));
                fileWriter();
                pageIndex = 0;
            }
			
            tempPage->EmptyItOut ();
            tempPage->Append (record);
        }
        listOfRecords.push_back (tempRecord);
    }
    
    
    // for the last run
    if(listOfRecords.size () > 0) {
        sort (listOfRecords.begin (), listOfRecords.end (), RecordsComparer (sortOrder));
        fileWriter();
        tempPage->EmptyItOut ();
    }
    delete record;
    delete tempPage;
}

//Function to merge the SORTED runs, record by record
void BigQ :: MergeSortedRuns () {
    
    RunsSorter* tempRun = new RunsSorter (&fileForRuns, sortOrder);
    Page page;

    //until priority is not empty
    while (!priorityQueueForRuns.empty ()) {
		
        Record* tempRecord = new Record ();
        tempRun = priorityQueueForRuns.top ();
        priorityQueueForRuns.pop ();
        tempRecord->Copy (tempRun->initialRecord);
        outputPipe->Insert (tempRecord);
		
        if (tempRun->fetchInitialRecord() > 0) {
            priorityQueueForRuns.push(tempRun);
		}
        delete tempRecord;
    }

    fileForRuns.Close();
    remove(fileNamePointer);
    outputPipe->ShutDown();
    delete tempRun;
}

