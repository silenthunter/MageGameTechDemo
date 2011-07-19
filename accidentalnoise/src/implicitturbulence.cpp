#include "implicitturbulence.h"
#include "utility.h"

namespace anl
{
    CImplicitTurbulence::CImplicitTurbulence() : CImplicitModuleBase(),m_source(0), m_px(1), m_py(1), m_pz(1), m_pw(1), m_pu(1), m_pv(1)
	{
		clearMaskSource();
		for(int c=0; c<6; ++c) clearAxisSource(c);
	}
	CImplicitTurbulence::~CImplicitTurbulence(){}

	void CImplicitTurbulence::setSource(CImplicitModuleBase *m){m_source=m;}

	void CImplicitTurbulence::setAxisSource(int which, CImplicitModuleBase *m)
	{
		if(which<0 || which>=6) return;
		m_axissource[which]=m;
	}
	void CImplicitTurbulence::clearAxisSource(int which)
	{
		if(which<0 || which>=6) return;
		m_axissource[which]=0;
	}
	void CImplicitTurbulence::setMaskSource(CImplicitModuleBase *b)
	{
		m_masksource=b;
	}
	void CImplicitTurbulence::clearMaskSource()
	{
		m_masksource=0;
	}

	void CImplicitTurbulence::setPowers(double px, double py, double pz, double pw, double pu, double pv)
	{
		m_px.set(px);
		m_py.set(py);
		m_pz.set(pz);
		m_pw.set(pw);
		m_pu.set(pu);
		m_pv.set(pv);
	}

	void CImplicitTurbulence::setXPower(double d)
	{
	    m_px.set(d);
	}
	void CImplicitTurbulence::setYPower(double d)
	{
	    m_py.set(d);
	}
	void CImplicitTurbulence::setZPower(double d)
	{
	    m_pz.set(d);
	}
	void CImplicitTurbulence::setWPower(double d)
	{
	    m_pw.set(d);
	}
	void CImplicitTurbulence::setUPower(double d)
	{
	    m_pu.set(d);
	}
	void CImplicitTurbulence::setVPower(double d)
	{
	    m_pv.set(d);
	}

	void CImplicitTurbulence::setXPower(CImplicitModuleBase *d)
	{
	    m_px.set(d);
	}
	void CImplicitTurbulence::setYPower(CImplicitModuleBase *d)
	{
	    m_py.set(d);
	}
	void CImplicitTurbulence::setZPower(CImplicitModuleBase *d)
	{
	    m_pz.set(d);
	}
	void CImplicitTurbulence::setWPower(CImplicitModuleBase *d)
	{
	    m_pw.set(d);
	}
	void CImplicitTurbulence::setUPower(CImplicitModuleBase *d)
	{
	    m_pu.set(d);
	}
	void CImplicitTurbulence::setVPower(CImplicitModuleBase *d)
	{
	    m_pv.set(d);
	}

	double CImplicitTurbulence::get(double x, double y)
	{
		if(!m_source) return 0.0;
		double scale;
		if(m_masksource)
		{
			scale=m_masksource->get(x,y);
			scale = (scale+1.0) * 0.5;
            scale=clamp(scale, 0.0, 1.0);
		}
		else scale=1.0;

		double nx=x, ny=y;
		if(m_axissource[0]) nx=nx+m_axissource[0]->get(x,y) * m_px.get(x,y) * scale;
		if(m_axissource[1]) ny=ny+m_axissource[1]->get(x,y) * m_py.get(x,y) * scale;

		return m_source->get(nx,ny);
	}

	double CImplicitTurbulence::get(double x, double y, double z)
	{
	    if(!m_source) return 0.0;
		double scale;
		if(m_masksource)
		{
			scale=m_masksource->get(x,y,z);
			scale = (scale+1.0) * 0.5;
            scale=clamp(scale, 0.0, 1.0);
		}
		else scale=1.0;

		double nx=x, ny=y, nz=z;
		if(m_axissource[0]) nx=nx+m_axissource[0]->get(x,y,z) * m_px.get(x,y,z) * scale;
		if(m_axissource[1]) ny=ny+m_axissource[1]->get(x,y,z) * m_py.get(x,y,z) * scale;
		if(m_axissource[2]) nz=nz+m_axissource[2]->get(x,y,z) * m_pz.get(x,y,z) * scale;


		return m_source->get(nx,ny,nz);
	}

	double CImplicitTurbulence::get(double x, double y, double z, double w)
	{
		if(!m_source) return 0.0;
		double scale;
		if(m_masksource)
		{
			scale=m_masksource->get(x,y,z,w);
			scale = (scale+1.0) * 0.5;
            scale=clamp(scale, 0.0, 1.0);
		}
		else scale=1.0;

		double nx=x, ny=y, nz=z, nw=w;
		if(m_axissource[0]) nx=nx+m_axissource[0]->get(x,y,z,w) * m_px.get(x,y,z,w) * scale;
		if(m_axissource[1]) ny=ny+m_axissource[1]->get(x,y,z,w) * m_py.get(x,y,z,w) * scale;
		if(m_axissource[2]) nz=nz+m_axissource[2]->get(x,y,z,w) * m_pz.get(x,y,z,w) * scale;
		if(m_axissource[3]) nw=nw+m_axissource[3]->get(x,y,z,w) * m_pw.get(x,y,z,w) * scale;

		return m_source->get(nx,ny,nz,nw);
	}

	double CImplicitTurbulence::get(double x, double y, double z, double w, double u, double v)
	{
		if(!m_source) return 0.0;
		double scale;
		if(m_masksource)
		{
			scale=m_masksource->get(x,y,z,w,u,v);
			scale = (scale+1.0) * 0.5;
            scale=clamp(scale, 0.0, 1.0);
		}
		else scale=1.0;

		double nx=x, ny=y, nz=z, nw=w, nu=u, nv=v;
		if(m_axissource[0]) nx=nx+m_axissource[0]->get(x,y,z,w,u,v) * m_px.get(x,y,z,w,u,v) * scale;
		if(m_axissource[1]) ny=ny+m_axissource[1]->get(x,y,z,w,u,v) * m_py.get(x,y,z,w,u,v) * scale;
		if(m_axissource[2]) nz=nz+m_axissource[2]->get(x,y,z,w,u,v) * m_pz.get(x,y,z,w,u,v) * scale;
		if(m_axissource[3]) nw=nw+m_axissource[3]->get(x,y,z,w,u,v) * m_pw.get(x,y,z,w,u,v) * scale;
		if(m_axissource[4]) nu=nu+m_axissource[4]->get(x,y,z,w,u,v) * m_pu.get(x,y,z,w,u,v) * scale;
		if(m_axissource[5]) nv=nv+m_axissource[5]->get(x,y,z,w,u,v) * m_pv.get(x,y,z,w,u,v) * scale;

		return m_source->get(nx,ny,nz,nw,nu,nv);
	}
};
