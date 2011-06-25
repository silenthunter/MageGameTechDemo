#include <noise\module\perlin.h>

namespace noise
{
	namespace module
	{
		const double DEFAULT_ENHANCEDTURBULENCE_FREQUENCY = DEFAULT_PERLIN_FREQUENCY;
		const double DEFAULT_ENHANCEDTURBULENCE_POWER = 1.0;
		const int DEFAULT_ENHANCEDTURBULENCE_ROUGHNESS = 3;
		const int DEFAULT_ENHANCEDTURBULENCE_SEED = DEFAULT_PERLIN_SEED;

		class EnhancedTurbulence : public Module
		{
		public:
			//Functions
			EnhancedTurbulence();
			~EnhancedTurbulence();

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

		protected:
			//Variables
			double mx_power;
			double my_power;
			double mz_power;
		};
	}
}