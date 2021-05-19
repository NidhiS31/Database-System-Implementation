#ifndef P3NEW_BIGQ__2__SORTEDFILE_H
#define P3NEW_BIGQ__2__SORTEDFILE_H


#include "GenericDBFile.h"

class SortedFile : public GenericDBFile {

private:

    OrderMaker *sortedFileOrder;
    OrderMaker *queryOrder;

    int lengthOfRun;
    Pipe *inputPipe;
    Pipe *outputPipe;
    int pageBufferSize;
    BigQ *bigq;

public:

    SortedFile (OrderMaker *order, int runLength);
    ~SortedFile ();

    int Create (const char *fpath);
    int Open (char *fpath);
    int Close ();

    void Load (Schema &myschema, const char *loadpath);

    void MoveFirst ();
    void Add (Record &addme);
    int GetNext (Record &fetchme);
    int GetNext (Record &fetchme, CNF &cnf, Record &literal);

    int fetchNextPerQuery (Record &fetchme, CNF &cnf, Record &literal);
    int fetchNext (Record &fetchme, CNF &cnf, Record &literal);
    int SearchPerBinary(Record &fetchme, CNF &cnf, Record &literal);
    void InitializeBigQ ();
    void InternalMerge ();
    int QueryOrderGen (OrderMaker &query, OrderMaker &order, CNF &cnf);
};


#endif //P3NEW_BIGQ__2__SORTEDFILE_H
