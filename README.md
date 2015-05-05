# dbcpu
by Dominik Baumeister (MatrNr. 03628973)

Start the program with the arguments <pagesOnDisk> <pagesInRAM> <threads> to trigger buffertest.

I tested with:

8000 500 8
8000 500 4
8000 500 2
8000 500 1


10000 1000 4
10000 1000 1

I let it run enough times each to nullify the chance of race conditions.
However at the moment fixPage is under a global lock -> not really the best option performance wise.