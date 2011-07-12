#include "HillTurbulence.h"

using noise::module::HillTurbulence;

HillTurbulence::HillTurbulence() : Module(GetSourceModuleCount ()), m_power(DEFAULT_HILLTURBULENCE_POWER)
{
	SetSeed(DEFAULT_HILLTURBULENCE_SEED);
	SetFrequency(DEFAULT_HILLTURBULENCE_FREQUENCY);
	SetRoughness(DEFAULT_HILLTURBULENCE_ROUGHNESS);
	scaleBiasModule.SetSourceModule(0, yDistortModule);
}

HillTurbulence::~HillTurbulence()
{

}

double HillTurbulence::GetFrequency() const
{
	return yDistortModule.GetFrequency();
}

int HillTurbulence::GetSeed() const
{
	return yDistortModule.GetSeed();
}

double HillTurbulence::GetValue(double x, double y, double z) const
{
	assert(m_pSourceModule[0] != NULL);

	double x1, y1, z1;
	x1 = x + (26519.0 / 65536.0);
	y1 = y + (18128.0 / 65536.0);
	z1 = z + (60493.0 / 65536.0);
	double yDistort = y + (scaleBiasModule.GetValue(x1, y1, z1) * m_power);

	return m_pSourceModule[0]->GetValue(x, yDistort, z);
}

void HillTurbulence::SetSeed(int seed)
{
	yDistortModule.SetSeed(seed);
}