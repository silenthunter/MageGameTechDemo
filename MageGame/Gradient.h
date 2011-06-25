#pragma once

#include <noise/noise.h>

namespace noise
{
	namespace module
	{
		class Gradient : public Module
		{
		public:
			//Functions
			Gradient();
			void SetGradient(double set_x1, double set_x2, double set_y1, double set_y2, double set_z1, double set_z2);
			virtual double GetValue (double x, double y, double z) const;

			virtual int GetSourceModuleCount () const
			{
				return 0;
			}

		private:
			//Functions

			//Variables
			double x1;
			double y1;
			double z1;
			double m_x;
			double m_y;
			double m_z;
			double m_vlen;
		};
	}
}