#include <cpr/cpr.h>

#include "candle.h"

candle::candle(std::string name)
{
    logfile.open(name, std::ios::app);
    if (!logfile.is_open()) { std::cerr << "Cannot open log file" << std::endl; return; }
}

std::map <int64_t, std::vector<std::string>> candle::getDatabaseMap() const
{
    return database;
}

void candle::flushLogs() {
    for (auto& it : database) {
        logfile << "-----------------------" << std::endl;
        logfile << "Id: " << it.first << std::endl;
        for (auto& vit : it.second) {
            logfile << "Raw text: " << vit << std::endl;;
        }
        logfile << "-------------------" << std::endl;
    }
    logfile.flush();
}

void candle::addToDatabase(int64_t userId, const std::string& text) {
    database[userId].push_back(text);
}

int candle::lightCandle(std::string who, std::string rs)
{
    std::string getUrl = "https://svechka.online/wp-json/contact-form-7/v1/contact-forms/99/refill";

    cpr::Header getHeaders = {
        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:128.0) Gecko/20100101 Firefox/128.0" },
        { "Accept", "application/json, */*;q=0.1" },
        { "Accept-Language", "en-US,en;q=0.5" },
        { "Accept-Encoding", "gzip, deflate, br, zstd" },
        { "Referer", "https://svechka.online/" },
        { "DNT", "1" },
        { "Sec-GPC", "1" },
        { "Connection", "keep-alive" },
        { "Sec-Fetch-Dest", "empty" },
        { "Sec-Fetch-Mode", "cors" },
        { "Sec-Fetch-Site", "same-origin" },
        { "Priority", "u=4" },
        { "TE", "trailers" }
    };

    auto getResponse = cpr::Get(cpr::Url{ getUrl }, getHeaders);

    if (getResponse.status_code == 200) {
        std::cout << "request complete: " << getResponse.text << std::endl;
    }
    else {
        std::cout << "request error: " << getResponse.status_code << " " << getResponse.error.message << std::endl;
        return 1;
    }


    std::string postUrl = "https://svechka.online/wp-json/contact-form-7/v1/contact-forms/99/feedback";


    cpr::Multipart payload = {
        { "_wpcf7", "99" },
        { "_wpcf7_version", "5.8" },
        { "_wpcf7_locale", "ru_RU" },
        { "_wpcf7_unit_tag", "wpcf7-f99-o1" },
        { "_wpcf7_container_post", "0" },
        { "_wpcf7_posted_data_hash", "" },
        { "user_name", who },
        { "user_name_2", "" },
        { "reason", rs }
    };

    cpr::Header postHeaders = {
        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:128.0) Gecko/20100101 Firefox/128.0" },
        { "Accept", "application/json, */*;q=0.1" },
        { "Accept-Language", "en-US,en;q=0.5" },
        { "Accept-Encoding", "gzip, deflate, br, zstd" },
        { "Referer", "https://svechka.online/" },
        { "Content-Type", "multipart/form-data; boundary=---------------------------31453476151503860581409499743" },
        { "Origin", "https://svechka.online" },
        { "DNT", "1" },
        { "Sec-GPC", "1" },
        { "Connection", "keep-alive" },
        { "Sec-Fetch-Dest", "empty" },
        { "Sec-Fetch-Mode", "cors" },
        { "Sec-Fetch-Site", "same-origin" },
        { "Priority", "u=0" },
        { "TE", "trailers" }
    };


    auto postResponse = cpr::Post(cpr::Url{ postUrl }, postHeaders, payload);

    if (postResponse.status_code == 200) {
        //std::cout << "request complete: " << postResponse.text << std::endl;
        std::cout << "Свечка для " << who << " по причине " << rs << " поставлена. Спасибо за Ваше сообщение" << std::endl;
    }
    else {
        std::cout << "Request error: " << postResponse.status_code << " " << postResponse.error.message << std::endl;
    }
}
