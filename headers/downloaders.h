// https://raw.githubusercontent.com/curl/curl/master/docs/examples/https.c

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <curl/curl.h>

using namespace std;

void c_init(){
	curl_global_init(CURL_GLOBAL_DEFAULT);
}
void c_finish(){
	curl_global_cleanup();
}

string html_downloader(string url){
	CURL *curl;
	CURLcode res;
	string out_file = "temp/" + to_string(rand());

	curl = curl_easy_init();
	if (curl)
	{
		FILE *f = fopen(out_file.c_str(), "w");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
		curl_easy_setopt(curl, CURLOPT_MAXFILESIZE, 1024*1024);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4); // timeout for the URL to download



#ifdef SKIP_PEER_VERIFICATION
		/*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificates. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif
#ifdef SKIP_HOSTNAME_VERIFICATION
		/*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		
		
		/* Check for errors */
		if (res != CURLE_OK){
			cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
			
			system(string("rm -rf "+ out_file).c_str());	
			return "FAIL";
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(f);

		cout << "File downloaded successfully." << endl;
		// return fiel now
		
		string ret;
		ifstream fin(out_file.c_str()); //taking file as inputstream
		ostringstream ss;
		ss << fin.rdbuf(); // reading data
		ret = ss.str();
		fin.close();
		system(string("rm -rf "+ out_file).c_str());	

		return ret;
	} else {
		cout << "curl_easy_init() failed." << endl;
		return "FAIL";
	}


	return NULL;
}



// int main(void)
// {
// 	// Global initialization of curl
// 	c_init();

// 	string url = "https://www.wanderlustworker.com/";
// 	string out_file = "target.html";


// 	html_download(url, out_file);
// 	string html;

// 	ifstream fin(out_file); //taking file as inputstream
// 	ostringstream ss;
// 	ss << fin.rdbuf(); // reading data
// 	html = ss.str();
// 	fin.close();

// 	// cout << html << endl;

	


// 	// After finishing all curl tasks
// 	c_finish();

// 	return 0;
// }