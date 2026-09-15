#pragma once

// Standard library includes
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

// Engine includes
#include "texture.hpp"

namespace pldn
{
    using TextureID = std::uint32_t;

    class TextureManager
    {
    public:
        TextureID Load(const std::string& path);
        const Texture& Get(TextureID id) const;

    private:
        std::unordered_map<std::string, TextureID> m_pathToID;
        std::vector<std::unique_ptr<Texture>> m_textures;
    };
}