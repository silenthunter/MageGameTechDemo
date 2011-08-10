#include "WorldTerrain.h"

using std::cout;
using std::endl;
using std::mt19937;
using std::uniform_int;
using std::uniform_real;
using std::variate_generator;

using anl::FBM;
using anl::GRADIENT;
using anl::QUINTIC;
using anl::RIDGEDMULTI;
using anl::BILLOW;
using anl::MULT;

WorldTerrain::WorldTerrain()
	:
	const0(0),
	constStone(5),

	//Ground

	//Plain
	plainFractal(FBM, GRADIENT, QUINTIC),
	plainScaleOff(0.2, -0.07),
	//Rolling Plain
	rollingFractal(FBM, GRADIENT, QUINTIC),
	rollingScaleOff(0.4, -0.07),
	//Highland
	highlandFractal(RIDGEDMULTI, GRADIENT, QUINTIC),
	highlandScaleOff(0.45, -0.85),
	//Mountain
	mountainFractal(BILLOW, GRADIENT, QUINTIC),
	mountainScaleOff(0.75, -2.025),
	//Chasm
	chasmFractal(BILLOW, GRADIENT, QUINTIC),
	chasmScaleOff(0.75, 1.1),
	//Terrain type fractal for select control
	plain_rolling_Control(FBM, GRADIENT, QUINTIC),
	highland_mountain_Control(FBM, GRADIENT, QUINTIC),
	plain_rolling_highland_mountain_Control(FBM, GRADIENT, QUINTIC),
	plain_rolling_highland_mountain_chasm_Control(FBM, GRADIENT, QUINTIC),

	//Caves

	caveBias(0.45),
	caveShape1(RIDGEDMULTI, GRADIENT, QUINTIC),
	caveShape2(RIDGEDMULTI, GRADIENT, QUINTIC),
	caveShapeBias(MULT),
	cavePerturb(FBM, GRADIENT, QUINTIC),
	cavePerturbScaleOff(0.5, 0.0),

	worldTerrain(MULT)
{
}

WorldTerrain::~WorldTerrain()
{
}

void WorldTerrain::Init(int seedVar)
{
	currSeed = seedVar;
}

void WorldTerrain::InputNewBoundary(unsigned short width, unsigned short depth, unsigned short height)
{
	currWidth = width;
	currDepth = depth;
	currHeight = height;
}

void WorldTerrain::GenerateRegularWorld()
{
	GenerateGround();
	GenerateCaves();
	worldTerrain.setSource(0, &groundSelect);
	worldTerrain.setSource(1, &caveSelect);
}

void WorldTerrain::TestFractal(double low, double high)
{
	double mx=-10000;
	double mn=10000;

	for(int c=0; c<10000; ++c)
	{
			double nx=(double)rand()/(double)RAND_MAX * 4.0;
			double ny=(double)rand()/(double)RAND_MAX * 4.0;
			double nz=(double)rand()/(double)RAND_MAX * 4.0;
			double v=plainFractal.get(nx,ny,nz);
			if(v>mx) mx=v;
			if(v<mn) mn=v;
	}

	double scale=(high - low) / (mx-mn);
    double offset=low-mn*scale;

	cout << "min: " << mn << endl;
	cout << "max: " << mx << endl;
	cout << "scale: " << scale << endl;
	cout << "offset: " << offset << endl;
}

