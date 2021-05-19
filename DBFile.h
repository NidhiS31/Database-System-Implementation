#ifndef DBFILE_H
#define DBFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "Pipe.h"
#include "BigQ.h"
#include "GenericDBFile.h"

class DBFile {
	
private:
    // The generic DB file pointer
	GenericDBFile *genDBFilePointer;

public:
    //Constructor and Deconstructor of DBFile class
	DBFile ();
	~DBFile ();

    //Functions to create, open and close a file from the file path
	int Create (const char *fpath, fType ftype, void *startup);
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
#endif
