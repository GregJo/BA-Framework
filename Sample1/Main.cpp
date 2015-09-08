#include "Logger.h"

int main()
{
	// Logger Test
	Logger::GetInstance().Init("Sample1 Log");
	Logger::GetInstance().IsInitialized();
	Logger::GetInstance().Log("Test Log");

	return 0;
}