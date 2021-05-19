#ifndef P3NEW_BIGQ__2__HEAPFILE_H
#define P3NEW_BIGQ__2__HEAPFILE_H


#include "GenericDBFile.h"

class HeapFile : public GenericDBFile {

public:

    //Constructor and Deconstructor of HeapFile class
    HeapFile ();
    ~HeapFile ();
    //Functions to create, open and close a file from the file path
    int Create (const char *fpath);
    int Open (char *fpath);
    int Close ();
    //Function to load data from tables
    void Load (Schema &myschema, const char *loadpath);
    //Function to move the pointer to the first record
    void MoveFirst ();
    //Function to add records
    void Add (Record &addme);
    //Functions to fetch next record
    int GetNext (Record &fetchme);
    int GetNext (Record &fetchme, CNF &cnf, Record &literal);
};

#endif //P3NEW_BIGQ__2__HEAPFILE_H
