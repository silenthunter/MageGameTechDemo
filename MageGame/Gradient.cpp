#include "Gradient.h"

#define lerp(t, a, b) (a + t * (b - a))

using noise::module::Gradient;

Gradient::Gradient() : Module(GetSourceModuleCount())
{

}

void Gradient::SetGradient(double set_x1, double set_x2, double set_y1, double set_y2, double set_z1, double set_z2)
{
    x1 = set_x1;
	y1 = set_y1;
	z1 = set_z1;
 
    m_x = set_x2 - x1;
    m_y = set_y2 - y1;
    m_z = set_z2 - z1;
    m_vlen = (m_x * m_x + m_y * m_y + m_z * m_z);
}

double Gradient::GetValue(double x, double y, double z) const
{
	double dx = x - x1;
    double dy = y - y1;
    double dz = z - z1;
    double dp = dx * m_x + dy * m_y + dz * m_z;
	dp = dp / m_vlen;
	if(dp < 0) dp = 0;
	if(dp > 1) dp = 1;
    return lerp(dp, 1.0, -1.0);
}