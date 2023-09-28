#include <iostream>
#include <thread>
#include <mutex>
#include <cmath>
#include <chrono>
#include <random>

// High resolution time calculation
using HRClock = std::chrono::high_resolution_clock;
using TimespanMS = std::chrono::duration<double, std::milli>;
//

/// <summary>
/// Convert integer to 26 based number system then transform every number 
/// to ASCII letter by adding it as offset to 'a' character.
/// </summary>
/// <param name="num"> - number in range from 0 to 26^5</param>
/// <returns>string of length 5 from ASCII characters in range 'a-z'</returns>
std::string intToStr(int num) {
	std::string res(5, 'a');
	for (int i = 4; num != 0; i--) {
		res[i] = 'a' + (num % 26);
		num /= 26;
	}
	return res;
}

// must be used as stdout is shared between threads
std::mutex mutex;

/// <summary>
/// Goes through all possible combinations of 5 letters until it finds combination equal to 'password'
/// </summary>
/// <param name="start"> - start position</param>
/// <param name="end"> - end position</param>
/// <param name="password"> - searched password</param>
/// <param name="test"> - number of test</param>
void bruteForce(int start, int end, std::string password, int test) {
	mutex.lock();
	std::cout << "[Test " << test << "] Thread " << std::this_thread::get_id() << " started" << std::endl;
	mutex.unlock();
	HRClock::time_point startTime = HRClock::now();
	bool hacked = false;
	while (start != end)
	{
		if (intToStr(start) == password) {
			hacked = true;
			break;
		}
		start++;
	}
	HRClock::time_point endTime = HRClock::now();
	TimespanMS time = endTime - startTime;
	mutex.lock();
	std::cout << "[Test " << test << "] Thread " << std::this_thread::get_id() << " ";
	if (hacked) {
		endTime = HRClock::now();
		std::cout << "hacked password: " << intToStr(start) << std::endl;
		std::cout << "[Test " << test << "] Elapsed time, ms: " << time.count() << std::endl;
	}
	else {
		std::cout << "failed" << std::endl;
	}
	mutex.unlock();
}

/// <summary>
/// Runs brute force on single thread
/// </summary>
/// <param name="start"> - start position</param>
/// <param name="end"> - end position</param>
/// <param name="password"> - searched password</param>
void test1(int start, int end, std::string password) {
	std::cout << "[Test 1] Begin" << std::endl;
	std::thread th1(bruteForce, start, end, password, 1);
	th1.join();
	std::cout << "[Test 1] End" << std::endl;
}

/// <summary>
/// Runs brute force on two threads
/// </summary>
/// <param name="start"> - start position</param>
/// <param name="end"> - end position</param>
/// <param name="password"> - searched password</param>
void test2(int start, int end, std::string password) {
	std::cout << "[Test 2] Begin" << std::endl;
	std::thread th1(bruteForce, start, end / 2, password, 2);
	std::thread th2(bruteForce, end / 2, end, password, 2);
	th1.join();
	th2.join();
	std::cout << "[Test 2] End" << std::endl;
}

/// <summary>
/// Runs brute force on four threads
/// </summary>
/// <param name="start"> - start position</param>
/// <param name="end"> - end position</param>
/// <param name="password"> - searched password</param>
void test3(int start, int end, std::string password) {
	std::cout << "[Test 3] Begin" << std::endl;
	std::thread th1(bruteForce, start, end / 4, password, 3);
	std::thread th2(bruteForce, end / 4, end / 2, password, 3);
	std::thread th3(bruteForce, end / 2, end / 4 * 3, password, 3);
	std::thread th4(bruteForce, end / 4 * 3, end, password, 3);
	th1.join();
	th2.join();
	th3.join();
	th4.join();
	std::cout << "[Test 3] End" << std::endl;
}

int main() {
	int start = 0;
	int end = std::pow(26, 5);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> range(start, end);
	std::string password = intToStr(range(gen));
	std::cout << "Random password: " << password << std::endl;
	test1(start, end, password);
	test2(start, end, password);
	test3(start, end, password);
}