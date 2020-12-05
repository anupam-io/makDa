#include <iostream>
#include "Crawler.h"
#include "Crawler.cpp"
// gives a Crawler object named `myCrawler` from default

using namespace std;
using namespace std::chrono;

int main(int argc, const char *argv[])
{
	srand(time(0));

  // Crawler: maxLinks pagesLimit threads
  myCrawler.maxLinks = stoi(argv[1]);
  myCrawler.pagesLimit = stoi(argv[2]);
  myCrawler.maxThreads = stoi(argv[3]);
	clock_t t1, t2;
  
	t1 = clock();
  myCrawler.initialize();
  myCrawler.runCrawler();
//   myCrawler.ranker();
  myCrawler.showResults();
  myCrawler.terminate();
	t2 = clock();


	double res = double(t2-t1)*1000 / CLOCKS_PER_SEC;

	cout << "FINISHED." << endl;
	cout<< RED << "Elapsed time in milliseconds : "
			<< res
			<< C_END << endl;

	ofstream fout("OUTPUT/crawler_timings.csv", std::ios_base::app);
	fout << myCrawler.maxThreads << ", " << res << endl;
	fout.close();

  return 0;
}
