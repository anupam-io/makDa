#ifndef ___CRAWLER_CPP
#define ___CRAWLER_CPP

#include "Crawler.h"

void Crawler::initialize()
{
	log.open("logs.txt");
	log << "Crawler initialized" << endl;

	log << "MAX_FILE_SIZE = " << max_file_size << endl;
	MAX_FILE_SIZE = max_file_size;
	;
	log << "TIME_OUT = " << time_out << endl;
	TIME_OUT = time_out;

	log << "srand() seed initialized." << endl;
	srand(time(0));

	log << "Curl global initialized." << endl;
	c_init();

	// initialize value of soem variables

	if (!RESTORE_DATA)
	{

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
					if (a[a.length() - 1] != '/')
					{
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
	else
	{
		system("unzip save_data.zip -d temp");

		ifstream in;

		in.open("temp/adjList.txt");
		string str;
		string key = "";
		while (true)
		{
			in >> str;
			if (str == "--END--")
			{
				break;
			}
			else if (str == "--end--")
			{
				key = "";
			}
			else if (key == "")
			{
				key = str;
			}
			else
			{
				adjList.add(key, str);
			}
		}
		in.close();
		system("rm temp/adjList.txt");

		in.open("temp/discoveredSites.txt");
		set<string> s;
		while (true)
		{
			in >> str;
			if (str == "--END--")
			{
				break;
			}
			else
			{
				discoveredSites.inc(str);
			}
		}
		in.close();
		system("rm temp/discoveredSites.txt");

		in.open("temp/linkQueue.txt");
		while (true)
		{
			in >> str;
			if (str == "--END--")
			{
				break;
			}
			else
			{
				linkQueue.push(str);
			}
		}
		in.close();
		system("rm temp/linkQueue.txt");

		system("rm -rf temp/adjList.txt temp/discoveredSites.txt linkQueue.txt");
	}
}

void Crawler::terminate()
{
	cout << "terminating." << endl;
	cout << adjList.value().size() << endl;
	cout << discoveredSites.value().size() << endl;
	cout << linkQueue.size() << endl;

	log << "Crawler terminated." << endl;
	c_finish();

	// save discoveredSites;
	// save adjList;
	// save linksQueue
	if (SAVE_DATA)
	{

		ofstream fout;
		fout.open("discoveredSites.txt");
		for (auto i : discoveredSites.value())
		{
			fout << i.first << endl;
		}
		fout << "--END--" << endl;
		fout.close();

		fout.open("adjList.txt");
		for (auto i : adjList.value())
		{
			fout << i.first << endl;
			for (auto j : i.second)
			{
				fout << j << endl;
			}
			fout << "--end--" << endl;
		}
		fout << "--END--" << endl;
		fout.close();

		fout.open("linkQueue.txt");
		while (linkQueue.size())
		{
			fout << linkQueue.pop() << endl;
		}
		fout << "--END--" << endl;
		fout.close();

		system("rm -rf save_data.zip");
		system("zip -q -r save_data.zip discoveredSites.txt adjList.txt linkQueue.txt");
		system("rm discoveredSites.txt adjList.txt linkQueue.txt");
	}

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
	ready = true;		 // because now main thread needs to be awaken
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

	cout << "Initializing runCrawler()..." << endl;
	cout << adjList.value().size() << endl;
	cout << discoveredSites.value().size() << endl;
	cout << linkQueue.size() << endl;
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

	log << "Crawling completed." << endl
			<< endl;
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
	// for calculating time of downloading
	cout << CYAN << "Thread " << th_no << " has downloaded files." << RESET << endl;

	t1 = clock();
	set<string> linkedSites = getLinks(html, myCrawler.maxLinks); // parsing html
	t2 = clock();
	p_Time = double(t2 - t1) * 1000 / CLOCKS_PER_SEC;
	// updating the shared variables
	cout << CYAN << "Thread " << th_no << " has extracted links." << RESET << endl;

	t1 = clock();
	for (auto i : linkedSites)
	{
		if (!myCrawler.discoveredSites.get(i))
		{
			cout << "Updated@@@" << endl;
			myCrawler.linkQueue.push(i);
			myCrawler.discoveredSites.inc(i);

			myCrawler.adjList.add(url, i);
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