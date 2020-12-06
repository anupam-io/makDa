#ifndef ___CRAWLER_CPP
#define ___CRAWLER_CPP

#include "Crawler.h"

void Crawler::initialize()
{
	log.open("logs.txt");
	log << "Crawler initialized" << endl;

	log << "MAX_FILE_SIZE = " << max_file_size << endl;
	MAX_FILE_SIZE = max_file_size;;
	log << "TIME_OUT = " <<  time_out << endl;
	TIME_OUT = time_out;


	log << "srand() seed initialized." << endl;
	srand(time(0));

	log << "Curl global initialized." << endl;
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
				if(a[a.length()-1]!='/'){
					a.push_back('/');
				}
				linkQueue.push(a);
				discoveredSites.inc(a);
			}
			else
			{
				break;
			}
		}
		log << "Links read from file \"INPUT/initialLinks.txt\"  " << endl;
	}
	else
	{
		// Unable to read starting links from input file
		log << "Error reading file: \"initialLinks.txt\"" << endl;
		linkQueue.push("https://www.google.com/");
	}
	log << endl;
}

void Crawler::terminate()
{
	log << "Crawler terminated." << endl;
	c_finish();
	log.close();
}

string Crawler::downloader(string url)
{
	auto res = html_downloader(url);
	if (res.first == true)
	{
		download_stat.inc("Success");
		return res.second;
	}
	else
	{
		download_stat.inc(res.second);
		return "";
	}

	return "";
}

void Crawler::gotosleep()
{
	unique_lock<mutex> lk(cv_m); // unique_lock for conditional variable
	cout << "Going to sleep now" << endl;

	ready = false; // because main thread is now not ready to process any data

	// written in a while loop, if spuriously woken up
	while (!ready)
		cv.wait(lk); // Waiting until not ready

	cout << "Awaken now." << endl;
}

void Crawler::awake()
{
	ready = true;	 // because now main thread needs to be awaken
	cv.notify_one(); // notifying the main thread which is sleeping
}

void Crawler::createThread()
{
	string currentSite = linkQueue.pop();
	totalVisitedPages.add(1);
	workingThreads.add(1);

	log << currentSite << endl;
	cout << GREEN << "Creating a thread, total: " << workingThreads.value() << RESET << endl;

	// update maxPagesReached

	if (totalVisitedPages.value() >= maxPages)
	{
		maxPagesReached.add(1);
		cout << RED << "~!!!pagesLimit Reached here.!!!~" << RESET << endl;
	}

	// create thread
	thread th = thread(childThread, currentSite, totalVisitedPages.value());
	th.detach();
}

void Crawler::runCrawler()
{
	while (true)
	{
		if (maxPagesReached.value())
		{
			if (workingThreads.value() < 1)
			{
				// exiting

				cout << RED << "Exiting as all threads are finished & pagelimit reached." << RESET << endl;

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

	log << "Crawling completed." << endl << endl;
}

/*
*/
void Crawler::showResults()
{
	string dashline = string(70, '-');

	cout << dashline << endl;
	cout << "DOWNLOADER ANALYTICS:" << endl;
	for (auto i : download_stat.value())
	{
		cout << i.first << ": " << i.second << endl;
	}
	cout << dashline << endl;
	cout << "CHILD THREAD ANALYTICS:" << endl;
	long long sum_d = 0, sum_p = 0, sum_u = 0;
	for (auto i : threadTimings)
	{
		sum_d += i[0];
		sum_p += i[1];
		sum_u += i[2];
	}
	cout << "Mean download time: " << sum_d / download_stat.value().size() << " ms." << endl;
	cout << "Mean parse time: " << sum_p / download_stat.value().size() << " ms." << endl;
	cout << "Mean shared_var updation time: " << sum_u / download_stat.value().size() << " ms." << endl;

	cout << dashline << endl;

	map<string, int> m;
	for (auto i : adjList.value())
	{
		for (auto j : i.second)
		{
			m[getDomain(j)]++;
		}
	}


	vector<pair<string, int>> v(m.begin(), m.end());
	sort(v.begin(), v.end(), [](auto a, auto b) { return a.second > b.second; });
	int xx = 0;
	for (auto i : v)
	{
		cout << "|   " << ++xx << ". "
			 << i.first << ": "
			 << string(57 - i.first.length() - int(log10(xx) + 1), ' ')
			 << i.second << "   |" << endl;
	}

	cout << dashline << endl;
}

void childThread(string url, int th_no)
{

	clock_t t1, t2;
	double d_Time, p_Time, u_Time;

	// downloading the file
	t1 = clock();
	string html = myCrawler.downloader(url); // downloading html
	t2 = clock();
	d_Time = double(t2 - t1) * 1000 / CLOCKS_PER_SEC;

	cout << CYAN << "Thread " << th_no << " has downloaded files." << RESET << endl;

	// for calculating time of downloading

	t1 = clock();
	set<string> linkedSites = getLinks(html, myCrawler.maxLinks); // parsing html
	t2 = clock();
	p_Time = double(t2 - t1) * 1000 / CLOCKS_PER_SEC;
	cout << CYAN << "Thread " << th_no << " has extracted links." << RESET << endl;

	// updating the shared variables
	t1 = clock();
	string currDomain = getDomain(url);
	for (auto i : linkedSites)
	{
		if (!myCrawler.discoveredSites.get(i))
		{
			myCrawler.linkQueue.push(i);
			myCrawler.discoveredSites.inc(i);

			myCrawler.adjList.add(currDomain, i);
		}
	}
	t2 = clock();
	u_Time = double(t2 - t1) * 1000 / CLOCKS_PER_SEC;
	cout << CYAN << "Thread " << th_no << " has updated shared var." << RESET << endl;

	// saving time measurements for this thread
	myCrawler.timingLock.lock();
	myCrawler.threadTimings.push_back(vector<double>{d_Time, p_Time, u_Time});
	myCrawler.timingLock.unlock();

	//
	unique_lock<mutex> lk(myCrawler.cv_m); // unique_lock for conditional variable
	myCrawler.workingThreads.add(-1);
	cout << BLUE << "Thread " << th_no << " finished, total: " << myCrawler.workingThreads.value() << RESET << endl;

	// waking up the parent thread
	if (myCrawler.maxPagesReached.value())
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