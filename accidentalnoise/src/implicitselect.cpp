#include "implicitselect.h"
#include "utility.h"

namespace anl
{
    CImplicitSelect::CImplicitSelect() : CImplicitModuleBase(), m_low(0), m_high(0), m_control(0),
        m_threshold(0.0), m_falloff(0.0)
	{
	}
	CImplicitSelect::~CImplicitSelect(){}

	void CImplicitSelect::setLowSource(CImplicitModuleBase *b)
	{
		m_low.set(b);
	}
	void CImplicitSelect::setHighSource(CImplicitModuleBase *b)
	{
		m_high.set(b);
	}
	void CImplicitSelect::setControlSource(CImplicitModuleBase *b)
	{
		m_control.set(b);
	}

	void CImplicitSelect::setLowSource(double b)
	{
		m_low.set(b);
	}
	void CImplicitSelect::setHighSource(double b)
	{
		m_high.set(b);
	}
	void CImplicitSelect::setControlSource(double b)
	{
		m_control.set(b);
	}

	void CImplicitSelect::setThreshold(double t)
	{
		//m_threshold=t;
		m_threshold.set(t);
	}
	void CImplicitSelect::setFalloff(double f)
	{
		//m_falloff=f;
		m_falloff.set(f);
	}
	void CImplicitSelect::setThreshold(CImplicitModuleBase *m)
	{
	    m_threshold.set(m);
	}
	void CImplicitSelect::setFalloff(CImplicitModuleBase *m)
	{
	    m_falloff.set(m);
	}

	double CImplicitSelect::get(double x, double y)
	{
		double control=m_control.get(x,y);
		double falloff=m_falloff.get(x,y);
		double threshold=m_threshold.get(x,y);
		double low=m_low.get(x,y);
		double high=m_high.get(x,y);

        if(falloff>0.0)
        {
            if(control < (threshold-falloff))
            {
                // Lies outside of falloff area below threshold, return first source
                return low;
            }
            else if(control > (threshold+falloff))
            {
                // Lise outside of falloff area above threshold, return second source
                return high;
            }
            else
            {
                // Lies within falloff area.
                double lower=threshold-falloff;
                double upper=threshold+falloff;
                double blend=quintic_blend((control-lower)/(upper-lower));
                return lerp(blend,low,high);
            }
        }
        else
        {
            if(control<threshold) return low;
            else return high;
        }
    }

    double CImplicitSelect::get(double x, double y, double z)
    {
		double control=m_control.get(x,y,z);
		double falloff=m_falloff.get(x,y,z);
		double threshold=m_threshold.get(x,y,z);
		double low=m_low.get(x,y,z);
		double high=m_high.get(x,y,z);

        if(falloff>0.0)
        {
            if(control < (threshold-falloff))
            {
                // Lies outside of falloff area below threshold, return first source
                return low;
            }
            else if(control > (threshold+falloff))
            {
                // Lise outside of falloff area above threshold, return second source
                return high;
            }
            else
            {
                // Lies within falloff area.
                double lower=threshold-falloff;
                double upper=threshold+falloff;
                double blend=quintic_blend((control-lower)/(upper-lower));
                return lerp(blend,low,high);
            }
        }
        else
        {
            if(control<threshold) return low;
            else return high;
        }
    }

	double CImplicitSelect::get(double x, double y, double z, double w)
	{
		double control=m_control.get(x,y,z,w);
		double falloff=m_falloff.get(x,y,z,w);
		double threshold=m_threshold.get(x,y,z,w);
		double low=m_low.get(x,y,z,w);
		double high=m_high.get(x,y,z,w);

        if(falloff>0.0)
        {
            if(control < (threshold-falloff))
            {
                // Lies outside of falloff area below threshold, return first source
                return low;
            }
            else if(control > (threshold+falloff))
            {
                // Lise outside of falloff area above threshold, return second source
                return high;
            }
            else
            {
                // Lies within falloff area.
                double lower=threshold-falloff;
                double upper=threshold+falloff;
                double blend=quintic_blend((control-lower)/(upper-lower));
                return lerp(blend,low,high);
            }
        }
        else
        {
            if(control<threshold) return low;
            else return high;
        }
    }

	double CImplicitSelect::get(double x, double y, double z, double w, double u, double v)
	{
		double control=m_control.get(x,y,z,w,u,v);
		double falloff=m_falloff.get(x,y,z,w,u,v);
		double threshold=m_threshold.get(x,y,z,w,u,v);
		double low=m_low.get(x,y,z,w,u,v);
		double high=m_high.get(x,y,z,w,u,v);

        if(falloff>0.0)
        {
            if(control < (threshold-falloff))
            {
                // Lies outside of falloff area below threshold, return first source
                return low;
            }
            else if(control > (threshold+falloff))
            {
                // Lise outside of falloff area above threshold, return second source
                return high;
            }
            else
            {
                // Lies within falloff area.
                double lower=threshold-falloff;
                double upper=threshold+falloff;
                double blend=quintic_blend((control-lower)/(upper-lower));
                return lerp(blend,low,high);
            }
        }
        else
        {
            if(control<threshold) return low;
            else return high;
        }
    }
};
