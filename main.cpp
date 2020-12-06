#include <iostream>
#include "Crawler.h"
#include "Crawler.cpp"
// gives a Crawler object named `myCrawler` from default

int main(int argc, const char *argv[])
{
	srand(time(0));

	// Crawler: maxLinks pagesLimit threads
	myCrawler.maxLinks = stoi(argv[1]);
	myCrawler.maxPages = stoi(argv[2]);
	myCrawler.maxThreads = stoi(argv[3]);
	myCrawler.max_file_size = stoi(argv[4]);
	myCrawler.time_out = stoi(argv[5]);


	clock_t t1, t2;

	t1 = clock();
	myCrawler.initialize();
	myCrawler.runCrawler();
	//   myCrawler.ranker();
	myCrawler.showResults();
	myCrawler.terminate();
	t2 = clock();

	double res = double(t2 - t1) * 1000 / CLOCKS_PER_SEC;

	cout << "FINISHED." << endl;
	cout << RED << "Elapsed time in milliseconds : " 
		<< res << RESET << endl;

	return 0;
}
