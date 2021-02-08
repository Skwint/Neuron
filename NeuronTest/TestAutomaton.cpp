#include "TestAutomaton.h"

#include "NeuronSim/Layer.h"
#include "NeuronSim/SynapseMatrix.h"

using namespace std;

TestAutomaton::TestAutomaton()
{
	mAutomaton = make_unique<Automaton>();
	mAutomaton->addListener(this);
}

TestAutomaton::~TestAutomaton()
{
	mAutomaton->removeListener(this);
}

void TestAutomaton::run()
{
	Test::run();

	testTypeChangeCallback();
	testSizeChangeCallback();
	testLayerCreatedRemovedCallbacks();
	testSynapseCreatedRemovedCallbacks();
	testAutoSynapseRemoval();
	testLayerResize();
}

void TestAutomaton::testTypeChangeCallback()
{
	TEST_SUB;
	mAutomaton->setNetworkType(mAutomaton->typeNames()[0]);
	resetChanges();
	mAutomaton->setNetworkType(mAutomaton->typeNames()[1]);
	TEST(mTypeChanged = true);
	mTypeChanged = false;
	checkNothingChanged();
}

void TestAutomaton::testSizeChangeCallback()
{
	TEST_SUB;
	mAutomaton->setSize(64,64);
	resetChanges();
	mAutomaton->setSize(100,200);
	TEST_EQUAL(mWidthChanged, 100);
	TEST_EQUAL(mHeightChanged, 200);

	mWidthChanged = 0;
	mHeightChanged = 0;
	checkNothingChanged();
}

void TestAutomaton::testLayerCreatedRemovedCallbacks()
{
	TEST_SUB;
	resetChanges();

	mLayer1 = mAutomaton->createLayer();
	TEST_EQUAL(mLayer1, mLayerChanged);

	mLayerChanged.reset();
	checkNothingChanged();

	mAutomaton->removeLayer(mLayer1);
	TEST_EQUAL(mLayer1, mLayerChanged);

	mLayerChanged.reset();
	checkNothingChanged();
}

void TestAutomaton::testSynapseCreatedRemovedCallbacks()
{
	TEST_SUB;
	mLayer1 = mAutomaton->createLayer();
	resetChanges();

	mSynapses1 = mAutomaton->createSynapse();
	TEST_EQUAL(mSynapses1, mSynapsesChanged);

	mSynapsesChanged.reset();
	checkNothingChanged();

	mAutomaton->removeSynapse(mSynapses1);
	TEST_EQUAL(mSynapses1, mSynapsesChanged);

	mSynapsesChanged.reset();
	checkNothingChanged();

	mAutomaton->removeLayer(mLayer1);
}

void TestAutomaton::testAutoSynapseRemoval()
{
	TEST_SUB;
	mLayer1 = mAutomaton->createLayer();
	mLayer2 = mAutomaton->createLayer();
	mSynapses1 = mAutomaton->createSynapse();
	mSynapses1->setSource(mLayer1);
	mSynapses1->setTarget(mLayer1);
	mSynapses2 = mAutomaton->createSynapse();
	mSynapses2->setSource(mLayer2);
	mSynapses2->setTarget(mLayer2);

	resetChanges();
	mAutomaton->removeLayer(mLayer2);
	TEST_EQUAL(mLayerChanged, mLayer2);
	TEST_EQUAL(mSynapsesChanged, mSynapses2);
	mAutomaton->removeLayer(mLayer1);
	TEST_EQUAL(mLayerChanged, mLayer1);
	TEST_EQUAL(mSynapsesChanged, mSynapses1);
	mLayerChanged.reset();
	mSynapsesChanged.reset();
	checkNothingChanged();
}

void TestAutomaton::testLayerResize()
{
	TEST_SUB;
	mAutomaton->setSize(64, 64);
	mLayer1 = mAutomaton->createLayer();
	mLayer2 = mAutomaton->createLayer();
	mAutomaton->setSize(128, 128);
	TEST_EQUAL(mLayer1->width(), 128);
	TEST_EQUAL(mLayer1->height(), 128);
	TEST_EQUAL(mLayer2->width(), 128);
	TEST_EQUAL(mLayer2->height(), 128);
}

void TestAutomaton::automatonTypeChanged()
{
	mTypeChanged = true;
}

void TestAutomaton::automatonSizechanged(int width, int height)
{
	mWidthChanged = width;
	mHeightChanged = height;
}

void TestAutomaton::automatonLayerCreated(std::shared_ptr<Layer> layer)
{
	mLayerChanged = layer;
}

void TestAutomaton::automatonLayerRemoved(std::shared_ptr<Layer> layer)
{
	mLayerChanged = layer;
}

void TestAutomaton::automatonSynapsesCreated(std::shared_ptr<SynapseMatrix> synapses)
{
	mSynapsesChanged = synapses;
}

void TestAutomaton::automatonSynapsesRemoved(std::shared_ptr<SynapseMatrix> synapses)
{
	mSynapsesChanged = synapses;
}

void TestAutomaton::resetChanges()
{
	mTypeChanged = false;
	mWidthChanged = 0;
	mHeightChanged = 0;
	mLayerChanged.reset();
	mSynapsesChanged.reset();
}

void TestAutomaton::checkNothingChanged()
{
	TEST(!mTypeChanged);
	TEST(!mWidthChanged);
	TEST(!mHeightChanged);
	TEST(!mLayerChanged);
	TEST(!mSynapsesChanged);
}