void WorldTerrain::GenerateGround()
{
	groundGradient.setGradient(0, 0, 0, 8, 0, 0);

	//Plain
	plainFractal.setSeed(currSeed);
	plainFractal.setNumOctaves(2);
	plainFractal.setFrequency(1);

	plainScaleOff.setSource(&plainFractal);

	plainScaleDomain.setSource(&plainScaleOff);
	plainScaleDomain.setYScale(0.0);

	plainTerrain.setSeed(currSeed + 11);
	plainTerrain.setSource(&groundGradient);
	plainTerrain.setYAxisSource(&plainScaleDomain);

	//Rolling Plain
	rollingFractal.setSeed(currSeed + 22);
	rollingFractal.setNumOctaves(2);
	rollingFractal.setFrequency(1);

	rollingScaleOff.setSource(&rollingFractal);

	rollingScaleDomain.setSource(&rollingScaleOff);
	rollingScaleDomain.setYScale(0.0);

	rollingTerrain.setSeed(currSeed + 33);
	rollingTerrain.setSource(&groundGradient);
	rollingTerrain.setYAxisSource(&rollingScaleDomain);

	//Highland (threshold -0.11)
	highlandFractal.setSeed(currSeed + 44);
	highlandFractal.setNumOctaves(2);
	highlandFractal.setFrequency(1);

	highlandScaleOff.setSource(&highlandFractal);

	highlandScaleDomain.setSource(&highlandScaleOff);
	highlandScaleDomain.setYScale(0.0);

	highlandTerrain.setSeed(currSeed + 55);
	highlandTerrain.setSource(&groundGradient);
	highlandTerrain.setYAxisSource(&highlandScaleDomain);

	//Mountain
	mountainFractal.setSeed(currSeed + 66);
	mountainFractal.setNumOctaves(4);
	mountainFractal.setFrequency(0.5);

	mountainScaleOff.setSource(&mountainFractal);

	mountainScaleDomain.setSource(&mountainScaleOff);
	mountainScaleDomain.setYScale(0.1);

	mountainTerrain.setSeed(currSeed + 77);
	mountainTerrain.setSource(&groundGradient);
	mountainTerrain.setYAxisSource(&mountainScaleDomain);

	//Chasm
	chasmFractal.setSeed(currSeed + 88);
	chasmFractal.setNumOctaves(4);
	chasmFractal.setFrequency(1);

	chasmScaleOff.setSource(&chasmFractal);

	chasmScaleDomain.setSource(&chasmScaleOff);
	chasmScaleDomain.setYScale(0.1);

	chasmTerrain.setSeed(currSeed + 99);
	chasmTerrain.setSource(&groundGradient);
	chasmTerrain.setYAxisSource(&chasmScaleDomain);

	//--------------------------------------------------------------------
	//Terrain type fractal for select control

	//Plain + Rolling Plain
	plain_rolling_Control.setSeed(currSeed + 110);
	plain_rolling_Control.setNumOctaves(3);
	plain_rolling_Control.setFrequency(0.5);

	plain_rolling_scaleDomain.setSource(&plain_rolling_Control);
	plain_rolling_scaleDomain.setYScale(0.1);

	//Highland + Mountain
	highland_mountain_Control.setSeed(currSeed + 121);
	highland_mountain_Control.setNumOctaves(3);
	highland_mountain_Control.setFrequency(0.5);

	highland_mountain_scaleDomain.setSource(&highland_mountain_Control);
	highland_mountain_scaleDomain.setYScale(0.1);

	//Plain + Rolling Plain + Highland + Mountain
	plain_rolling_highland_mountain_Control.setSeed(currSeed + 132);
	plain_rolling_highland_mountain_Control.setNumOctaves(3);
	plain_rolling_highland_mountain_Control.setFrequency(0.5);

	plain_rolling_highland_mountain_scaleDomain.setSource(&plain_rolling_highland_mountain_Control);
	plain_rolling_highland_mountain_scaleDomain.setYScale(0.1);

	//Plain + Rolling Plain + Highland + Mountain + Chasm
	plain_rolling_highland_mountain_chasm_Control.setSeed(currSeed + 143);
	plain_rolling_highland_mountain_chasm_Control.setNumOctaves(3);
	plain_rolling_highland_mountain_chasm_Control.setFrequency(0.5);

	plain_rolling_highland_mountain_chasm_scaleDomain.setSource(&plain_rolling_highland_mountain_chasm_Control);
	plain_rolling_highland_mountain_chasm_scaleDomain.setYScale(0.1);
	
	//Selects

	//Plain + Highland + Mountain
	plain_rolling_select.setLowSource(&plainTerrain);
	plain_rolling_select.setHighSource(&rollingTerrain);
	plain_rolling_select.setControlSource(&plain_rolling_scaleDomain);
	plain_rolling_select.setThreshold(0.1);
	plain_rolling_select.setFalloff(0.15);

	//Highland + Mountain
	highland_mountain_select.setLowSource(&highlandTerrain);
	highland_mountain_select.setHighSource(&mountainTerrain);
	highland_mountain_select.setControlSource(&highland_mountain_scaleDomain);
	highland_mountain_select.setThreshold(0.2);
	highland_mountain_select.setFalloff(0.15);

	//Plain + Rolling + Highland + Mountain
	plain_rolling_highland_mountain_select.setLowSource(&plain_rolling_select);
	plain_rolling_highland_mountain_select.setHighSource(&highland_mountain_select);
	plain_rolling_highland_mountain_select.setControlSource(&plain_rolling_highland_mountain_scaleDomain);
	plain_rolling_highland_mountain_select.setThreshold(0.25);
	plain_rolling_highland_mountain_select.setFalloff(0.15);

	//Plain + Rolling + Highland + Mountain + Chasm
	plain_rolling_highland_mountain_chasm_select.setLowSource(&plain_rolling_highland_mountain_select);
	plain_rolling_highland_mountain_chasm_select.setHighSource(&chasmTerrain);
	plain_rolling_highland_mountain_chasm_select.setControlSource(&plain_rolling_highland_mountain_chasm_scaleDomain);
	plain_rolling_highland_mountain_chasm_select.setThreshold(0.4);
	plain_rolling_highland_mountain_chasm_select.setFalloff(0.15);

	//--------------------------------------------------------------------
	//Final Ground Terrain
	groundTerrain.setSource(&plain_rolling_highland_mountain_chasm_select);

	groundSelect.setLowSource(&const0);
	groundSelect.setHighSource(&constStone);
	groundSelect.setControlSource(&groundTerrain);
	groundSelect.setThreshold(0.0);
}


void WorldTerrain::GenerateCaves()
{
	caveBias.setSource(&groundTerrain);

	caveShape1.setSeed(currSeed + 7);
	caveShape1.setNumOctaves(1);
	caveShape1.setFrequency(4);

	caveShape2.setSeed(currSeed + 14);
	caveShape2.setNumOctaves(1);
	caveShape2.setFrequency(4);

	caveShapeBias.setSource(0, &caveShape1);
	caveShapeBias.setSource(1, &caveBias);
	caveShapeBias.setSource(2, &caveShape2);

	cavePerturb.setSeed(currSeed + 21);
	cavePerturb.setNumOctaves(6);
	cavePerturb.setFrequency(3);

	cavePerturbScaleOff.setSource(&cavePerturb);

	caveTranslateDomain.setSource(&caveShapeBias);
	caveTranslateDomain.setXAxisSource(&cavePerturbScaleOff);

	caveSelect.setLowSource(&constStone);
	caveSelect.setHighSource(&const0);
	caveSelect.setControlSource(&caveTranslateDomain);
	caveSelect.setThreshold(0.25);
}