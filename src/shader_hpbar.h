#pragma once
#include <xx_shader.h>

namespace xx {

    struct Shader_HPBarData {
        XY pos{}, scale{ 1, 1 };                // float * 4
        xx::RGBA8 color{ 255, 255, 255, 255 };  // u8 * 4
        uint32_t numbers[4];                    // u32 * 4 / u8 * 16   // last one is len
        float value{};                          // float * 1
    };

    struct Shader_HPBar : xx::Shader {
        using xx::Shader::Shader;
        GLint uTex0{ -1 }, aVert{ -1 }, aPosScale{ -1 }, aColor{ -1 }, aHPBar{ -1 }, aValue{ -1 };
        xx::GLVertexArrays va;
        xx::GLBuffer vb, ib;

        static constexpr int32_t maxNums{ 20000 };
        std::unique_ptr<Shader_HPBarData[]> data = std::make_unique_for_overwrite<Shader_HPBarData[]>(maxNums);
        Shared<GLTexture> tex;
        int32_t count{};

        void Init();
        virtual void Begin() override;
        virtual void End() override;
        void Commit();
        void SetTex(Shared<GLTexture> tex_);
        Shader_HPBarData* Draw(uint32_t v);
        Shader_HPBarData* Draw(uint32_t v1, uint32_t v2);
        Shader_HPBarData* Draw(std::string_view sv);
        void Draw(Shader_HPBarData const& d_);
    };

}
