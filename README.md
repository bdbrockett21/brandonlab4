# Programming Lab 4: Simulating a Virtual Memory Manager

## Learning Outcomes

By the end of this activity, a student should be able to:

1. Gain experience in C programming
2. Implement a simulated virtual memory manager
3. Implement demand paging
4. Managing a Translation Look-Aside Buffer (TLB).
2. Represent, implement, and manipulate tables in C.
3. Use memory allocation
4. Implement a makefile to compile and link your program

## Introduction

This lab consists of writing a program that translates logical to physical addresses for a virtual address space of size 2<sup>16</sup> 
 = 65,536 bytes. Your program will read from a file containing logical addresses and, using a TLB and a page table, will translate each logical address to its corresponding physical address and output the value of the byte stored at the translated physical address. Your learning goal is to use simulation to understand the steps involved in translating logical to physical addresses. This will include resolving page faults using demand paging, managing a TLB, and implementing a page-replacement algorithm.


## Description

Your program will read a file containing several 32-bit integer numbers representing logical addresses. However, you only need to be concerned with 16-bit addresses, so you must mask the rightmost 16 bits of each logical address. These 16 bits are divided into (1) an 8-bit page number and (2) an 8-bit page offset. Hence, the addresses are structured as shown as:

<div style="text-align:center;"><img src="images/address.jpeg"></div>

Other specifics include the following:

* 2<sup>8</sup> entries in the page table
* Page size of 2<sup>8</sup> (256) bytes
* 16 entries in the TLB
* Frame size of 2<sup>8</sup> (256) bytes
* 256 frames
* Physical memory of 65,536 bytes (256 frames × 256-byte frame size)

Additionally, your program needs to be concerned only with reading logical addresses and translating them to their corresponding physical addresses. You do not need to support writing to the logical address space.

### Address Translation

Your program will translate logical to physical addresses using a TLB and page table as outlined in Section 9.3. Extract the page number from the logical address, then consult the TLB. In the case of a TLB hit, obtain the frame number from the TLB. In the case of a TLB miss, consult the page table. In the latter case, get the frame number from the page table, or a page fault occurs. A visual representation of the address-translation process is provided below.

<div style="text-align:center;"><img src="images/address-translation.svg"></div>

### Handling Page Faults

Your program will implement demand paging. The file **BACKING_STORE.bin** represents the backing store, a binary file of size 65,536 bytes provided with this project. When a page fault occurs, you will read a 256-byte page from the file **BACKING_STORE** and store it in an available page frame in physical memory. Since all frames are invalid at the beginning of your program, you should start adding at frame 0 in physical memory.

For example, Let's assume we get our first logical address, resulting in a page number of 251. At the start, the TLB is empty, and all the page table entries are invalid (i.e., set to 0 - see figure below). The steps to map the logical address to the physical address are as follows (**NOTE: steps correlate to steps shown in the figure below**):

1. The TLB is consulted for the frame number. This results in a TLB miss!
2. Next, the page table is consulted.  
3. This results in a page fault!
4. Page 251 is located in the  **BACKING_STORE** at byte address `251*256 = 64256` (remember that pages begin at 0 and are 256 bytes in size) and loaded into Frame 0, which is the next available frame. Fill the frame starting at 0, 1, 2, ..., and 255. This works since the size of the logical space is the same as the physical space.
5. Update
   <ol type="a"><li>The TLB table.</li><li>The page table.</li></ol>
6. Access the memory located at frame 0 and offset 25 (i.e., physical address 25). 

The TLB or the page table will resolve subsequent accesses to page 251.

<div style="text-align:center;"><img src="images/example1.svg"></div>

You will need to treat **BACKING_STORE.bin** as a random-access file so that your program can randomly seek to certain file positions for reading. Use the standard C library functions for performing I/O, including <span style="color:blue;">fopen()</span>, <span style="color:blue;">fread()</span>, <span style="color:blue;">fseek()</span>, and <span style="color:blue;">fclose()</span>.

