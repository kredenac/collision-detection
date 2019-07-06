#include "Logger.h"

Logger::Logger()
{
	instances.reserve(vectorCapacity);
}

Logger::~Logger()
{
}

void Logger::addInstance(float delta, float objectSize, int objectCount, std::string && bonusData)
{
	instances.push_back( std::make_tuple( delta, objectSize, objectCount, bonusData ));
}

void Logger::startNewMeasurement(std::string name)
{
	fileName = name;
	instances.resize(0);
}

void Logger::finishMeasurement()
{
	const auto fname = pathToFile + fileName + "_" + getTimeStamp() + fileExtension;
	std::fstream file;
	file.open(fname, std::fstream::out);
	for (const auto & tuple : instances) {
		float delta; float objectSize; int objectCount; std::string bonusData;
		std::tie(delta, objectSize, objectCount, bonusData) = tuple;

		if (bonusData.empty()) {
			file << delta << ", " << objectSize << ", " << objectCount << "\n";
		}
		else {
			file << delta << ", " << objectSize << ", " << objectCount << ", " << bonusData << "\n";
		}
	}

	file.close();
}

std::string Logger::getTimeStamp()
{
	time_t rawtime;

	std::time(&rawtime);
	struct tm * timeinfo = localtime(&rawtime);
	const int timeStampBufferSize = 100;
	char buffer[timeStampBufferSize];
	strftime(buffer, sizeof(buffer), "%d-%m-%Y-%H-%M-%S", timeinfo);
	std::string str(buffer);
	
	return str;
}



