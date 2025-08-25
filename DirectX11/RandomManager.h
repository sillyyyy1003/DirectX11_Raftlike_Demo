#pragma once
#include <string>
#include <unordered_map>
#include <random>
#include <chrono>

class RandomGenerator
{
public:
    RandomGenerator() = default;
    RandomGenerator(unsigned int seed);

    int GetRandomInt(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(m_engine);
    }

    float GetRandomFloat(float min = 0.0f, float max = 1.0f)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(m_engine);
    }

    bool Chance(float probability) // 0.0 ~ 1.0
    {
        return GetRandomFloat() < probability;
    }

private:
    std::mt19937 m_engine;
};


class RandomManager
{
public:
    static RandomManager& Instance()
    {
        static RandomManager instance;
        return instance;
	}
    RandomManager(const RandomManager&) = delete;
    RandomManager& operator=(const RandomManager&) = delete;

    RandomGenerator& GetGenerator(const std::string& systemName)
    {
        if (m_generators.find(systemName) == m_generators.end())
        {
            // 見つからない場合、新しいジェネレーターを作成
            unsigned int subSeed = (unsigned int)(m_masterSeed ^ std::hash<std::string>{}(systemName));
            m_generators.emplace(systemName, RandomGenerator(subSeed));
        }

        return m_generators.at(systemName);
    }

    unsigned int GetMasterSeed() const { return m_masterSeed; }
private:
    RandomManager()
    {
        m_masterSeed = static_cast<unsigned int>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
            );
    }
    ~RandomManager() = default;

	unsigned int m_masterSeed;
	std::unordered_map<std::string, RandomGenerator> m_generators;
};

