#pragma once

#include <glad/glad.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace ShaderLoader
{
    inline std::string readFile(const std::filesystem::path& path)
    {
        std::ifstream file(path);

        if (!file.is_open())
            throw std::runtime_error(
                "Shader-Datei konnte nicht geoeffnet werden: "
                + path.string()
            );

        std::ostringstream content;
        content << file.rdbuf();

        if (file.bad())
            throw std::runtime_error(
                "Fehler beim Lesen: " + path.string()
            );

        return content.str();
    }

    inline GLuint compile(GLenum type, const std::string& source,
        const std::filesystem::path& path)
    {
        GLuint shader = glCreateShader(type);

        if (shader == 0)
            throw std::runtime_error("glCreateShader fehlgeschlagen.");

        const char* code = source.c_str();
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success != GL_TRUE)
        {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

            std::string log(length > 0 ? length : 1, '\0');
            glGetShaderInfoLog(
                shader, static_cast<GLsizei>(log.size()),
                nullptr, log.data()
            );

            glDeleteShader(shader);

            throw std::runtime_error(
                "Shader-Kompilierung fehlgeschlagen: "
                + path.string() + "\n" + log
            );
        }

        return shader;
    }

    inline GLuint load(const std::string& name)
    {
        const std::filesystem::path directory = SHADER_DIRECTORY;

        const auto vertexPath = directory / (name + ".vert");
        const auto fragmentPath = directory / (name + ".frag");

        // Beide Dateien lesen, bevor OpenGL-Objekte erzeugt werden.
        const auto vertexSource = readFile(vertexPath);
        const auto fragmentSource = readFile(fragmentPath);

        GLuint vertex = 0;
        GLuint fragment = 0;
        GLuint program = 0;

        try
        {
            vertex = compile(GL_VERTEX_SHADER, vertexSource, vertexPath);
            fragment = compile(
                GL_FRAGMENT_SHADER, fragmentSource, fragmentPath
            );

            program = glCreateProgram();

            if (program == 0)
                throw std::runtime_error("glCreateProgram fehlgeschlagen.");

            glAttachShader(program, vertex);
            glAttachShader(program, fragment);
            glLinkProgram(program);

            GLint success = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &success);

            if (success != GL_TRUE)
            {
                GLint length = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

                std::string log(length > 0 ? length : 1, '\0');
                glGetProgramInfoLog(
                    program, static_cast<GLsizei>(log.size()),
                    nullptr, log.data()
                );

                throw std::runtime_error(
                    "Shader-Linking fehlgeschlagen:\n" + log
                );
            }

            glDetachShader(program, vertex);
            glDetachShader(program, fragment);
            glDeleteShader(vertex);
            glDeleteShader(fragment);

            return program;
        }
        catch (...)
        {
            if (program) glDeleteProgram(program);
            if (vertex) glDeleteShader(vertex);
            if (fragment) glDeleteShader(fragment);
            throw;
        }
    }
}