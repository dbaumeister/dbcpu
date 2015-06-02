dbcpu

by Dominik Baumeister (MatrNr. 03628973)

# Assignment 4

Immediately runs the provided tests for Assignment 4.
However sadly not all of them pass.

1. test<uint64_t, MyCustomUInt64Cmp>(n); passes!

2. test<Char<20>, MyCustomCharCmp<20>>(n); fails with weird behavior:

    // assertion fails: lookup element 809676 with 1024 max. pages in BufferManager
    
    // assertion fails: lookup element 777924 with 512 max. pages in BufferManager
    
    // assertion fails: lookup element 762048 with 256 max. pages in BufferManager
    
    // assertion fails: lookup element 750141 with 64 max. pages in BufferManager
    
    // assertion fails: lookup element 750141 with 16 max. pages in BufferManager
    
    // assertion fails: lookup element 0 with 4 max. pages in BufferManager

3. test<IntPair, MyCustomIntPairCmp>(n); passes!