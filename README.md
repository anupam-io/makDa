<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-4-orange.svg?style=flat-square)](#contributors)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

# Multi Threaded Web-Crawler

## Description
The goal of this project is to create a multi-threaded web crawler. A Web crawler is an Internet bot that systematically browses the World Wide Web, typically for the purpose of Web indexing. Any search engine uses these indexes, web graphs, and an appropriate algorithm ( such as PageRank ) to rank the pages. The main focus of the project would be to implement a multi-threaded downloader that can download multi websites at the same time. The plan is to implement this in C or C++.

## Features/Deliverables
 - Part 1: Multi-threaded web-crawler
 - Part 2: (Extended scope) Web Ranking
 
## [MULTITHREADED Web Crawler](#multithreaded-web-crawler)

### [Multithreaded Components](#multithreaded-components)
 - **Crawler** as a thread controller
 - **Child thread**
    - HTML downloader
    - Link extractor
    - Domain extractor
    - Ranking using different algorithms

### [Multithreaded Psuedocode](#multithreaded-psuedocode)
### Crawler loop code
<pre><code>...
while(1){
    if(pagesLimitReached || visitedpages>=pagesLimit){
        pagesLimitReached = true;
        if(w_threads){
            <b>gotosleep()</b>;
        }
        else {
            break;
        }
    }
    else{
        if (w_threads < maxThreads && queue_size>0){
            createThread();
        }
        else if(w_threads == 0){
            break;
        }
        else{
            <b>gotosleep()</b>;
        }
    }
}
...</pre></code>

### Child Thread code
<pre>
<code>
...
download(url);
parse(url);
update(queue, visitedLinks, ranking);
if(pagesLimitReached){
    if(workingThreads == 0){
        <b>wake_parent()</b>;
    }
}
else{
    <b>wake_parent()</b>;
}
...
</pre></code>

### [Different locking techniques](#different-locking-techniques)
- [Using **THREAD SAFE DATA STRUCTURE** technique](#using-thread-safe-data-structure-technique)
    - For each of the data structure, having a single lock or RW lock if required
    - Waiting time distributed over different locks
    - Different thread safe data structures:
        - Thread safe integer
        - Thread safe queue
        - Thread safe map
    - *Pros* No need to keep the lock acquired for more amount of time. Hence concurrency can be efficiently achieved in multi processor CPUs
    - *Cons* Overhead due to multiple locks

### [How to run multi threaded web crawler](#how-to-run-multi-threaded-web-crawler)
- use `make` to compile the program
- `maxlinks`, `pagelimit`, `threads` can be given as argument in with `make` command.
    - For e.g. `make maxlinks=100 pagelimit=100 threads=20`
    - Here the arguments are:
        - `maxlinks`: Maximum number of links to be extracted from a website
        - `pagelimit`: Maximum number of websites to be downloaded while crawling
        - `threads`: Maximum number of threads to be created
        - `rankerFlag`: Flag to choose which ranking algorithm to be executed (`n` = simple counter based web ranker, `sp` = pagerank with sampling, `ip` = pagerank with iteration)

_[Back to Table of Contents](#table-of-contents)_

### [Simple counter based](#simple-counter-based)

<pre><code>
...
corpus = read(csv_file)
for website in corpus.keys():
    for x in corpus[website]:
        rank[website]+=1
...
</code></pre>

#### Demo run
<pre><code>------------------------------------------------
  Domain Name rankings using counter
------------------------------------------------

................................................
  Domain Name            Rank
................................................

1 .  mygov.in                                  43
2 .  main.ayush.gov.in                         36
3 .  tourism.gov.in                            24
4 .  digitalindia.gov.in                       19
5 .  asi.nic.in                                16
------------------------------------------------------------</pre></code>

## [Libraries used](#libraries-used)
 - Sockets
 - OpenSSL
 - Pthread library
    - For concurrency and synchronization techniques
       - Locks
         - Single locks
         - Reader Writer locks
       - Condition Variables
 - Matplotlib
    - Plotting the graphs

## [Credits](#credits)
 - [How to write a multi-threaded webcrawler](http://www.andreas-hess.info/programming/webcrawler/index.html)
 - [DOWNLOADING A WEB PAGE IN C USING A SOCKET](http://www.martinbroadhurst.com/downloading-a-web-page-in-c-using-a-socket.html)
 - [std::condition_variable](https://www.cplusplus.com/reference/condition_variable/condition_variable/)
 - [Scrapy](https://scrapy.org/)
 - [PageRank](https://cs50.harvard.edu/ai/2020/projects/2/pagerank/)

## [Contributors](#contributors)
- [Anupam Kumar](https://github.com/akcgjc007)

<!--
Suggested edits:
    make a database of downloading websites
    upload compressed files online
    add the parsing feature
    add the word search feature
    
    Add a beautiful python web app for live progress track.[instead of showing everything on terminal]




    Architecture
    Intelligent Crawling
    Efficient algorithms
    Scalability
    Language independent
    Transparency
    Reliability
    Anti-crawler mechanisms
    Data delivery
    Support
    Data Quality


    file crawler
    more than one crawler
    dist web crawler

-->