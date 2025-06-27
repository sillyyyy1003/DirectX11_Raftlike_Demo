#pragma once
#include <unordered_map>
#include <Jolt.h>
#include <Core/Reference.h>
#include <Physics/Collision/Shape/Shape.h>
#include <Physics/Collision/Shape/BoxShape.h>
#include <Physics/Collision/Shape/SphereShape.h>
#include "DebugLog.h"
using namespace std;
using namespace JPH;
class ShapeFactory
{
private:
	ShapeFactory() = default;
	~ShapeFactory() = default;

	typedef std::unordered_map<string, RefConst<Shape>> ShapeList;
	ShapeList m_shapeCache;

public:
	static ShapeFactory& Instance();

    RefConst<Shape> GetOrCreateBox(const RVec3& halfExtent)
    {
        std::string key = "Box_" + Vec3ToStr(halfExtent);
        return GetOrCreateShape(key, [&]()
            {
                BoxShapeSettings settings(halfExtent);
                return settings.Create();
            });
    }

    RefConst<Shape> GetOrCreateSphere(float radius)
    {
        std::string key = "Sphere_" + std::to_string(radius);
        return GetOrCreateShape(key, [&]()
            {
                SphereShapeSettings settings(radius);
                return settings.Create();
            });
    }

private:

    template <typename CreateFunc>
    RefConst<Shape> GetOrCreateShape(const std::string& key, CreateFunc&& createFunc)
    {
        auto it = m_shapeCache.find(key);
        if (it != m_shapeCache.end())
            return it->second;

       ShapeSettings::ShapeResult result = createFunc();
        if (result.HasError())
        {
            DebugLog::LogError("[ShapeFactory]:Failed to create shape: {}", key);
        	return nullptr;
        }

        RefConst<Shape> shape = result.Get();
        m_shapeCache[key] = shape;
        return shape;
    }

    static std::string Vec3ToStr(const RVec3& v)
    {
        return std::to_string(v.GetX()) + "_" + std::to_string(v.GetY()) + "_" + std::to_string(v.GetZ());
    }
};

