#pragma once

#include "Vector3.h"

#include <algorithm>
#include <glm/glm.hpp>

namespace EisEngine {
    /// \n A 4-dimensional vector representing the RGBA% (each value representing a % of 255) values of a colour.
    class Color {
    public:
        /// \n Defines a color for use in EisEngine.
        /// \n All values are to be within the range [0, 1], representing percentage of 255.
        /// Anything outside of the range will be clamped to this range.
        explicit Color(float _r = 1, float _g = 1, float _b = 1, float _a = 1) :
        r(std::clamp(_r, 0.0f, 1.0f)),
        g(std::clamp(_g, 0.0f, 1.0f)),
        b(std::clamp(_b, 0.0f, 1.0f)),
        a(std::clamp(_a, 0.0f, 1.0f))
        { }
        /// \n Creates a color form a vector3 + transparency value.
        explicit Color(const Vector3& v, const float& a) :
        r(v.x),
        g(v.y),
        b(v.z),
        a(a) { }

        /// \n A float representing the colour's red values in % of 255.
        float r;
        /// \n A float representing the colour's green values in % of 255.
        float g;
        /// \n A float representing the colour's blue values in % of 255.
        float b;
        /// \n A float representing the colour's transparency value.
        /// \n a = 0 being fully transparent, a = 1 being fully opaque.
        float a;

        /// \n The color white.
        /// \n @b r = 1; @b g = 1, @b b = 1, @b a = 1.
        static const Color white;
        /// \n The color blue.
        /// \n @b r = 0; @b g = 0, @b b = 1, @b a = 1.
        static const Color blue;
        /// \n The color red.
        /// \n @b r = 1; @b g = 0, @b b = 0, @b a = 1.
        static const Color red;
        /// \n The color green.
        /// \n @b r = 0; @b g = 1, @b b = 0, @b a = 1.
        static const Color green;
        /// \n The color yellow.
        /// \n @b r = 1; @b g = 1, @b b = 0, @b a = 1.
        static const Color yellow;
        /// \n The color black.
        /// \n @b r = 0; @b g = 0, @b b = 0, @b a = 1.
        static const Color black;
        /// \n The color orange.
        /// \n @b r = 1; @b g ~ 0.647, @b b = 0, @b a = 1.
        static const Color orange;
        /// \n Full Transparency.
        /// \n @b r = 1, @b g = 1, @b b = 1, @b a = 0.
        static const Color transparent;

        operator glm::vec4() const{ return {r, g, b, a};}
    };
}
