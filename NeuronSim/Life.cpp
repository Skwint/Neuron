#include "Life.h"

Life::Life(int width, int height) :
	Net(width, height)
{

}

Life::~Life()
{

}

std::string Life::name()
{
	return "Life";
}

void Life::tick()
{
	for (int rr = 0; rr < mHeight; ++rr)
	{
		NeuronLife * cell = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			cell->input = 0.0f;
			++cell;
		}
	}

	for (int rr = 0; rr < mHeight; ++rr)
	{
		NeuronLife * src = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			for (int tr = -1; tr < 2; ++tr)
			{
				NeuronLife * dst = src + tr * rowStep() - 1;
				for (int tc = -1; tc < 2; ++tc)
				{
					if (tc || tr)
						dst->input += src->output;
					else
						dst->input += 0.5f * src->output;
					++dst;
				}
			}
			++src;
		}
	}

	for (int rr = 0; rr < mHeight; ++rr)
	{
		NeuronLife * cell = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			if (cell->input > 2.25f && cell->input < 3.75f)
			{
				cell->output = 1.0f;
			}
			else
			{
				cell->output = 0.0f;
			}
			++cell;
		}
	}
}
