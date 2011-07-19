#ifndef IMPLICIT_TURBULENCE_H
#define IMPLICIT_TURBULENCE_H
#include "implicitmodulebase.h"

namespace anl
{
class CImplicitTurbulence : public CImplicitModuleBase
{
public:
	CImplicitTurbulence();
	~CImplicitTurbulence();

	void setSource(CImplicitModuleBase *m);
	void setAxisSource(int which, CImplicitModuleBase *m);
	void clearAxisSource(int which);
	void setMaskSource(CImplicitModuleBase *b);
	void clearMaskSource();
	void setPowers(double px, double py, double pz, double pw, double pu, double pv);
	void setXPower(double d);
	void setYPower(double d);
	void setZPower(double d);
	void setWPower(double d);
	void setUPower(double d);
	void setVPower(double d);
	void setXPower(CImplicitModuleBase *d);
	void setYPower(CImplicitModuleBase *d);
	void setZPower(CImplicitModuleBase *d);
	void setWPower(CImplicitModuleBase *d);
	void setUPower(CImplicitModuleBase *d);
	void setVPower(CImplicitModuleBase *d);
	double get(double x, double y);
	double get(double x, double y, double z);
	double get(double x, double y, double z, double w);
	double get(double x, double y, double z, double w, double u, double v);

    protected:
    CImplicitModuleBase *m_source, *m_axissource[6], *m_masksource;
	CScalarParameter m_px, m_py, m_pz, m_pw, m_pu, m_pv;


};
};

#endif
