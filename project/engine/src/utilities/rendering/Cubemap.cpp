#include "engine/utilities/rendering/Cubemap.h"
#include "engine/utilities/rendering/Shader.h"
#include "engine/utilities/Debug.h"

namespace EisEngine {
    Cubemap::Cubemap() :
        internalFormat(GL_RGB), imageFormat(GL_RGB),
        wrapS(GL_CLAMP_TO_EDGE), wrapT(GL_CLAMP_TO_EDGE),
        wrapR(GL_CLAMP_TO_EDGE), minFilterMode(GL_LINEAR),
        maxFilterMode(GL_LINEAR)
    {
        glGenTextures(1, &textureID);
    }

    void Cubemap::Generate(unsigned int& index, unsigned int w, unsigned int h, unsigned char* data) {
        if(this->width <= 0)
            this->width = (int) w;
        else if (this->width != w)
            DEBUG_RUNTIME_ERROR("Attempting to create a Cubemap with textures of different dimensions!")
        if(this->height <= 0)
            this->height = (int) h;
        else if (this->height != h)
            DEBUG_RUNTIME_ERROR("Attempting to create a Cubemap with textures of different dimensions!")

        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + index,
                0,
                (GLint) internalFormat,
                (GLint) this->width,
                (GLint) this->height,
                0,
                imageFormat,
                GL_UNSIGNED_BYTE,
                data
                );

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void Cubemap::SetParams() {
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, (GLint) maxFilterMode);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (GLint) minFilterMode);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, (GLint) wrapS);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, (GLint) wrapT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, (GLint) wrapR);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void Cubemap::Bind() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    }
}