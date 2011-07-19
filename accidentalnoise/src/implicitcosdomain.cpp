#include "implicitcosdomain.h"
#include <math.h>

namespace anl
{
    CImplicitCosDomain::CImplicitCosDomain() : m_source(0),m_x(false), m_y(false), m_z(false), m_w(false), m_u(false), m_v(false){}
    CImplicitCosDomain::~CImplicitCosDomain(){}

    void CImplicitCosDomain::setAffectX(bool v){m_x=v;}
    void CImplicitCosDomain::setAffectY(bool v){m_y=v;}
    void CImplicitCosDomain::setAffectZ(bool v){m_z=v;}
    void CImplicitCosDomain::setAffectW(bool v){m_w=v;}
    void CImplicitCosDomain::setAffectU(bool v){m_u=v;}
    void CImplicitCosDomain::setAffectV(bool v){m_v=v;}
    void CImplicitCosDomain::setSource(double v){m_source.set(v);}
    void CImplicitCosDomain::setSource(CImplicitModuleBase *m){m_source.set(m);}

    double CImplicitCosDomain::get(double x, double y)
    {
        double nx=x, ny=y;
        if(m_x) nx=cos(nx);
        if(m_y) ny=cos(ny);

        return m_source.get(nx,ny);
    }

    double CImplicitCosDomain::get(double x, double y, double z)
    {
        double nx=x, ny=y, nz=z;
        if(m_x) nx=cos(nx);
        if(m_y) ny=cos(ny);
        if(m_z) nz=cos(nz);

        return m_source.get(nx,ny,nz);
    }
    double CImplicitCosDomain::get(double x, double y, double z, double w)
    {
        double nx=x, ny=y, nz=z, nw=w;
        if(m_x) nx=cos(nx);
        if(m_y) ny=cos(ny);
        if(m_z) nz=cos(nz);
        if(m_w) nw=cos(nw);

        return m_source.get(nx,ny,nz,nw);
    }
    double CImplicitCosDomain::get(double x, double y, double z, double w, double u, double v)
    {
        double nx=x, ny=y, nz=z, nw=w, nu=u, nv=v;
        if(m_x) nx=cos(nx);
        if(m_y) ny=cos(ny);
        if(m_z) nz=cos(nz);
        if(m_w) nw=cos(nw);
        if(m_u) nu=cos(nu);
        if(m_v) nv=cos(nv);

        return m_source.get(nx,ny,nz,nw,nu,nv);
    }
};
