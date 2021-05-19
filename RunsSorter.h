
#ifndef P2_2WORKINGVERSION_RUNSSORTER_H
#define P2_2WORKINGVERSION_RUNSSORTER_H


#include "File.h"

class RunsSorter {

private:
    Page bufferPage;
public:
    int pageIndex;
    int runsSize;
    Record* initialRecord;
    OrderMaker* sortOrder;
    File *fileForRuns;

    RunsSorter (int runLength1, int pageOffset1, File *file, OrderMaker *order);
    RunsSorter (File *file, OrderMaker *order);
    ~RunsSorter();
    int fetchInitialRecord();
};


#endif //P2_2WORKINGVERSION_RUNSSORTER_H
