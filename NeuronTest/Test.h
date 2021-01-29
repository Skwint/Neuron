#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <string>

// A macro for testing with is not essential, but it captures file and line
// number information for us, which makes our life a lot simpler.
#define TEST(x) { test(__FILE__, __LINE__, #x, (x)); }
#define TEST_EQUAL(l, r) { testEqual(__FILE__, __LINE__, #l, #r, l, r); }
#define TEST_APPROX_EQUAL(l, r) { testApproxEqual(__FILE__, __LINE__, #l, #r, l, r); }

// Abstract base class for tests.
// Implementations are responsible for the name() and run() functions.
// After a call to run() the passes() and fails() functions should
// return the number of successful/failed tests that were executed.
class Test
{
public:
	Test();
	virtual ~Test();

	virtual std::string name() = 0;
	virtual void run();

	bool test(const std::string & file, int line, const std::string & expr, bool value);
	template <typename T> inline bool testEqual(const std::string & file, int line, const std::string & leftStr, const std::string & rightStr, const T & left, const T & right);
	template <typename T> inline bool testApproxEqual(const std::string & file, int line, const std::string & leftStr, const std::string & rightStr, const T & left, const T & right);

	int passes() { return mPasses; }
	int fails() { return mFails; }

protected:
	int mPasses;
	int mFails;
};

template <typename T>
inline bool Test::testEqual(const std::string & file, int line, const std::string & leftStr, const std::string & rightStr, const T & left, const T & right)
{
	if (left == right)
	{
		++mPasses;
		return true;
	}
	else
	{
		std::cout << file << ":" << line << " FAILED\n";
		std::cout << "comparing: [" << leftStr << "] = " << left << "\n";
		std::cout << "       to: [" << rightStr << "] = " << right << "\n";
		++mFails;
		return false;
	}
}

template <typename T>
inline bool Test::testApproxEqual(const std::string & file, int line, const std::string & leftStr, const std::string & rightStr, const T & left, const T & right)
{
	if (approxEqual(left, right))
	{
		++mPasses;
		return true;
	}
	else
	{
		std::cout << file << ":" << line << " FAILED\n";
		std::cout << "comparing: [" << leftStr << "] = " << left << "\n";
		std::cout << "       to: [" << rightStr << "] = " << right << "\n";
		++mFails;
		return false;
	}
}

#endif
