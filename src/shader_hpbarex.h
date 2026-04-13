#pragma once
#include <xx_shader.h>

namespace xx {

    struct Shader_HPBarExData {
        XY pos{}, scale{ 1, 1 };                // float * 4
        xx::RGBA8 color{ 255, 255, 255, 255 };  // u8 * 4
        uint32_t numbers[4];                    // u32 * 4 / u8 * 16   // last one is len
        float value{};                          // float * 1

        void Fill(XY pos_, XY scale_, xx::RGBA8 color_, float value_, std::string_view sv_);
    };

    struct Shader_HPBarEx : xx::Shader {
        using xx::Shader::Shader;
        GLint uTex0{ -1 }, aVert{ -1 }, aPosScale{ -1 }, aColor{ -1 }, aHPBar{ -1 }, aValue{ -1 };
        xx::GLVertexArrays va;
        xx::GLBuffer vb, ib;

        static constexpr int32_t maxNums{ 20000 };
        std::unique_ptr<Shader_HPBarExData[]> data = std::make_unique_for_overwrite<Shader_HPBarExData[]>(maxNums);
        Shared<GLTexture> tex;
        int32_t count{};

        void Init();
        virtual void Begin() override;
        virtual void End() override;
        void Commit();
        void SetTex(Shared<GLTexture> tex_);
        Shader_HPBarExData* Alloc();
    };

}
