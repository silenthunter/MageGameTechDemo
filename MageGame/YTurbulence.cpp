#include "YTurbulence.h"

using noise::module::YTurbulence;

YTurbulence::YTurbulence() : Module (GetSourceModuleCount ()), m_power (DEFAULT_YTURBULENCE_POWER)
{
	SetSeed(DEFAULT_YTURBULENCE_SEED);
	SetFrequency(DEFAULT_YTURBULENCE_FREQUENCY);
	SetRoughness(DEFAULT_YTURBULENCE_ROUGHNESS);
}

YTurbulence::~YTurbulence()
{

}

double YTurbulence::GetFrequency () const
{
	return yDistortModule.GetFrequency ();
}

int YTurbulence::GetSeed () const
{
	return yDistortModule.GetSeed ();
}

double YTurbulence::GetValue (double x, double y, double z) const
{
	assert (m_pSourceModule[0] != NULL);

	double x1, y1, z1;
	x1 = x + (26519.0 / 65536.0);
	y1 = y + (18128.0 / 65536.0);
	z1 = z + (60493.0 / 65536.0);
	double yDistort = y + (yDistortModule.GetValue(x1, y1, z1) * m_power);

	return m_pSourceModule[0]->GetValue (x, yDistort, z);
}

void YTurbulence::SetSeed (int seed)
{
	yDistortModule.SetSeed (seed);
}