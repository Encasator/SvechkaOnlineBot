#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class candle
{
private:
	std::fstream logfile;
	std::map <int64_t, std::vector<std::string>> database;
public:
	candle(std::string name);
	candle(const candle& other) = delete;
	candle(candle&& rvalue) = delete;
	virtual ~candle() = default;

	std::map <int64_t, std::vector<std::string>> getDatabaseMap() const;

	void flushLogs();
	void addToDatabase(int64_t userId, const std::string& text);
	int lightCandle(std::string who, std::string reason);

};