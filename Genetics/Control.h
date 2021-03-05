#ifndef CONTROL_H
#define CONTROL_H

#include "NeuronSim/Automaton.h"

#include <random>
#include <memory>

class Control
{
private:
	struct SynapseDna
	{
		float weight;
		std::vector<uint32_t> pixels;
	};
	struct LayerDna
	{
		int spikeDuration;
		ConfigSet config;
	};
	struct Dna
	{
		float score;
		std::vector<SynapseDna> synapses;
		std::vector<LayerDna> layers;
	};
public:
	Control();
	~Control();

	void run();

private:
	void score();
	void applyDna();
	void applyNoise();
	void evaluate();
	void initializeConfigData();
	void initializeSynapseData();
	void changeAutomaton();
	void createAutomaton();
	void keepCurrent();
	void restoreBest();

private:
	std::unique_ptr<Automaton> mAutomaton;
	int mWidth;
	int mHeight;
	std::mt19937 mRandom;
	std::uniform_real_distribution<> mZeroToOne;
	std::uniform_real_distribution<> mMinusOneToOne;
	std::uniform_real_distribution<> mSmallMultiplier;
	std::uniform_int_distribution<> mSmallOffset;
	std::uniform_real_distribution<> mLargeMultiplier;
	std::uniform_int_distribution<> mLargeOffset;

	bool mSolved;
	Dna mCurrent;
	Dna mBest;
};

#endif
