#pragma once

#include <OpenGL/OpenGlInclude.h>

namespace EisEngine {
    class ResourceManager;
    /// \n A 2-dimensional texture. Is attached to a Renderer component to apply.
    class Texture2D {
        friend ResourceManager;
    public:
        /// \n Binds the texture as the current active GL_TEXTURE_2D.
        void Bind() const;

        /// \n Texture width in pixels.
        unsigned int Width;
        /// \n Texture height in pixels.
        unsigned int Height;
    private:
        /// \n Creates a new texture object.
        Texture2D();

        /// \n Generates a texture from the given image data.
        /// @param width - unsigned int: the width of the texture in pixels.
        /// @param height - unsigned int: the height of the texture in pixels
        /// @param data - unsigned char*: a pointer to the image data.
        void Generate(unsigned int width, unsigned int height, unsigned char* data);

        /// \n the unique ID in the resource management system.
        unsigned int textureID;

        /// \n texture object format.
        unsigned int internalFormat;
        /// \n source image format.
        unsigned int imageFormat;

        /// \n Wrapping mode on s-Axis.
        unsigned int wrapS;
        /// \n Wrapping mode on t-Axis.
        unsigned int wrapT;
        /// \n Filtering mode if texture can be fully displayed on screen.
        unsigned int minFilterMode;
        /// \n Filtering mode if texture cannot be fully displayed on screen (n(texture.pixels) > n(screen.pixels)
        unsigned int maxFilterMode;
    };
}
