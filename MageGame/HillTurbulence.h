#include <noise\module\perlin.h>
#include <noise\module\scalepoint.h>
#include <noise\module\scalebias.h>

namespace noise
{
	namespace module
	{
		const double DEFAULT_HILLTURBULENCE_FREQUENCY = DEFAULT_PERLIN_FREQUENCY;
		const double DEFAULT_HILLTURBULENCE_POWER = 1.0;
		const int DEFAULT_HILLTURBULENCE_ROUGHNESS = 3;
		const int DEFAULT_HILLTURBULENCE_SEED = DEFAULT_PERLIN_SEED;

		class HillTurbulence : public Module
		{
		public:
			//Functions
			HillTurbulence();
			~HillTurbulence();

			double GetXPower () const
			{
				return mx_power;
			}

			double GetYPower () const
			{
				return my_power;
			}

			double GetZPower () const
			{
				return mz_power;
			}

			virtual int GetSourceModuleCount () const
			{
				return 1;
			}

			virtual double GetValue (double x, double y, double z) const;

			void SetXPower (double power)
			{
				mx_power = power;
			}

			void SetYPower (double power)
			{
				my_power = power;
			}

			void SetZPower (double power)
			{
				mz_power = power;
			}

			//Variables
			Perlin xDistortModule;
			Perlin yDistortModule;
			Perlin zDistortModule;
			ScalePoint ySPModule;
			ScaleBias ySBModule;

		protected:
			//Variables
			double mx_power;
			double my_power;
			double mz_power;
		};
	}
}