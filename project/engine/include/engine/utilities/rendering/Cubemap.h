#pragma once

#include <OpenGL/OpenGlInclude.h>

namespace EisEngine {
    class ResourceManager;
    class Cubemap {
        friend ResourceManager;
    public:
        void Bind() const;
    private:
        unsigned int id;

        /// \n texture object format.
        unsigned int internalFormat;
        /// \n source image format.
        unsigned int imageFormat;

        /// \n Wrapping mode on s-Axis.
        unsigned int wrapS;
        /// \n Wrapping mode on t-Axis.
        unsigned int wrapT;
        /// \n Wrapping mode on r-Axis.
        unsigned int wrapR;
        /// \n Filtering mode if texture can be fully displayed on screen.
        unsigned int minFilterMode;
        /// \n Filtering mode if texture cannot be fully displayed on screen (n(texture.pixels) > n(screen.pixels)
        unsigned int maxFilterMode;
    };
}
