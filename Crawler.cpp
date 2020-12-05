#ifndef ___CRAWLER_CPP
#define ___CRAWLER_CPP

#include "Crawler.h"

void Crawler::initialize()
{

	log.open("logs.txt");
	log << "Crawler initialized" << endl;

	srand(time(0));
	c_init();

	// initialize value of soem variables


	// Add initial urls from initialLinks.txt
	ifstream lin("INPUT/initialLinks.txt");
	if (lin)
	{
		int n;
		lin >> n;
		string a;
		for (int i = 0; i < n; i++)
		{
			lin >> a;
			if (a != "")
			{
				linkQueue.push(a);
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		// Unable to read starting links from input file
		cout << "Error reading file: \"initialLinks.txt\"" << endl;
		linkQueue.push("https://www.google.com");
	}
}

void Crawler::terminate(){
	c_finish();
	log.close();

}

string Crawler::downloader(string url)
{
	auto res = html_downloader(url);
	if(res.first == true){
		download_stat.inc("SUCCESS");
		return res.second;
	} else{
		download_stat.inc(res.second);
		return "";
	}

	return "";
}

void Crawler::gotosleep()
{
	unique_lock<mutex> lk(cv_m); // unique_lock for conditional variable
	cout << "Going to sleep now" << endl;

	ready = false;				 // because main thread is now not ready to process any data

	// written in a while loop, if spuriously woken up
	while (!ready)
		cv.wait(lk); // Waiting until not ready

	cout << "Awaken now." << endl;
}

void Crawler::awake()
{
	ready = true;		// because now main thread needs to be awaken
	cv.notify_one();	// notifying the main thread which is sleeping
}

void Crawler::createThread()
{
	string currentSite = linkQueue.pop();
	totalVisitedPages.add(1);
	workingThreads.add(1);
	discoveredSites.inc(currentSite);

	log << currentSite << endl;
	cout << GREEN << "Creating a thread, total: " << workingThreads.value() << C_END << endl;

    // update pagesLimitReached

	if(totalVisitedPages.value() >= pagesLimit){
		pagesLimitReached.add(1);
		cout << RED << "~!!!pagesLimit Reached here.!!!~" << C_END << endl;
	}

    // create thread
	thread th = thread(childThread, currentSite, totalVisitedPages.value());
	th.detach();
}

void Crawler::runCrawler()
{
	while (true)
	{
		if (pagesLimitReached.value())
		{
			if(workingThreads.value() < 1)
			{
				// exiting

				cout << RED << "Exiting as all threads are finished & pagelimit reached." << C_END << endl;

				break;
			}
			else
			{
				// sleep as some threads are still working
				gotosleep();
			}
		}
		else
		{
			// pagesLimit not reached
			if (workingThreads.value() < maxThreads && linkQueue.size() > 0)
			{
				// create thread
				createThread();
			}
			else if (workingThreads.value() == 0)
			{
				// exiting
				cout << "EXITING AS NO THREADS WORKING. & pagelimit not reached" << endl;
				break;
			}
			else
			{
				// sleep
				gotosleep();
			}
		}

		// End of crawler loop
	}

	log << "Crawling completed." << endl
			<< endl;
}

/*
*/
void Crawler::showResults()
{
	string dashline = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

	cout << dashline << endl;
	cout << "DOWNLOADER ANALYTICS:" << endl;
	for(auto i: download_stat.value()){
		cout << i.first << ": " << i.second << endl;
	}
	cout << dashline << endl;
	cout << "CHILD THREAD ANALYTICS:" << endl;
	long long sum_d = 0, sum_p = 0, sum_u = 0;
	for(auto i: threadTimings){
		sum_d+=i[0]; sum_p+=i[1]; sum_u+=i[2];
	}
	cout << "Mean download time: " << sum_d/download_stat.value().size() << " ms." << endl;
	cout << "Mean parse time: " << sum_p/download_stat.value().size() << " ms." << endl;
	cout << "Mean shared_var updation time: " << sum_u/download_stat.value().size() << " ms." << endl;

	cout << dashline << endl;

}

void childThread(string url, int th_no)
{
	
	clock_t t1, t2;
	double totaldTime = 0;
	double d_Time, p_Time, u_Time;

	

	// downloading the file
	t1 = clock();
	string html = myCrawler.downloader(url); // downloading html
	t2 = clock();
	d_Time = double(t2-t1)*1000 / CLOCKS_PER_SEC;

	cout << CYAN << "Thread " << th_no << " has downloaded files." << C_END << endl;

	// for calculating time of downloading
	
	t1 = clock();
	set<string> linkedSites = getLinks(html, myCrawler.maxLinks); // parsing html
	t2 = clock();
	p_Time = double(t2-t1)*1000 / CLOCKS_PER_SEC;
	cout << CYAN << "Thread " << th_no << " has extracted links." << C_END << endl;

	// updating the shared variables
	t1 = clock();
	string currDomain = getDomain(url);
	for (auto i : linkedSites)
	{
		if (!myCrawler.discoveredSites.get(i))
		{
			myCrawler.discoveredSites.inc(i);
			myCrawler.linkQueue.push(i);
			myCrawler.pageRank.add(currDomain, getDomain(i));
		}
	}
	t2 = clock();
	u_Time = double(t2-t1)*1000 / CLOCKS_PER_SEC;
	cout << CYAN << "Thread " << th_no << " has updated shared var." << C_END << endl;

	// saving time measurements for this thread
	myCrawler.timingLock.lock();
	myCrawler.threadTimings.push_back(vector<double>{d_Time, p_Time, u_Time});
	myCrawler.timingLock.unlock();


	

//
	unique_lock<mutex> lk(myCrawler.cv_m); // unique_lock for conditional variable
	myCrawler.workingThreads.add(-1);
	cout << BLUE << "Thread " << th_no << " finished, total: " << myCrawler.workingThreads.value() << C_END << endl;

    // waking up the parent thread
	if (myCrawler.pagesLimitReached.value())
	{
		if (myCrawler.workingThreads.value() < 1)
		{
			myCrawler.awake();
		}
	}
	else
	{
		myCrawler.awake();
	}

	// EXIT NOW
}

#endif