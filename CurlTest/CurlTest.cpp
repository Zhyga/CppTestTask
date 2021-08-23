#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <thread>
#include "curl/curl.h"

void logInfo(std::string fileName, std::string message);

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

void threadRunner(std::string fileName, std::string message) {
    std::ofstream file(fileName, std::ios::app);
    file << message << std::endl;
    file.close();
}


void logInfo(std::string fileName, std::string message) {
    std::thread tr(threadRunner, fileName, message);
    tr.detach();
}

//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    const std::string fileName = argv[argc - 1];
    std::string headBuffer;
    std::string bodyBuffer;
    CURL* curl_handle = curl_easy_init();
    if (curl_handle)
    {
        const std::string URL = "https://www.google.com";
        curl_easy_setopt(curl_handle, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &bodyBuffer);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, &headBuffer);
        CURLcode res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK)
            logInfo(fileName, "curl_easy_perform() failed: %s\n" + (std::string)curl_easy_strerror(res));
        curl_easy_cleanup(curl_handle);
    }

    std::vector<std::string> lines;
    char separator = '\n';
    int token_begin = 0;
    for (int i = 0; i < headBuffer.size(); i++) {
        if (headBuffer[i] == separator) {
            lines.push_back(headBuffer.substr(token_begin, i - token_begin));
            token_begin = i + 1;
        }
    }

    std::regex dateRegex("date:(.*)");
    std::string dateString;
    for (std::string line : lines) {
        if (std::regex_search(line, dateRegex)) {
            dateString = line;
            break;
        }
    }
    if (dateString.empty()) {
        logInfo(fileName, "Cant find date");
    }

    dateString = dateString.substr(11, dateString.size() - 12);
    struct tm* tm;
    std::time_t a = getEpochTime(dateString);
    tm = localtime(&a);
    struct timeval tv;
    tv.tv_sec = a;
    tv.tv_usec = 0;

    if (settimeofday(&tv, NULL) < 0) {
        logInfo(fileName, "time and date is not setted");
    }

    curl_handle = curl_easy_init();
    if (curl_handle)
    {
        const std::string URL = "https://example.com";
        curl_easy_setopt(curl_handle, CURLOPT_URL, URL.c_str());
        CURLcode res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK)
            logInfo(fileName, "curl_easy_perform() failed: %s\n" + (std::string)curl_easy_strerror(res));
        curl_easy_cleanup(curl_handle);
    }
    return 0;
}