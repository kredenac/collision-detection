#pragma once
#include <iostream>
#include <ctime>
#include <vector>
#include <fstream>
#include <string>
#include <tuple>

// collects delta times of each frame and saves them as .csv file
class Logger
{
public:
	Logger();
	~Logger();

	// add a delta time to be saved
	void addInstance(float delta, float objectSize, int objectCount, std::string && additionalData);

	// start a new measurement to be stored in file with the given name
	void startNewMeasurement(std::string fileName);

	// finish the measurement and store it in the .csv file
	void finishMeasurement();

private:
	std::string getTimeStamp();

	std::vector<std::tuple<float, float, int, std::string>> instances;
	const int vectorCapacity = 60 * 60 * 10;
	std::string fileName;
	const std::string pathToFile = "../../measurements/";
	const std::string fileExtension = ".csv";
};