The size of physical memory is the same as that of the virtual address space—65,536 bytes—so you do not need to be concerned about page replacements during a page fault.

### Managing the TLB

The TLB contains 16 entries, as specified earlier.  This means that when a TLB miss occurs, the TLB must be updated with the latest (page number, frame number) pair.  Initially, the TLB is empty, and updating it will be simple.  Just add the (page number, frame number) pair in the next available spot in the table.  When the table becomes full,  a decision needs to be made as to which entry in the table to replace.  The most straightforward strategy is to use a FIFO replacement strategy.  That is, treat the TLB as a circular queue and replace the entry at the head of the queue.  You may also use an LRU replacement strategy as well.

## Test File

The file **address.txt** provided with this project contains integer values representing logical addresses ranging from 0 to 65535 (the virtual address space size). Your program will open this file, read each logical address, translate it to its corresponding physical address, and output the value of the signed byte at the physical address.

## How to Approach the Implementation

First, write a simple program with two functions, one that extracts the page number and one that extracts the offset based on:

<div style="text-align:center;"><img src="images/address.jpeg"></div>

The easiest way to do this is by using the operators for bit-masking and bit-shifting. 

```
Page Number = (logical_address >> 8) & 0x00FF
Offset = logical_address & 0x00FF
```

To assemble the physical address, you will need to shift the frame value 8 bits to the left and **or** the offset to it.  That is,

```
int physical_address = (frame << 8) | offset
```

You are ready to begin once you can correctly establish the page number and offset from an integer number. Use the logical addresses provided in the file **address.txt**. The project also includes a file called **correct.txt** that provides the correct results for each of the first fifty (50) addresses in **address.txt**. Use the **correct.txt** file to verify that your functions are correct. 

Initially, you should bypass the TLB and use only a page table. You can integrate the TLB once your page table is working correctly. Remember, address translation can work without a TLB; the TLB just makes it faster. In this implementation, accessing the TLB will be done using a *linear search* and will be slow. However, the point of the exercise is not speed but to simulate the address-translation algorithm.

When you are ready to implement the TLB, use the strategy discussed in the section *Managing the TLB*.

## Lab Structure

The project contains the following files.

1. <strong><u>lab4.h</u></strong> - header file that has all the #define macros and structure definitions for the TLB, page table, backing store, and physical memory.  It also declares the functions for the TLB, page table, backing store, physical memory.
2. <strong><u>address.c</u></strong> - where all implementation of the TLB, page table, backing store, and physical memory.
3. <strong><u>main.c</u></strong> - Implemented.  Reads the logical addresses from a file provided on the command line and generates the output as specified in the <span style="font-weight:bold;">How to Run Your Program</span> section below.
4. <strong><u>correct.txt</u></strong> - provides the correct results for each of the first fifty (50) addresses.
5.  <strong><u>addresses.txt</u></strong> - list of logical addresses.
6.  <strong><u>BACKING_STORE.bin</u></strong> - represents the backing store, a binary file of size 65,536 bytes. 

You are not to change **lab4.h**. The only requirement is that you have an implementation of all the functions declared in **lab4.py**.

## What to Submit

You will need to submit the following files:

1. **address.c** - Contains all the implemented functions provided in **lab4.h** for the TLB, paging system, and physical memory.
3. **makefile** - compiles and creates an executable called <code>translate</code>

## How to Run Your Program

Your program should run as follows:

<p><span style="background-color:#EAEAEA; font-family:courier,monospace; padding: 5px; border-radius: 5px;">./translate addresses.txt</span></p>
 

Your program will read in the file **addresses.txt** or any file indicated on the command line.  The file **addresses.txt** contains 1,000 logical addresses ranging from 0 to 65535. Your program translates each logical address to a physical address and determines the contents of the signed byte stored at the correct physical address. (Recall that in the C language, the <span style="color: blue;">char</span> data type occupies a byte of storage, so we suggest using *signed char* values.)

