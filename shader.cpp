#include "shader.h"
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QtMath>

Shader::Shader()
{
    program = new QOpenGLShaderProgram;
}
