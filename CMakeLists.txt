cmake_minimum_required(VERSION 3.15)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(src
        BigQ.cc
        BigQ.h
        Comparer.cc
        Comparer.h
        Comparison.cc
        Comparison.h
        ComparisonEngine.cc
        ComparisonEngine.h
        Config.h
        DBFile.cc
        DBFile.h
        QueryProcessor.cc
        QueryProcessor.h
        Defs.h
        Errors.h
        File.cc
        File.h
        Function.cc
        Function.h
        HeapFile.cc
        HeapFile.h
        MyDBInterpreter.cc
        MyDBInterpreter.h
        ParseTree.h
        Pipe.cc
        Pipe.h
        Pthreadutil.h
        QueryPlan.cc
        QueryPlan.h
        Record.cc
        Record.h
        RelOp.cc
        RelOp.h
        RunsSorter.cc
        RunsSorter.h
        Schema.cc
        Schema.h
        SortedFile.cc
        SortedFile.h
        Statistics.cc
        Statistics.h
        Stl.h
        test.cc
        test.h
        TwoWayList.cc
        TwoWayList.h)
