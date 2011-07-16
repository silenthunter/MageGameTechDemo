#include "HillTurbulence.h"

using noise::module::HillTurbulence;

HillTurbulence::HillTurbulence() : Module (GetSourceModuleCount ()), mx_power (DEFAULT_HILLTURBULENCE_POWER), my_power (DEFAULT_HILLTURBULENCE_POWER), mz_power (DEFAULT_HILLTURBULENCE_POWER)
{
	xDistortModule.SetSeed(DEFAULT_HILLTURBULENCE_SEED);
	yDistortModule.SetSeed(DEFAULT_HILLTURBULENCE_SEED);
	zDistortModule.SetSeed(DEFAULT_HILLTURBULENCE_SEED);
	xDistortModule.SetFrequency(DEFAULT_HILLTURBULENCE_FREQUENCY);
	yDistortModule.SetFrequency(DEFAULT_HILLTURBULENCE_FREQUENCY);
	zDistortModule.SetFrequency(DEFAULT_HILLTURBULENCE_FREQUENCY);
	xDistortModule.SetOctaveCount(DEFAULT_HILLTURBULENCE_ROUGHNESS);
	yDistortModule.SetOctaveCount(DEFAULT_HILLTURBULENCE_ROUGHNESS);
	zDistortModule.SetOctaveCount(DEFAULT_HILLTURBULENCE_ROUGHNESS);

	ySPModule.SetSourceModule(0, yDistortModule);
	ySPModule.SetXScale(0.4);
	ySPModule.SetYScale(0);
	ySPModule.SetZScale(0.4);

	ySBModule.SetSourceModule(0, ySPModule);
}

HillTurbulence::~HillTurbulence()
{

}

double HillTurbulence::GetValue (double x, double y, double z) const
{
	assert(m_pSourceModule[0] != NULL);

	double x0, y0, z0;
	double x1, y1, z1;
	double x2, y2, z2;
	x0 = x + (12414.0 / 65536.0);
	y0 = y + (65124.0 / 65536.0);
	z0 = z + (31337.0 / 65536.0);
	x1 = x + (26519.0 / 65536.0);
	y1 = y + (18128.0 / 65536.0);
	z1 = z + (60493.0 / 65536.0);
	x2 = x + (53820.0 / 65536.0);
	y2 = y + (11213.0 / 65536.0);
	z2 = z + (44845.0 / 65536.0);
	double xDistort = x + (xDistortModule.GetValue (x0, y0, z0) * mx_power);
	double yDistort = y + (ySBModule.GetValue (x1, y1, z1) * my_power);
	double zDistort = z + (zDistortModule.GetValue (x2, y2, z2) * mz_power);

	return m_pSourceModule[0]->GetValue(xDistort, yDistort, zDistort);
}