#ifndef TEST_LIFE_H
#define TEST_LIFE_H

#include "Test.h"

#include <memory>

#include "NeuronSim/Automaton.h"

class TestLife : public Test, public Automaton::Listener
{
public:
	TestLife();
	~TestLife();

	std::string name() { return "Life"; }
	void run();

private:
	void testBasicLife();
	void testInterleavedLife();
	void testSaveLoad();

private:
	std::unique_ptr<Automaton> mAutomaton;
};

#endif
