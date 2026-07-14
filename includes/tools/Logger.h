#pragma once
#include <cstdio>
#include <glm/glm.hpp>

class Logger
{
public:
	template <typename... Args>
	static void log(unsigned int logLevel, Args ... args)
	{
		if (logLevel <= mLogLevel)
		{
			std::printf(args ...);
			std::fflush(stdout);
		}
	}
	static void setLogLevel(unsigned int inLogLevel)
	{
		inLogLevel <= 9 ? mLogLevel = inLogLevel : mLogLevel = 9;
	}

	static void logm4(unsigned int inlogLevel, const glm::mat4& M)
	{
		std::printf(
			"%f %f %f %f\n"
			"%f %f %f %f\n"
			"%f %f %f %f\n"
			"%f %f %f %f\n",
			M[0][0], M[1][0], M[2][0], M[3][0],
			M[0][1], M[1][1], M[2][1], M[3][1],
			M[0][2], M[1][2], M[2][2], M[3][2],
			M[0][3], M[1][3], M[2][3], M[3][3]);
			std::fflush(stdout);
	}

	static void logv3(unsigned int logLevel, const glm::vec3& v)
	{
		std::printf("%f %f %f\n", v.x, v.y, v.z);
		std::fflush(stdout);
	}


private:
	static unsigned int mLogLevel;
};