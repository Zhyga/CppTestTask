#define CURL_STATICLIB
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "curl/curl.h"

std::time_t getEpochTime(const std::string& dateTime)
{
    static const std::string dateTimeFormat("%d %b %Y %H:%M:%S %Z");
    std::istringstream ss{ dateTime };
    std::tm dt;
    ss >> std::get_time(&dt, dateTimeFormat.c_str());
    return std::mktime(&dt);
}

size_t write_data(char* ptr, size_t size, size_t nmemb, void* data)
{
    size_t realsize = size * nmemb;
    ((std::string*)data)->append(ptr, realsize);
    return realsize;
}
//---------------------------------------------------------------------------
int main()
{
    std::string readBuffer;
    CURL* curl_handle = curl_easy_init();
    if (curl_handle)
    {

        const std::string URL = "https://www.google.com";
        curl_easy_setopt(curl_handle, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, &readBuffer);
        CURLcode res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK)
            std::cout << "curl_easy_perform() failed: %s\n" << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl_handle);
    }
    std::regex dateRegex("Date:(.*)");
    std::vector<std::string> lines;
    char separator = '\n';
    int token_begin = 0;
    for (int i = 0; i < readBuffer.size(); i++) {
        if (readBuffer[i] == separator) {
            lines.push_back(readBuffer.substr(token_begin, i - token_begin));
            token_begin = i + 1;
        }
    }
    std::string dateString;
    for (std::string line : lines) {
        if (std::regex_search(line, dateRegex)) {
            dateString = line;
            break;
        }
    }
    dateString = dateString.substr(11, dateString.size()-12);

    struct tm* tm;
    std::time_t a = getEpochTime(dateString);
    //std::time(&a);
    tm = localtime(&a);
    tm->tm_year += 1900;
    //tm->tm_mon -= 1;
    a = mktime(tm);
    //strftime("21 Aug 2021 21:12:59 GMT", "%d %b %Y %H:%M:%S %Z", &tm); for linux
    
    std::cout << dateString;
    getchar();
    return 0;
}