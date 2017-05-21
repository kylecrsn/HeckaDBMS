# HeckaDBMS

### Abstract

HeckaDBMS is a full-stack database system including an in-memory key-value storage, implementations of Strict 2PL and Microsoft's "Optimistic" Hekaton, and a client for launching concurrent transaction attempts and testing metrics.

### Project Structure

The project is split into directories based on the logical modules each piece of code is split up into:

##### /

Top-level files including the program's entry point defined in main.cpp

##### /client

An interactive CLI-based client for communicating with the database and performing transactions

##### /tm

A thin layer of code proxying requests to the database through one of the two access protocols

##### /tm/2pl

An implementation of Strict-2PL based around managing a lock table for concurrent read/write access

##### /tm/hekaton

An implementation of [Microsoft's "Optimistic" Hekaton](http://vldb.org/pvldb/vol5/p298_per-akelarson_vldb2012.pdf) DBMS

##### /dm

Utility methods for creating and managing the datastore. Provides a simple interface to the TM through Get and Put functions, which equate to read and write operations respectively. By doing so, the DM facilitates the TM's access to the data in a manner that is independent of which protocol is being used

Below is a chart visualizing how these modules are hierarchically organized:



### Getting Started

The following instructions are for Linux Ubuntu-derivative environments

1. First off, make sure git is installed and clone the repo

    ```sh
    $ sudo apt-get update
    $ sudo apt-get install -y git
    $ git clone git@github.com:carsonkk/HeckaDBMS.git
    ```

2. This project uses cmake for compilations, so make a directory for output files from the build process

    ```sh
    $ mkdir build
    $ cd build
    ```
    
3. At this stage, attempt to build the project. If you encounter any errors with missing dependencies for C++ libraries or cmake, skip to 4. then come back to 3. and make sure compilation succeeds

    ```sh
    $ cmake ../
    $ make
    ```
    
4. If compilation failed in the previous step, make sure you have C++ and cmake libraries installed in discoverable locations

    ```sh
    $ sudo apt-get install -y build-essential
    $ sudo apt-get install -y cmake
    ```

5. The output in the build directory will include the *heckadbms* executable. To run the program, follow the following:

    ```sh
    $ ./heckadbms -p (2pl|hekaton) [-t (manual|scale|vary)]
    ```
    - **(-p) protocol_type**: Specifies which underlying protocol the Transaction Manager should use for transaction attempts. *2pl* refers to a variant of Strict 2PL while *hekaton* is a slightly altered version of Microsoft's Hekaton system
    - **(-t) test_metric**: Optional argument which specifies what type of test to perform against the database. By default, the system will use *manual* testing, which provides the user with a verbose experience in the client, launching individual transactions one step at a time. *Scale* refers to a testing approach in which the user specifies certain parameters such as the minimum and maximum number of threads that attempt concurrent transactions, scaling the load on the system towards the maximum to stress test its effectiveness. Finally, *vary* uses a consistent number of user-specified threads but instead uses a variable ratio in the number of read-only to read-write transaction attempts















