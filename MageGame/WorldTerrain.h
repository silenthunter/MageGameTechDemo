#pragma once
#include <time.h>
#include <iostream>
#include <random>

#include "GameGlobals.h"

#include "implicitconstant.h"
#include "implicitgradient.h"
#include "implicitfractal.h"
#include "implicitscaleoffset.h"
#include "implicitscaledomain.h"
#include "implicittranslatedomain.h"
#include "implicitselect.h"
#include "implicitcache.h"
#include "implicitcombiner.h"
#include "implicitbias.h"

class WorldTerrain
{
public:
	//Functions
	WorldTerrain();
	~WorldTerrain();
	void Init(int seedVar = time(0)); //Input a custom seed or leave it blank for a random seed using time(0)
	void InputNewBoundary(unsigned short width, unsigned short depth, unsigned short height);
	void GenerateRegularWorld();

	//Variables
	anl::CImplicitCombiner worldTerrain;
	unsigned short currWidth;
	unsigned short currDepth;
	unsigned short currHeight;

private:
	//Functions
	void TestFractal(double low, double high);

	void GenerateGround();
	void GenerateCaves();
	
	//Variables
	int currSeed;
	anl::CImplicitConstant const0;
	anl::CImplicitConstant constStone;

	anl::CImplicitGradient groundGradient;

	//Ground
	//Plain
	anl::CImplicitFractal plainFractal;
	anl::CImplicitScaleOffset plainScaleOff;
	anl::CImplicitScaleDomain plainScaleDomain;
	anl::CImplicitTranslateDomain plainTerrain;

	//Rolling Plain
	anl::CImplicitFractal rollingFractal;
	anl::CImplicitScaleOffset rollingScaleOff;
	anl::CImplicitScaleDomain rollingScaleDomain;
	anl::CImplicitTranslateDomain rollingTerrain;

	//Highland
	anl::CImplicitFractal highlandFractal;
	anl::CImplicitScaleOffset highlandScaleOff;
	anl::CImplicitScaleDomain highlandScaleDomain;
	anl::CImplicitTranslateDomain highlandTerrain;

	//Mountain
	anl::CImplicitFractal mountainFractal;
	anl::CImplicitScaleOffset mountainScaleOff;
	anl::CImplicitScaleDomain mountainScaleDomain;
	anl::CImplicitTranslateDomain mountainTerrain;

	//Chasm
	anl::CImplicitFractal chasmFractal;
	anl::CImplicitScaleOffset chasmScaleOff;
	anl::CImplicitScaleDomain chasmScaleDomain;
	anl::CImplicitTranslateDomain chasmTerrain;

	//Terrain Type (for Select)
	anl::CImplicitFractal plain_rolling_Control;
	anl::CImplicitScaleDomain plain_rolling_scaleDomain;
	anl::CImplicitFractal highland_mountain_Control;
	anl::CImplicitScaleDomain highland_mountain_scaleDomain;
	anl::CImplicitFractal plain_rolling_highland_mountain_Control;
	anl::CImplicitScaleDomain plain_rolling_highland_mountain_scaleDomain;
	anl::CImplicitFractal plain_rolling_highland_mountain_chasm_Control;
	anl::CImplicitScaleDomain plain_rolling_highland_mountain_chasm_scaleDomain;

	anl::CImplicitSelect plain_rolling_select;
	anl::CImplicitSelect highland_mountain_select;
	anl::CImplicitSelect plain_rolling_highland_mountain_select;
	anl::CImplicitSelect plain_rolling_highland_mountain_chasm_select;

	anl::CImplicitCache groundTerrain;

	anl::CImplicitSelect groundSelect;


	//Caves
	anl::CImplicitBias caveBias;
	anl::CImplicitFractal caveShape1;
	anl::CImplicitFractal caveShape2;
	anl::CImplicitCombiner caveShapeBias;
	anl::CImplicitFractal cavePerturb;
	anl::CImplicitScaleOffset cavePerturbScaleOff;
	anl::CImplicitTranslateDomain caveTranslateDomain;
	anl::CImplicitSelect caveSelect;
};