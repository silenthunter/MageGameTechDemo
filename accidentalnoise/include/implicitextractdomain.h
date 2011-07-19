#ifndef IMPLICIT_EXTRACTDOMAIN_H
#define IMPLICIT_EXTRACTDOMAIN_H
#include "implicitmodulebase.h"

namespace anl
{
    class CImplicitExtractDomain : public CImplicitModuleBase
    {
        public:
        CImplicitExtractDomain();
        ~CImplicitExtractDomain();

        void setCoefficientX(CImplicitModuleBase *m);
        void setCoefficientY(CImplicitModuleBase *m);
        void setCoefficientZ(CImplicitModuleBase *m);
        void setCoefficientW(CImplicitModuleBase *m);
        void setCoefficientU(CImplicitModuleBase *m);
        void setCoefficientV(CImplicitModuleBase *m);

        void setCoefficientX(double c);
        void setCoefficientY(double c);
        void setCoefficientZ(double c);
        void setCoefficientW(double c);
        void setCoefficientU(double c);
        void setCoefficientV(double c);

        double get(double x, double y);
        double get(double x, double y, double z);
        double get(double x, double y, double z, double w);
        double get(double x, double y, double z, double w, double u, double v);

        protected:
        CScalarParameter m_cx, m_cy, m_cz, m_cw, m_cu, m_cv;  // Coefficients
    };
};

#endif
