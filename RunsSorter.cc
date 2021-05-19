#include "RunsSorter.h"

//Constructor for the class RunsSorter
RunsSorter :: RunsSorter (int runLength1, int pageOffset1, File *file, OrderMaker* order) {

    runsSize = runLength1;
    pageIndex = pageOffset1;
    initialRecord = new Record ();
    fileForRuns = file;
    sortOrder = order;
    fileForRuns->GetPage (&bufferPage, pageIndex);
    fetchInitialRecord();

}

//Method overload of the constructor for RunsSorterClass
RunsSorter :: RunsSorter (File *file, OrderMaker *order) {
    fileForRuns = file;
    sortOrder = order;
    initialRecord = NULL;
}

//Destructor for runsSorter class
RunsSorter :: ~RunsSorter () {
    delete initialRecord;
}

//Function to fetch the first record of a given run
int RunsSorter :: fetchInitialRecord () {
    Record* record = new Record();
    if(runsSize <= 0)
        return 0;

    //if first record is empty go to next page
    if (bufferPage.GetFirst(record) == 0) {
        pageIndex++;
        fileForRuns->GetPage(&bufferPage, pageIndex);
        bufferPage.GetFirst(record);
    }
    runsSize--;
    initialRecord->Consume(record);
    return 1;
}
