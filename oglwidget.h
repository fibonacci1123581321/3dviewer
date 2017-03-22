#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
//#include </usr/include/GL/glu.h>
//#include </usr/include/GL/gl.h>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObjectFormat>
#include "shader.h"
#include "modeller.h"

class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    //Q_OBJECT

private:
    /*QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLShaderProgram *m_program;
    QOpenGLShader *m_shader;
    QOpenGLTexture *m_texture;*/


    int m_xRot;
    int m_yRot;
    int m_zRot;

    float lineWidth = 5.0f;
    float vertSize = 10.0f;

    bool m_core;
    QPointF mouse_last_pos;
    QPointF mouse_pos;
    QPoint m_lastPos;

    int width;
    int height;
    Modeller screenQuad;
    float time = 0.0f;

    QOpenGLFramebufferObjectFormat fboFormat;

    QOpenGLShaderProgram *shaded_shader;
    QOpenGLShaderProgram *edges_shader;
    QOpenGLShaderProgram *vertices_shader;
    QOpenGLShaderProgram *screen_shader;
    QOpenGLTexture * tex;
    QOpenGLFramebufferObject *fbo;
    Shader *shaded;
    Shader *lines;
    Shader *points;
    Shader *screen;

    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int m_normalMatrixLoc;
    int m_lightPosLoc;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;

    QMatrix4x4 ortho_proj;
    QMatrix4x4 ortho_camera;
    QMatrix4x4 ortho_world;


    int m_projMatrixLoc2;
    int m_mvMatrixLoc2;
    int m_normalMatrixLoc2;

    int oldX;
    int oldY;
    int newX;
    int newY;


    bool m_transparent;
    bool useArcBall;
    bool rotate;

public:
    OGLWidget(QWidget *parent = 0);
        ~OGLWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

//public slots:
    void cleanup();
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setLineWidth(int width);
    void setVertSize(int size);

//signals:
//    void xRotationChanged(int angle);
//    void yRotationChanged(int angle);
//    void zRotationChanged(int angle);
//    void lineWidthChanged(int width);
//    void vertSizeChanged(int size);


protected:
        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();
        //QVector3D getArcBallVector(int x, int y);
        QVector3D getArcballVector(const QPoint& pt, int width, int height);
        //void updateMouse();
        void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
        void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
        void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

private:
        void setupVertexAttribs(QOpenGLBuffer vbo);
        void setupModel(Modeller modeller, QOpenGLVertexArrayObject * vao);
        void renderModel(Modeller model, Shader *shader, QOpenGLVertexArrayObject * vao,
                        QMatrix4x4 camera, QMatrix4x4 projection, QMatrix4x4 world,
                        bool renderVertices, bool renderEdges, bool renderShaded);
        void setupScene();
        void updateModel(Modeller modeller);
        void addModel(Modeller scene[], Modeller newModel);
        Modeller translate(Modeller model, QVector3D vector3);
        void initShader(Shader *shader,
                        char *vertexShaderSource,
                        char *fragmentShaderSource);
};

#endif // OGLWIDGET_H
