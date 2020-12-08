Forked from [Web-Crawler](https://github.com/ShrutiKatpara/Web-Crawler). 

# beQ - A multithreaded web crawler with libcURL & thread-safe data structures
## Description
The goal of this project is to create a multi-threaded web crawler. A Web crawler is an Internet bot that systematically browses the World Wide Web, typically for the purpose of Web indexing. Any search engine uses these indexes, web graphs, and an appropriate algorithm ( such as PageRank ) to rank the pages. The main focus of the project would be to implement a multi-threaded downloader that can download multi websites at the same time. The plan is to implement this in C++.

## Features/Deliverables
 - Part 1: Download HTTP as well as HTTPS protocol files
 - Part 2: Multiple threads working
 - Part 3: (Extended scope) Web Ranking and Web Indexing

## Components
 - **Crawler** as a thread controller
 - **Child thread**
    - HTML downloader using libcURL
    - Link extractor
    - Domain extractor
    - Updating shared variables

## Psuedocode
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
<pre><code>crawler_loop(){
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
 }
 ...</pre></code>

## Using thread-safe data structure for concurrency 
    - For each of the data structure, having a single lock or RW lock if required
    - Waiting time distributed over different locks
    - Different thread safe data structures:
        - Thread safe integer
        - Thread safe queue
        - Thread safe map
    - *Pros*:
     - No need to keep the lock acquired for more amount of time. Concurrency can be achieved by removing the serial nature shown by single-locking mechanism.
    - *Cons*: 
     - Overhead due to multiple locks

## How to run multi threaded web crawler
 - use `make` to compile the program
 - supported arguments are:
  - `maxlinks`		  : maximum links extracted from a single website
  - `pagelimit`		 : maximum pages to process
  - `threads`			  : maximum concurrent threads to create
  - `maxfilesize`	: maximum file size to download
  - `timeout`			  : maximum time waiting for website response
  - `restore_data`: restoring crawler state from previous run
  - `save_data`		 : saving current crawler state for next run

## Web-Ranking(Simple counter based)
I have used a simple counter based mechanism for web-ranking. If one website contains 10 links, the each ranking point is awarded to the domain of the website found.
I am also storing the output as a adjacency-list of the graph. So that other ranking mechanisms can be implemented on the top of it later.

## Libraries used
 - libcURL
  - for managing HTML downloads
 - Pthread library
    - For concurrency and synchronization techniques
       - Locks
         - Single locks
         - Reader Writer locks
       - Condition Variables
 - Matplotlib
    - Plotting the graphs

## Credits
 - [How to write a multi-threaded webcrawler](http://www.andreas-hess.info/programming/webcrawler/index.html)
 - [DOWNLOADING A WEB PAGE IN C USING A SOCKET](http://www.martinbroadhurst.com/downloading-a-web-page-in-c-using-a-socket.html)
 - [std::condition_variable](https://www.cplusplus.com/reference/condition_variable/condition_variable/)
 - [Scrapy](https://scrapy.org/)

## Contributors
- [Anupam Kumar](https://github.com/akcgjc007)
