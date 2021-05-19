#include <string>
#include <fstream>
#include <iostream>
#include "Record.h"
#include "Schema.h"
#include "Comparison.h"
#include "DBFile.h"
#include "Defs.h"
#include "HeapFile.h"
#include "SortedFile.h"

GenericDBFile :: ~GenericDBFile () {
}

//Constructor for DBFile class
DBFile :: DBFile () {
}

//Deconstructor for DBFile class
DBFile :: ~DBFile () {
	delete genDBFilePointer;
}

//Function to create the file of the selected type
int DBFile :: Create (const char *fpath, fType ftype, void *startup) {
	
	ofstream outStreamObj;
	char outStreamPath[100];
	
	sprintf (outStreamPath, "%s.md", fpath);
	outStreamObj.open (outStreamPath);

    //When the file type to be implemented is Heap
	if (ftype == HEAP) {
		outStreamObj << "heap" << endl;
        genDBFilePointer = new HeapFile;
	}

    //When the file type to be implemented is Sorted
	else if (ftype == SORTED) {
		outStreamObj << "SORTED" << endl;
		outStreamObj << ((SortInfo *) startup)->lengthOfRun << endl;
		outStreamObj << ((SortInfo *) startup)->mySortOrder->numOfAttributes << endl;
		((SortInfo *) startup)->mySortOrder->PrintInOfstream (outStreamObj);

        genDBFilePointer = new SortedFile (((SortInfo *) startup)->mySortOrder, ((SortInfo *) startup)->lengthOfRun);
	}
	else if (ftype == TREE)
	    outStreamObj << "tree" << endl;
	else
	    {
		outStreamObj.close ();
		cout << "Sorry!! The file type(" << fpath << ") cannot be recognized!" << endl;
		return 0;
	}
	
	genDBFilePointer->Create (fpath);
	outStreamObj.close ();
    //return 1 when successful
	return 1;
}

//Function to open a file
int DBFile :: Open (char *fpath) {
	ifstream inStreamObj;
	string tempString;
	
	int attributeNumber;
	char *inStreamPath = new char[100];
	
	sprintf (inStreamPath, "%s.md", fpath);
	inStreamObj.open (inStreamPath);
	
	if (inStreamObj.is_open ()) {
		inStreamObj >> tempString;
		if (!tempString.compare ("heap"))
            genDBFilePointer = new HeapFile;
		else if (!tempString.compare ("SORTED")){
			
			int lengthOfRun;
			OrderMaker *order = new OrderMaker;
			
			inStreamObj >> lengthOfRun;
			inStreamObj >> order->numOfAttributes;
			
			for (int index = 0; index < order->numOfAttributes; index++) {
				inStreamObj >> attributeNumber;
				inStreamObj >> tempString;
				
				order->whichAtts[index] = attributeNumber;
				
				if (!tempString.compare ("Int"))
					order->whichTypes[index] = Int;
				else if (!tempString.compare ("Double"))
					order->whichTypes[index] = Double;
				else if (!tempString.compare ("String"))
					order->whichTypes[index] = String;
				else {
					delete order;
					inStreamObj.close ();
                    cout << "Invalid SORTED file (" << fpath << ")" << endl;
					return 0;
				}
				
			}

            genDBFilePointer = new SortedFile (order, lengthOfRun);
			
		} else if (!tempString.compare ("tree")) {

		} else {
			inStreamObj.close ();
            cout << "Invalid file type  (" << fpath << ")" << endl;
            return 0;
		}
		
	} else {
		inStreamObj.close ();
        cout << "Cannot Open the file (" << fpath << ")!" << endl;
		return 0;
	}
	
	genDBFilePointer->Open (fpath);
	inStreamObj.close ();
	
	return 1;
}

//Function to load the selected file type instance from the text file
void DBFile :: Load (Schema &myschema, const char *loadpath) {
	genDBFilePointer->Load (myschema, loadpath);
}

//Function to move the pointer to the first record of the heap file
void DBFile :: MoveFirst () {
	genDBFilePointer->MoveFirst ();
}

//Function to add the next record to the end of the file
void DBFile :: Add (Record &addme) {
	genDBFilePointer->Add (addme);
}

//Function to fetch the next record from the page
int DBFile :: GetNext (Record &fetchme) {
	return genDBFilePointer->GetNext (fetchme);
}

//Method overload of GetNext function to fetch the next record from the page
//that is accepted by the selection predicate
int DBFile :: GetNext (Record &fetchme, CNF &cnf, Record &literal) {
	return genDBFilePointer->GetNext (fetchme, cnf, literal);
}

//Function to close the DBFile file
int DBFile :: Close () {
    return genDBFilePointer->Close ();
}