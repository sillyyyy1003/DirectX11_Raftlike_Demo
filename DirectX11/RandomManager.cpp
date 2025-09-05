#include "RandomManager.h"

RandomGenerator::RandomGenerator(unsigned int seed)
{
	m_engine.seed(seed);
}
