# ECS154A
Cache Design 
You are to implement two caches for a simulated RAM that has 16-bit addresses, and is byte addressable.
Though addresses are byte addressable, blocks are always read from and written to RAM with addresses that are multiples
of eight, e.g., 8, 56, and 416. Both caches will have block sizes of 8 bytes, and a total capacity of 512 bytes.
Your caches will need to support a read operation (reading a byte from the cache) and a write operation (writing a
new byte of data into the cache). Both caches will support a write-back write policy which will require you to use a dirtybit.
In addition, cache must support a write-allocate write miss policy, in which a write miss causes the appropriate line to
be brought into the cache from memory, and the write's value to update the correct part of the line in the cache, which will
then become dirty.
The first cache, implemented in dmcache.cpp, should rely on direct mapping. After you have dmcache.cpp
working properly, you can implement a four-way set associative cache in sacache.cpp that uses least recently used (LRU)
replacement policy for blocks.
Both caches take as input a filename from the command line. The file specified by the filename will be an ASCII
file with each line in the following 4-byte format:

Bytes Function
1-2 16 bit address
3 Read/Write
4 8 bits of Data

The read function will be designated by all 0's and the write will be designated by all 1's. Upon a read operation
the data segment of the 4-byte format will be ignored. However when a write occurs the data will be written into the
specified byte. For ease of creation the input file will be in hex. The direct mapped cache produces as output a file named
dm-out.txt, and the set associative cache produces a file named sa-out.txt. Each line of the output file corresponds to the
results of each read operation from the input file. The information on each line will be address specified in the input line,
the 8 bytes of data displayed with the lowest addressed byte on the right, a HIT output indicating whether the requested
item was found in the cache (0 for miss, 1 for hit), and the dirty-bit. Note that the dirty bit will displayed as 1 if the
original dirty bit was set, even if there is a read miss that will cause the dirty bit to be cleared. These four pieces of
information should be separated by a space. Here is an example with some comments and bold added by me.

[ssdavis@lect1 p5]$ cat dmtest-8-64-40-1.txt // put into 4 columns to save space
3F69 FF E2
856D FF 9A
3F68 FF F9
856D FF 0C
856B FF F8
3F6F FF 87
856F FF DA
3F6A FF F1
3F6E FF C1
856B FF 69
3F6F FF 23
856E FF 1A
3F6E FF 66
3F6A FF 90
856B FF A1
3F6F FF C8
3F69 FF 33
856C 00 00
3F69 00 33
8569 00 00
3F6F FF D9
8568 00 00
3F6D FF 51
3F68 FF E1
8568 FF 97
856C FF 2E
856C 00 2E
3F6A FF 4C
856C FF 9A
3F6B FF DA
3F6A FF B1
8568 FF C2
3F6E FF 4C
856E 00 1A
3F6A FF 23
3F6E FF 73
3F6F FF C2
3F6E FF 8F
3F6C 00 00
856F FF F8

[ssdavis@lect1 p5]$ dmcache.out dmtest-8-64-40-1.txt
[ssdavis@lect1 p5]$ cat dm-out.txt
856C DA1A0C00A1000000 0 1 // 856C 00 00
3F69 C8660000009033F9 0 0 // 3F69 00 33
8569 DA1A0C00A1000000 0 0 // 8569 00 00
8568 DA1A0C00A1000000 0 1 // 8568 00 00
856C DA1A0C2EA1000097 1 1 // 856C 00 2E
856E DA1A0C9AA10000C2 0 1 // 856E 00 1A
3F6C C28F5100DA2333E1 1 1 // 3F6C 00 00
[ssdavis@lect1 p5]$

You will find a test input files, their corresponding correct output files, my executables, dmcacheTest.out, and
sacacheTest.out in ~ssdavis/154/p5. dmcacheTest.out and sacacheTest.out can be used to create input files. The
corresponding output files are created by supplying the input files to my own versions of dmcache.out and sacache.out.
An important thing to notice is that when a line gets evicted from the cache, and at some later point is brought
back into the cache by a subsequent read, the read must return the correct value (not just zero), as if it was stored in RAM
when it was evicted from the cache. A specific example of this is line 9 in dm-test-output.txt. You can implement this
however you like, but a perfectly acceptable way to do it is to have an array of length 2^16 chars to act as RAM that cache
lines get evicted to. Initialize the contents of your cache and memory to all 0's.
