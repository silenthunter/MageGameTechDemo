#include "implicitsindomain.h"
#include <math.h>

namespace anl
{
    CImplicitSinDomain::CImplicitSinDomain() : m_source(0),m_x(false), m_y(false), m_z(false), m_w(false), m_u(false), m_v(false){}
    CImplicitSinDomain::~CImplicitSinDomain(){}

    void CImplicitSinDomain::setAffectX(bool v){m_x=v;}
    void CImplicitSinDomain::setAffectY(bool v){m_y=v;}
    void CImplicitSinDomain::setAffectZ(bool v){m_z=v;}
    void CImplicitSinDomain::setAffectW(bool v){m_w=v;}
    void CImplicitSinDomain::setAffectU(bool v){m_u=v;}
    void CImplicitSinDomain::setAffectV(bool v){m_v=v;}
    void CImplicitSinDomain::setSource(double v){m_source.set(v);}
    void CImplicitSinDomain::setSource(CImplicitModuleBase *m){m_source.set(m);}

    double CImplicitSinDomain::get(double x, double y)
    {
        double nx=x, ny=y;
        if(m_x) nx=sin(nx);
        if(m_y) ny=sin(ny);

        return m_source.get(nx,ny);
    }

    double CImplicitSinDomain::get(double x, double y, double z)
    {
        double nx=x, ny=y, nz=z;
        if(m_x) nx=sin(nx);
        if(m_y) ny=sin(ny);
        if(m_z) nz=sin(nz);

        return m_source.get(nx,ny,nz);
    }
    double CImplicitSinDomain::get(double x, double y, double z, double w)
    {
        double nx=x, ny=y, nz=z, nw=w;
        if(m_x) nx=sin(nx);
        if(m_y) ny=sin(ny);
        if(m_z) nz=sin(nz);
        if(m_w) nw=sin(nw);

        return m_source.get(nx,ny,nz,nw);
    }
    double CImplicitSinDomain::get(double x, double y, double z, double w, double u, double v)
    {
        double nx=x, ny=y, nz=z, nw=w, nu=u, nv=v;
        if(m_x) nx=sin(nx);
        if(m_y) ny=sin(ny);
        if(m_z) nz=sin(nz);
        if(m_w) nw=sin(nw);
        if(m_u) nu=sin(nu);
        if(m_v) nv=sin(nv);

        return m_source.get(nx,ny,nz,nw,nu,nv);
    }
};

