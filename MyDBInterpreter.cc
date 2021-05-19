#include <iostream>

#include "Errors.h"
#include "MyDBInterpreter.h"
#include "ParseTree.h"
#include "Statistics.h"
#include "QueryPlan.h"
#include "QueryProcessor.h"

using std::cout;

extern "C" {
  int yyparse(void);     // defined in y.tab.c
}

extern struct FuncOperator *finalFunction;
extern struct TableList *tables;
extern struct AndList *boolean;
extern struct NameList *groupingAtts;
extern struct NameList *attsToSelect;
extern int distinctAtts;
extern int distinctFunc;
extern char* newtable;
extern char* oldtable;
extern char* newfile;
extern char* deoutput;
extern struct AttrList *newattrs;

void MyDBInterpreter::run() {
  char *fileName = "Statistics.txt";
  Statistics s;
  QueryProcessor d;
  QueryPlan plan(&s);
  while(true) {
    cout << "MyDB> ";
    yyparse();

    s.Read(fileName);

    if (newtable) {
      if (d.createTable()) cout << "Table created with the name " << newtable << std::endl;
      else cout << "Table " << newtable << " already exists." << std::endl;
    } else if (oldtable && newfile) {
      if (d.insertInto()) cout << "Values inserted into " << oldtable << std::endl;
      else cout << "Insert failed." << std::endl;
    } else if (oldtable && !newfile) {
      if (d.dropTable()) cout << oldtable <<" dropped" << std::endl;
      else cout << "Table " << oldtable << " does not exist." << std::endl;
    } else if (deoutput) {
      plan.setOutput(deoutput);
    } else if (attsToSelect || finalFunction) {
      plan.plan();
      plan.print();
      plan.execute();
    }
    clear();
  }
}

void MyDBInterpreter::clear() {
  newattrs = NULL;
  finalFunction = NULL;
  tables = NULL;
  boolean = NULL;
  groupingAtts = NULL;
  attsToSelect = NULL;
  newtable = oldtable = newfile = deoutput = NULL;
  distinctAtts = distinctFunc = 0;
  FATALIF (!remove ("*.tmp"), "remove tmp files failed");
}
