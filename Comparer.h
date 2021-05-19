

#ifndef P2_2WORKINGVERSION_COMPARER_H
#define P2_2WORKINGVERSION_COMPARER_H

#include "Comparison.h"
#include "RunsSorter.h"

class RecordsComparer {

private:
    OrderMaker *sortOrder;
public:
    RecordsComparer (OrderMaker *givenOrder);
    bool operator() (Record* leftRecord, Record* rightRecord);
};

class RunsComparer {
public:
    bool operator() (RunsSorter* left, RunsSorter* right);
};


#endif //P2_2WORKINGVERSION_COMPARER_H
