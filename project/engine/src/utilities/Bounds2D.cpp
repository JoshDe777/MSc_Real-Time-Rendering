#include "engine/utilities/Bounds2D.h"

namespace EisEngine{
    const Bounds2D Bounds2D::Square1x1 = Bounds2D(0.5f,0.5f,-0.5f,-0.5f);

    Bounds2D &Bounds2D::operator*=(const EisEngine::Vector3 &v) {
        this->top *= v.y;
        this->right *= v.x;
        this->bottom *= v.y;
        this->left *= v.x;
        return *this;
    }

    Bounds2D &Bounds2D::normalize(const Vector3 &scale) {
        this->left *= 1.0f / scale.x;
        this->right *= 1.0f / scale.x;
        this->top *= 1.0f / scale.y;;
        this->bottom *= 1.0f / scale.y;;
        return *this;
    }
}
