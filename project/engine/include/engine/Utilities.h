#pragma once

#include <random>

#include "engine/utilities/Math.h"
#include "engine/utilities/Debug.h"
#include "engine/utilities/Color.h"
#include "engine/Events.h"

#include "engine/utilities/Vector2.h"
#include "engine/utilities/Vector3.h"

#include "engine/utilities/DebugLine.h"
#include "engine/utilities/Bounds2D.h"

#include "engine/utilities/rendering/PrimitiveMesh2D.h"
#include "engine/utilities/rendering/PrimitiveMesh3D.h"
#include "engine/utilities/rendering/PrimitiveSpriteMesh.h"
#include "engine/utilities/rendering/Texture2D.h"

using Vector2 = EisEngine::Vector2;
using Vector3 = EisEngine::Vector3;
using Color = EisEngine::Color;
using DebugLine = EisEngine::DebugLine;
using Bounds2D = EisEngine::Bounds2D;
using PrimitiveMesh2D = EisEngine::rendering::PrimitiveMesh2D;
using PrimitiveMesh3D = EisEngine::rendering::PrimitiveMesh3D;
using PrimitiveSpriteMesh = EisEngine::rendering::PrimitiveSpriteMesh;
using Texture2D = EisEngine::Texture2D;

namespace EisEngine{
    /// \n Produces a string of the given vector of floats in the format\n
    /// &lt;a, b, ..., n>.
    inline std::string to_string(const std::vector<float> &v){
        std::ostringstream oss;
        oss.precision(6);

        oss << "<";

        for (size_t i = 0; i < v.size(); ++i) {
            oss << v[i];
            if (i != v.size() - 1)
                oss << ", ";
        }

        oss << ">";

        return (std::string) oss.str();
    }

    /// \n Produces a string of the given vector of unsigned ints in the format\n
    /// &lt;a, b, ..., n>.
    inline std::string to_string(const std::vector<unsigned int> &v){
        std::ostringstream oss;
        oss.precision(6);

        oss << "<";

        for (size_t i = 0; i < v.size(); ++i) {
            oss << v[i];
            if (i != v.size() - 1)
                oss << ", ";
        }

        oss << ">";

        return (std::string) oss.str();
    }

    /// \n Produces a string of the given b2Vec2, in the format (x, y)
    inline std::string to_string(const b2Vec2 &v){
        std::ostringstream oss;
        oss.precision(6);
        oss << "(" << v.x << ", " << v.y << ")";
        return (std::string) oss.str();
    }

    /// \n Produces a string of the provided vector of b2Vec2's, in the format
    /// &lt(x1, y1), (x2, y2), ..., (xn, yn)>.
    inline std::string to_string(const std::vector<b2Vec2> &v){
        std::ostringstream oss;
        oss.precision(6);

        oss << "<";

        for (size_t i = 0; i < v.size(); ++i) {
            oss << to_string(v[i]);
            if (i != v.size() - 1)
                oss << ", ";
        }

        oss << ">";

        return (std::string) oss.str();
    }

    /// \n Provides a random float value x with\n min (incl.) &le x &lt max (excl.)
    inline float RandomFloat(const float& min, const float& max){
        std::random_device rnd;
        std::mt19937 gen(rnd());
        std::uniform_real_distribution<> dis(min, max);
        return static_cast<float>(dis(gen));
    }

    /// \n Provides a random integer value x within the range [min, max] (both inclusive).
    inline int RandomInt(const int& min, const int& max){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(min,max);
        return distr(gen);
    }

    /// \n Determines whether a vector contains a specific item.
    template<typename T>
    bool ListContains(const std::vector<T>& list, const T& item){
        return std::find(list.begin(), list.end(), item) != list.end();
    }

    inline std::vector<Vector3> AiVector3DToVector3(std::vector<aiVector3D>& v){
        std::vector<Vector3> result = {};

        for(auto i : v)
            result.emplace_back(i);

        return result;
    }

    inline std::vector<glm::vec3> Vec3VectorToGlm(const std::vector<Vector3>& v){
        std::vector<glm::vec3> out = {};
        for(auto i : v)
            out.emplace_back(i.x, i.y, i.z);
        return out;
    }

    inline std::vector<glm::vec2> Vec2VectorToGlm(const std::vector<Vector2>& v){
        std::vector<glm::vec2> out = {};
        for(auto i : v)
            out.emplace_back(i.x, i.y);
        return out;
    }
}
