#pragma once

namespace pldn
{
    class Texture2D
    {
    public:
        Texture2D();
        ~Texture2D();

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;

        bool Load(const char* path);
        void Bind(unsigned int slot = 0) const;

    private:
        unsigned int m_textureHandle = 0;
        int m_width = -1;
        int m_height = -1;
    };
}