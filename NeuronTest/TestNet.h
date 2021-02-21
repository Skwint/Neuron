#ifndef TEST_NET_H
#define TEST_NET_H

#include "Test.h"

#include "NeuronSim/SynapseMatrix.h"

class TestNet : public Test, public SynapseMatrix::Listener
{
public:
	TestNet();
	~TestNet();

	std::string name() { return "Net"; }
	void run();

private:
	void synapseMatrixChanged(SynapseMatrix * matrix) override {}
};

#endif
