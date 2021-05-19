#ifndef GENERICDBFILE_H
#define GENERICDBFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "Pipe.h"
#include "BigQ.h"

typedef enum {
    HEAP,
    SORTED,
    TREE
} fType;

struct SortInfo {

    OrderMaker *mySortOrder;
    int lengthOfRun;

};

class GenericDBFile {

protected:

    //Pointer to the file
    File *filePointer;
    //Pointer to the storage file's path
    char *filePath;
    //Pointer to the buffered Page
    Page *bufferPagePointer;
    //Buffered page's index
    off_t bufferPageIndex;
    //Writing mode indicator
    bool writeIndicatorFlag;

public:

    virtual int Create (const char *fpath) = 0;
    virtual int Open (char *fpath) = 0;
    virtual int Close () = 0;

    virtual void Load (Schema &myschema, const char *loadpath) = 0;

    virtual void MoveFirst () = 0;
    virtual void Add (Record &addme) = 0;
    virtual int GetNext (Record &fetchme) = 0;
    virtual int GetNext (Record &fetchme, CNF &cnf, Record &literal) = 0;

    virtual ~GenericDBFile ();

};
#endif //P3NEW_BIGQ__2__GENERICDBFILE_H
