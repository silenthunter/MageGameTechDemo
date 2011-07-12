#include <noise\module\perlin.h>
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

			double GetFrequency () const;

			double GetPower () const
			{
				return m_power;
			}

			int GetRoughnessCount () const
			{
				return yDistortModule.GetOctaveCount ();
			}

			int GetSeed () const;

			virtual int GetSourceModuleCount () const
			{
				return 1;
			}

			virtual double GetValue (double x, double y, double z) const;

			void SetFrequency (double frequency)
			{
				yDistortModule.SetFrequency (frequency);
			}

			void SetPower (double power)
			{
				m_power = power;
			}

			void SetRoughness (int roughness)
			{
				yDistortModule.SetOctaveCount (roughness);
			}

			void SetSeed (int seed);

			//Variables
			Perlin yDistortModule;
			ScaleBias scaleBiasModule;

		protected:
			//Variables
			double m_power;
		};
	}
}