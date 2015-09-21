/*
 * Copyright (c) 2014, 2015 Ashley Davis (SgtCoDFish)
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdint>

#include <string>
#include <utility>
#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include "APG/SpriteBatch.hpp"
#include "APG/ShaderProgram.hpp"
#include "APG/Buffer.hpp"
#include "APG/APGCommon.hpp"
#include "APG/Sprite.hpp"
#include "APG/Mesh.hpp"
#include "APG/VertexAttribute.hpp"
#include "APG/Game.hpp"

#include "APG/internal/Assert.hpp"

const char * const APG::SpriteBatch::POSITION_ATTRIBUTE = "position";
const char * const APG::SpriteBatch::COLOR_ATTRIBUTE = "color";
const char * const APG::SpriteBatch::TEXCOORD_ATTRIBUTE = "texcoord";
const uint32_t APG::SpriteBatch::DEFAULT_BUFFER_SIZE = 1000;

APG::SpriteBatch::SpriteBatch(ShaderProgram * const program, uint32_t bufferSize) :
                bufferSize(bufferSize),
                vao(),
                vertexBuffer( { VertexAttribute(POSITION_ATTRIBUTE, AttributeUsage::POSITION, 2), //
                VertexAttribute(COLOR_ATTRIBUTE, AttributeUsage::COLOR, 4), //
                VertexAttribute(TEXCOORD_ATTRIBUTE, AttributeUsage::TEXCOORD, 2) }),
                indexBuffer(false),
                vertices(bufferSize, 0.0f),
                color(1.0f, 1.0f, 1.0f, 1.0f),
                projectionMatrix(
                        glm::ortho(0.0f, (float) APG::Game::screenWidth, (float) APG::Game::screenHeight, 0.0f, 0.0f,
                                1.0f)),
                transformMatrix(1.0f),
                combinedMatrix(1.0f) {
    if (program == nullptr) {
        this->ownedShaderProgram = SpriteBatch::createDefaultShader();

        this->program = ownedShaderProgram.get();
    } else {
        this->program = program;
    }

    const unsigned int indicesLength = bufferSize * 6;
    std::vector < uint16_t > indices(indicesLength, 0);

    uint16_t j = 0;
    for (unsigned int i = 0; i < indicesLength; i += 6, j += 4) {
        indices[i + 0] = j;
        indices[i + 1] = j + 1;
        indices[i + 2] = j + 2;
        indices[i + 3] = j + 2;
        indices[i + 4] = j + 3;
        indices[i + 5] = j;
    }

    indexBuffer.setData(indices, indices.size());
}

void APG::SpriteBatch::switchTexture(APG::Texture * const newTexture) {
    flush();
    lastTexture = newTexture;
}

void APG::SpriteBatch::setupMatrices() {
    combinedMatrix = projectionMatrix * transformMatrix;
    program->setUniformf("projTrans", combinedMatrix);
}

void APG::SpriteBatch::draw(APG::Texture * const image, float x, float y, uint32_t width, uint32_t height, float srcX,
        float srcY, uint32_t srcWidth, uint32_t srcHeight) {
    REQUIRE(drawing, "Must call begin() before draw().");
    REQUIRE(image != nullptr, "Can't draw null image");

//    REQUIRE(srcX >= 0 && srcX < image->getWidth() && srcX + srcWidth < image->getWidth(),
//            "Image coordinates must be inside image in SpriteBatch::draw.");
//    REQUIRE(srcY >= 0 && srcY < image->getHeight() && srcY + srcHeight < image->getHeight(),
//            "Image coordinates must be inside image in SpriteBatch::draw.");

    if (image != lastTexture) {
        switchTexture(image);
    }

    const auto u1 = srcX * image->getInvWidth();
    const auto v1 = srcY * image->getInvHeight();
    const auto u2 = (srcX + srcWidth) * image->getInvWidth();
    const auto v2 = (srcY + srcHeight) * image->getInvHeight();

    const auto finalX = x + width;
    const auto finalY = y + height;

    vertices[idx++] = x;
    vertices[idx++] = y;
    vertices[idx++] = color.r;
    vertices[idx++] = color.g;
    vertices[idx++] = color.b;
    vertices[idx++] = color.a;
    vertices[idx++] = u1;
    vertices[idx++] = v1;

    vertices[idx++] = x;
    vertices[idx++] = finalY;
    vertices[idx++] = color.r;
    vertices[idx++] = color.g;
    vertices[idx++] = color.b;
    vertices[idx++] = color.a;
    vertices[idx++] = u1;
    vertices[idx++] = v2;

    vertices[idx++] = finalX;
    vertices[idx++] = finalY;
    vertices[idx++] = color.r;
    vertices[idx++] = color.g;
    vertices[idx++] = color.b;
    vertices[idx++] = color.a;
    vertices[idx++] = u2;
    vertices[idx++] = v2;

    vertices[idx++] = finalX;
    vertices[idx++] = y;
    vertices[idx++] = color.r;
    vertices[idx++] = color.g;
    vertices[idx++] = color.b;
    vertices[idx++] = color.a;
    vertices[idx++] = u2;
    vertices[idx++] = v1;

    if (idx > bufferSize / 2) {
        flush();
    }
}

void APG::SpriteBatch::draw(APG::SpriteBase * const sprite, float x, float y) {
    REQUIRE(drawing, "Must call begin() before draw().");
    REQUIRE(sprite != nullptr, "Cannot draw null sprite in SpriteBatch.");

    if (sprite->getTexture() != lastTexture) {
        switchTexture(sprite->getTexture());
    }

    const auto u1 = sprite->getU();
    const auto v1 = sprite->getV();
    const auto u2 = sprite->getU2();
    const auto v2 = sprite->getV2();

    vertices[idx++] = x;
    vertices[idx++] = y;
    vertices[idx++] = color.r;
    vertices[idx++] = color.g;
    vertices[idx++] = color.b;
    vertices[idx++] = color.a;
    vertices[idx++] = u1;
    vertices[idx++] = v1;

    vertices[idx++] = x;
    vertices[idx++] = y + sprite->getHeight();
    vertices[idx++] = color.r;
    vertices[idx++] = color.g;
    vertices[idx++] = color.b;
    vertices[idx++] = color.a;
    vertices[idx++] = u1;
    vertices[idx++] = v2;

    vertices[idx++] = x + sprite->getWidth();
    vertices[idx++] = y + sprite->getHeight();
    vertices[idx++] = color.r;
    vertices[idx++] = color.g;
    vertices[idx++] = color.b;
    vertices[idx++] = color.a;
    vertices[idx++] = u2;
    vertices[idx++] = v2;

    vertices[idx++] = x + sprite->getWidth();
    vertices[idx++] = y;
    vertices[idx++] = color.r;
    vertices[idx++] = color.g;
    vertices[idx++] = color.b;
    vertices[idx++] = color.a;
    vertices[idx++] = u2;
    vertices[idx++] = v1;

    if (idx > bufferSize / 2) {
        flush();
    }
}

void APG::SpriteBatch::flush() {
    if (idx == 0) {
        return;
    }

    vao.bind();
    lastTexture->bind();
    program->setUniformi("tex", lastTexture->getGLTextureUnit());
    vertexBuffer.setData(vertices, idx);
    vertexBuffer.bind(program);
    indexBuffer.bind();

    //TODO: Fix hacky sizeof.
    const int strideInBytes = (vertexBuffer.getAttributes().getStride() * sizeof(GLfloat));
    const int spriteCount = idx / strideInBytes;
    const int indexCount = spriteCount * 6;

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0);

    idx = 0;
}

void APG::SpriteBatch::begin() {
    drawing = true;
    setupMatrices();
    program->use();

    glEnable (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void APG::SpriteBatch::end() {
    REQUIRE(drawing, "Can't end before a begin in SpriteBatch.");

    drawing = false;

    if (idx > 0) {
        flush();
    }
}

void APG::SpriteBatch::setProjectionMatrix(const glm::mat4 &matrix) {
    if (drawing) {
        flush();
    }

    this->projectionMatrix = matrix;

    if (drawing) {
        setupMatrices();
    }
}

std::unique_ptr<APG::ShaderProgram> APG::SpriteBatch::createDefaultShader() {
    std::stringstream vertexShaderStream, fragmentShaderStream;

    vertexShaderStream << "#version 150 core\n" //
            << "in vec2 " << POSITION_ATTRIBUTE << ";\n" //
            << "in vec4 " << COLOR_ATTRIBUTE << ";\n" //
            << "in vec2 " << TEXCOORD_ATTRIBUTE << ";\n" //
            << "out vec2 frag_texcoord;\n" //
            << "out vec4 frag_color;\n" //
            << "uniform mat4 projTrans;\n" //
            << "void main() {\n" //
            << "frag_color = " << COLOR_ATTRIBUTE << ";\n" //
            << "frag_texcoord = " << TEXCOORD_ATTRIBUTE << ";\n" //
            << "gl_Position = projTrans * vec4(" << POSITION_ATTRIBUTE << ", 0.0, 1.0);" //
            << "}\n\n";

    fragmentShaderStream << "#version 150 core\n" //
            << "in vec4 frag_color;\n"  //
            << "in vec2 frag_texcoord;\n"  //
            << "out vec4 outColor;\n"  //
            << "uniform sampler2D tex;\n"  //
            << "void main() {\n"  //
            << "outColor = frag_color * texture(tex, frag_texcoord);\n"  //
            << "}\n\n";

    const auto vertexShader = vertexShaderStream.str();
    const auto fragmentShader = fragmentShaderStream.str();

    return ShaderProgram::fromSource(vertexShader, fragmentShader);
}
