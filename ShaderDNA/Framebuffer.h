#pragma once
#include <glad/glad.h>

struct Framebuffer
{
    GLuint fbo = 0;
    GLuint texture = 0;

    int width = 800;
    int height = 600;
};

struct fbos
{
    Framebuffer mainView;
    Framebuffer secondView;
    Framebuffer thirdView;
    Framebuffer fourthView;
    Framebuffer fifthView;
    Framebuffer sixthView;
    Framebuffer seventhView;
};