Your program is to output the following values:

1. The page and offset in the format `(<page>, <offse>)`.
2. The logical address  (the integer value read from **addresses.txt**).
3. The frame and offest in the format `(<frame>, <offset>)`.
4. The corresponding physical address (what your program translates the logical address to).
5. The signed byte value stored in physical memory at the translated physical address.

Using the following format:

<p> <span style="background-color:#EAEAEA; color:purple; padding: 5px; border-radius: 5px;">(page, offset) logical_address, (frame, offset) physical_address, signed byte value</span></p>

For example, for the following 10 logical addresses that are in a file called addr.txt, 

```
16916
62493
30198
53683
40185
28781
24462
48399
64815
18295
```

The output should look as follows:

```
(66,20) 16916, (0, 20) 20, 0 
(244,29) 62493, (1, 29) 285, 0 
(117,246) 30198, (2, 246) 758, 29 
(209,179) 53683, (3, 179) 947, 108 
(156,249) 40185, (4, 249) 1273, 0 
(112,109) 28781, (5, 109) 1389, 0 
(95,142) 24462, (6, 142) 1678, 23 
(189,15) 48399, (7, 15) 1807, 67 
(253,47) 64815, (8, 47) 2095, 75 
(71,119) 18295, (9, 119) 2423, -35 
TLB Hit Rate: 0.00 %
Page Fault Rate: 100.00 %
``` 

Note that for only ten logical addresses all located in different pages, the TLB hit rate must be 0 and the page fault rate must be 100%.

If the logical addresses were such that five of them were located in the same pages as the other five, as in the example below,

```
16916
62493
30198
53683
40185
16914
62494
30188
53680
40186
```

Then the output will look as follows, with a 50% TLB hit rate and a 50% page fault rate.

```
(66,20) 16916, (0, 20) 20, 0 
(244,29) 62493, (1, 29) 285, 0 
(117,246) 30198, (2, 246) 758, 29 
(209,179) 53683, (3, 179) 947, 108 
(156,249) 40185, (4, 249) 1273, 0 
(66,18) 16914, (0, 18) 18, 16 
(244,30) 62494, (1, 30) 286, 61 
(117,236) 30188, (2, 236) 748, 0 
(209,176) 53680, (3, 176) 944, 0 
(156,250) 40186, (4, 250) 1274, 39 
TLB Hit Rate: 50.00 %
Page Fault Rate: 50.00 %

```


The file **correct.txt**, which contains the correct output values for the 50 addresses in the file **addresses.txt**, should be used to determine if your program is correctly translating logical to physical addresses, and if your paging/TLB system is working correctly.

<p style="color:red;"><u>NOTE:</u> your output must match exactly as provided above for each logical address in <strong>addresses.txt</strong>. There should be a single space after each comma (,).</p>

## Statistics

After completion, your program is to report the following statistics:

1. Page-fault rate — The percentage of address references that resulted in page faults.
2. TLB hit rate — The percentage of address references resolved in the TLB.

The exact format of the output of the page-fault and TLB hit rate are provided in the example output in the previous section.

Since the logical addresses in **addresses.txt** were generated randomly and do not reflect any memory access locality, do not expect to have a high TLB hit rate.

The functions to count the page-faults and TLB hits are implemented in the **stat.c** file, as are the functions to calculate the rates.  Those functions are also declared in the **lab4.h** file. 

## Submission Requirements

Click the <span style="background-color:green;color:white;">Use as template</span> button to copy the repo to your GitHub account and **clone** your repo on **your VM**. You should commit your code as you work on your program and push it systematically to your repo.

<p style="color:purple">Your submission must include a makefile that creates the executable as indicated in the <span style="color:black; font-size:1.2em; font-weight:bold;">What to Submit</span>  (i.e. when make is run on the command line without any arguments, it should create a <code>translate</code> executable). </p>

<div style="background-color:pink;">After completing your work, submit your code on <strong>CodeGrade</strong>.</div>




