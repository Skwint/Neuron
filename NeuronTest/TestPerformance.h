#ifndef TEST_PERFORMANCE_H
#define TEST_PERFORMANCE_H

#include "Test.h"

#include <memory>

#include "NeuronSim/Automaton.h"

// The performance test provides a repeatable sequence against which to 
// measure changes in performance with attempted optimizations.
// It can also help to detect mistakes that lead to slow down as a side
// effect.
// This test always passes. The relevant output is in the log files.
class TestPerformance : public Test, public Automaton::Listener
{
public:
	TestPerformance();
	~TestPerformance();

	std::string name() { return "Performance"; }
	void run();

private:
	void performance(const std::filesystem::path file);

private:
	std::unique_ptr<Automaton> mAutomaton;
};

#endif
