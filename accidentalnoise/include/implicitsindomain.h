#ifndef IMPLICIT_SINDOMAIN_H
#define IMPLICIT_SINDOMAIN_H
#include "implicitmodulebase.h"

namespace anl
{
    class CImplicitSinDomain : public CImplicitModuleBase
    {
        public:
        CImplicitSinDomain();
        ~CImplicitSinDomain();

        void setAffectX(bool v);
        void setAffectY(bool v);
        void setAffectZ(bool v);
        void setAffectW(bool v);
        void setAffectU(bool v);
        void setAffectV(bool v);

        void setSource(double v);
        void setSource(CImplicitModuleBase *m);

        double get(double x, double y);
        double get(double x, double y, double z);
        double get(double x, double y, double z, double w);
        double get(double x, double y, double z, double w, double u, double v);

        protected:
        CScalarParameter m_source;
        bool m_x, m_y, m_z, m_w, m_u, m_v;
    };
};

#endif

