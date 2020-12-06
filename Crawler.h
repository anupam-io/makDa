#ifndef ___CRAWLER_H
#define ___CRAWLER_H

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <map>
#include <set>
#include <thread>
#include <math.h>
#include <condition_variable>

#include "headers/downloaders.h"
#include "headers/getLinks.h"
#include "headers/getDomain.h"

#include "thread_safe/int_ts.cpp"
#include "thread_safe/map_ts.cpp"
#include "thread_safe/queue_ts.cpp"

using namespace std;

#define RED "\033[31m"	 /* Red */
#define CYAN "\033[36m"	 /* Cyan */
#define GREEN "\033[32m" /* Green */
#define BLUE "\033[34m"	 /* Blue */
#define RESET "\033[0m"

class Crawler
{
	// setting public for testing purposes
public:
	ofstream log; // logging

	int_ts workingThreads; 		// total no of threads working
	int_ts maxPagesReached;	// for storing total processed pages till now
	int_ts totalVisitedPages;	// hashing utility for visited pages
	int max_file_size = 1024;
	int time_out = 5;

	mutex timingLock;			
	vector<vector<double>> threadTimings;
	map_ts<string, int> download_stat;

	// lock and cond_var for parent
	bool ready = false;
	condition_variable cv;
	mutex cv_m;

	// Parameters
	int maxLinks = 10;
	int maxPages = 10;
	int maxThreads = 5;

	// MAKE THESE THREAD SAFE
	// queue for storing linked websites
	queue_ts linkQueue;
	// map for storing visited websites
	map_ts<string, bool> discoveredSites;
	// for storing page relations
	map_ts<string, set<string>> adjList;
	

	// Constructor
	Crawler()
	{
		
	}

	// Destructor
	~Crawler()
	{
		
	}

	// Public Functions

	/*
		Initialize the Crawler.
	*/
	void initialize();

	/*
		Terminate the Crawler
	*/
	void terminate();

	/*
		Downloads a website and save it in buffer folder
	*/
	string downloader(string url);

	/*
		Parse a file from the buffer and update parameters{concurrency part}
	*/
	void parseFile(string filename);

	/*
		Start a crawler to discover a specific website.
	*/
	void runCrawler();

	/*
		Show the results of the crawling
	*/
	void showResults();

	/*
		Create a single thread
	*/
	void createThread();

	/*
		Sleeping the main thread
	*/
	void gotosleep();

	/*
		Awake the main thread
	*/
	void awake();

} myCrawler;

/*
	Child thread for downloading + parsing + updating shared variables
*/
void childThread(string url, int th_no);

#endif