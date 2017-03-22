#pragma once
#include "oglwidget.h"
//#include "modeller.h"
#include <QOpenGLFunctions>
#include <qmath.h>
#include <QMouseEvent>
#include <iostream>

const int maxModelCount = 100;
int modelCount = 0;

Modeller scene[maxModelCount];
QOpenGLVertexArrayObject s_vao[maxModelCount];
QOpenGLVertexArrayObject screenQuad_vao;
QOpenGLBuffer s_vbo[maxModelCount];

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

void OGLWidget::cleanup()
{
    makeCurrent();

    for (int i = 0; i < maxModelCount; i++)
    {
        scene[i].vbo().destroy();
    }

    delete shaded_shader;
    delete edges_shader;
    delete vertices_shader;
    delete screen_shader;
    shaded_shader = 0;
    edges_shader = 0;
    vertices_shader = 0;
    screen_shader = 0;
    doneCurrent();
}

OGLWidget::~OGLWidget()
{
    /*makeCurrent();

    delete m_texture;
    delete m_shader;
    delete m_program;

    m_vbo.destroy();
    m_vao.destroy();

    doneCurrent();*/

    cleanup();
}

void OGLWidget::addModel(Modeller scene[], Modeller newModel)
{
    scene[modelCount] = newModel;
    modelCount += 1;
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void OGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        //emit xRotationChanged(angle);
        update();
    }
}

void OGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        //emit yRotationChanged(angle);
        update();
    }
}

void OGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        //emit zRotationChanged(angle);
        update();
    }
}

void OGLWidget::setLineWidth(int width)
{
    //printf("%i", width);
    lineWidth = float(width);
    //emit lineWidthChanged(width);
    update();
}
void OGLWidget::setVertSize(int size)
{
    //printf("%i", size);
    vertSize = float(size);
    //emit vertSizeChanged(size);
    update();
}

QSize OGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize OGLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void OGLWidget::setupScene()
{
    Modeller model;
    Modeller coolModel;
    //Add Some Geometry to models



    float w = 0.2f;
    float h = 0.2f;
    coolModel.quad(-w, h, -w, -h, w, -h, w, h);
    //model.quad(0.0f, -h, 0.0f, h, w, -h, w, 0.0f);
    model.quad(0.0f, -0.40f, 0.00f, -0.25f, 0.26f, -0.24f, 0.14f, -0.35f);

    //Add Model to Scene
    addModel(scene, coolModel);
    addModel(scene, model);
    //addModel(scene, coolModel);

    //Setup Each Model in Scene
    for (int i = 0; i < maxModelCount; i++)
    {
        setupModel(scene[i], &s_vao[i]);
    }
}

static char *vertexShaderSource =
"attribute vec4 vertex;\n"
"attribute vec3 normal;\n"
"attribute vec2 vertexUV;\n"
"varying vec3 vert;\n"
"varying vec3 vertNormal;\n"
"varying vec2 UV;\n"
"uniform mat4 projMatrix;\n"
"uniform mat4 mvMatrix;\n"
"uniform mat3 normalMatrix;\n"
"void main() {\n"
"   vert = vertex.xyz;\n"
"   vertNormal = normalMatrix * normal;\n"
"   gl_Position = projMatrix * mvMatrix * vertex;\n"
"   UV = vertexUV;\n"
"}\n";

static char *fragmentEdges =
"varying highp vec3 vert;\n"
"varying highp vec3 vertNormal;\n"
"uniform highp vec3 lightPos;\n"
"void main() {\n"
"   highp vec3 L = normalize(lightPos - vert);\n"
"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
"   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
"   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
"   gl_FragColor = vec4(vert, 1.0);\n"
"}\n";

static char *fragmentVertices =
"varying highp vec3 vert;\n"
"varying highp vec3 vertNormal;\n"
"uniform highp vec3 lightPos;\n"
"void main() {\n"
"   highp vec3 L = normalize(lightPos - vert);\n"
"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
"   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
"   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
"   gl_FragColor = vec4(vert, 1.0);\n"
"}\n";

static char *fragmentShaded =
"varying highp vec3 vert;\n"
"varying highp vec3 vertNormal;\n"
"varying highp vec2 UV;\n"
"uniform sampler2D diffuseTexture;"
"uniform highp vec3 lightPos;\n"
"void main() {\n"
"   highp vec3 L = normalize(lightPos - vert);\n"
"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
"   highp vec3 color = vec3(0.5, 0.5, 0.5);\n"
"   highp vec3 col = clamp(color * 0.01 + color * 0.8 * NL, 0.0, 1.0);\n"
"   gl_FragColor = vec4(UV,0, 1.0);\n"
//"   gl_FragColor = texture(diffuseTexture, UV );\n"
"}\n";


static char *screenVertexShader =
"attribute vec4 vertex;\n"
"attribute vec3 normal;\n"
"attribute vec2 vertexUV;\n"
"varying vec3 vert;\n"
"varying vec3 vertNormal;\n"
"varying vec2 UV;\n"
"uniform mat4 projMatrix;\n"
"uniform mat4 mvMatrix;\n"
"uniform mat3 normalMatrix;\n"
"void main() {\n"
"   vert = vertex.xyz;\n"
"   vertNormal = normalMatrix * normal;\n"
"   gl_Position = projMatrix * mvMatrix * vertex;\n"
"   UV = vertexUV;\n"
"}\n";
static char *screenFragmentShader =
"varying highp vec3 vert;\n"
"varying highp vec3 vertNormal;\n"
"varying highp vec2 UV;\n"
"uniform sampler2D diffuseTexture;"
"uniform highp vec3 lightPos;\n"
"void main() {\n"
"   highp vec3 L = normalize(lightPos - vert);\n"
"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
"   highp vec3 color = vec3(0.5, 0.5, 0.5);\n"
"   highp vec3 col = clamp(color * 0.01 + color * 0.8 * NL, 0.0, 1.0);\n"
"   gl_FragColor = vec4(UV,0, 1.0);\n"
//"   gl_FragColor =  vec4(0.7,0.7,0.7,1.0)*texture(diffuseTexture, UV );\n"
"}\n";

void OGLWidget::initShader(	Shader *shader,
                            char *vertexShaderSource,
                            char *fragmentShaderSource
)
{
    //Inits a shader program here
    shader->program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shader->program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shader->program->bindAttributeLocation("vertex", 0);
    shader->program->bindAttributeLocation("normal", 1);
    shader->program->bindAttributeLocation("vertexUV", 2);
    shader->program->link();
    shader->program->bind();
    shader->projMatrixLoc = shader->program->uniformLocation("projMatrix");
    shader->mvMatrixLoc = shader->program->uniformLocation("mvMatrix");
    shader->normalMatrixLoc = shader->program->uniformLocation("normalMatrix");
    shader->lightPosLoc = shader->program->uniformLocation("lightPos");
    shader->program->setUniformValue(shader->lightPosLoc, QVector3D(0, 0, 0.25f));
    shader->program->release();
}

void OGLWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &OGLWidget::cleanup);
    initializeOpenGLFunctions();

    //GLint viewport[4];
    //glGetIntegerv(GL_VIEWPORT, viewport);

    setupScene();
    ortho_camera.setToIdentity();
    m_camera.setToIdentity();
    m_camera.translate(0, 0, -1);
    //glDisable(GL_CULL_FACE);
    glClearColor(0.1f, 0.1f, 0.1f, 1);

    shaded = new Shader;
    lines = new Shader;
    points = new Shader;

    initShader(shaded, vertexShaderSource, fragmentShaded);
    initShader(lines, vertexShaderSource, fragmentEdges);
    initShader(points, vertexShaderSource, fragmentVertices);

    //fboFormat.setAttachment(QOpenGLFramebufferObject::Depth);


    screen = new Shader;
    initShader(screen, screenVertexShader, screenFragmentShader);

    //Screen Quad
    float size = 1.0f;
    screenQuad.quad(-1.0f*size,-1.0f*size,
                    1.0f*size, -1.0f*size,
                    1.0f*size, 1.0f*size,
                    -1.0f*size, 1.0f*size);
    setupModel(screenQuad, &screenQuad_vao);
}

void OGLWidget::setupModel(Modeller modeller, QOpenGLVertexArrayObject * vao)
{
    vao->create();
    QOpenGLVertexArrayObject::Binder vaoBinder(vao);

    modeller.vbo().create();
    modeller.vbo().bind();
    modeller.vbo().allocate(modeller.constData(), modeller.count() * sizeof(GLfloat));
    setupVertexAttribs(modeller.vbo());
}

void OGLWidget::updateModel(Modeller modeller)
{
    modeller.vbo().bind();
    modeller.vbo().allocate(modeller.constData(), modeller.count() * sizeof(GLfloat));
}

void OGLWidget::setupVertexAttribs(QOpenGLBuffer vbo)
{
    vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glEnableVertexAttribArray(2);

    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(6 * sizeof(GLfloat)));
    vbo.release();
}

void OGLWidget::renderModel(Modeller model,
                           Shader *shader,
                           QOpenGLVertexArrayObject * vao,
                           QMatrix4x4 camera, QMatrix4x4 projection, QMatrix4x4 world,
                           bool renderVertices, bool renderEdges, bool renderShaded)
{
    QOpenGLVertexArrayObject::Binder vaoBinder(vao);
    shader->program->bind();
    shader->program->setUniformValue(shader->projMatrixLoc, projection);
    shader->program->setUniformValue(shader->mvMatrixLoc, camera * world);
    QMatrix3x3 normalMatrix = world.normalMatrix();
    shader->program->setUniformValue(shader->normalMatrixLoc, normalMatrix);

    if (renderShaded)
    {
        glDisable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, model.vertexCount());
    }
    if (renderEdges)
    {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(lineWidth);
        glDrawArrays(GL_TRIANGLES, 0, model.vertexCount());
    }
    if (renderVertices)
    {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glPointSize(vertSize);
        glDrawArrays(GL_TRIANGLES, 0, model.vertexCount());
    }

    shader->program->release();
}

Modeller OGLWidget::translate(Modeller model, QVector3D vector3)
{
    for (int i = 0; i < model.vertexCount(); i++)
    {
        int vertex = i + 1;
        int x = 3;
        int y = 2;
        int z = 1;
        int index = (8 * vertex) - (x + 5);
        model.data[index] = model.data[index] + vector3.x();
        index = (8 * vertex) - (y + 5);
        model.data[index] = model.data[index] + vector3.y();
        index = (8 * vertex) - (z + 5);
        model.data[index] = model.data[index] + vector3.z();
    }
    return model;
}

void OGLWidget::paintGL()
{
    time += 0.01f;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);


    /*m_world.setToIdentity();
    m_world.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);*/

    float sinTime = (qSin(time*2.0f));
    //float sinTime2 = (qSin((time+0.5)*10.0f));
    printf("time: %f \n", sinTime);

    //scene[0] = translate(scene[0], QVector3D(sinTime*0.0005f, 0.0f, 0.0f));
    //scene[0] = translate(scene[0], QVector3D(0.0f, sinTime2*0.001f, 0.0f));

    for (int i = 0; i < maxModelCount; i++)
    {
        updateModel(scene[i]);
    }



    for (int i = 0; i < maxModelCount; i++)
    {
        renderModel(scene[i], shaded, &s_vao[i], m_camera, m_proj, m_world, false, false, true);
        renderModel(scene[i], points, &s_vao[i], m_camera, m_proj, m_world, false, true, false);
        renderModel(scene[i], lines, &s_vao[i], m_camera, m_proj, m_world, true, false, false);
    }
}

void OGLWidget::resizeGL(int w, int h)
{
    width = w;
    height = h;
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);

    ortho_proj.setToIdentity();
    ortho_proj.ortho(QRect(0, 0, width, height));
}

/*QVector3D OGLWidget::getArcBallVector(int x, int y)
{
   QVector3D pt = QVector3D(2.0 * x / this->widthMM() - 1.0, 2.0 * y / this->heightMM() - 1.0 , 0);
   pt.setY(pt.y() * -1);

   // compute z-coordinates

   float xySquared = pt.x() * pt.x() + pt.y() * pt.y();

   if(xySquared <= 1.0)

       pt.setZ(std::sqrt(1.0 - xySquared));
   else
       pt.normalize();

   return pt;

}*/

QVector3D OGLWidget::getArcballVector(const QPoint& pt, int width, int height)
{
    QVector3D P = QVector3D(1.0*pt.x()/width * 2 - 1.0,
                            1.0*pt.y()/height * 2 - 1.0,
                            0);
    P.setY(-P.y());

    float OP_squared = P.x()*P.x() + P.y()*P.y();
    if (OP_squared <= 1)
    {
        P.setZ(std::sqrt(1 - OP_squared));  // Pythagore
    }
    else
    {
        P.normalize();  // nearest point
        //OP_squared = P.lengthSquared();
        //P.setZ(std::sqrt(1 - OP_squared));
    }
    return P;
}

/*void OGLWidget::updateMouse()
{

        QVector3D v = getArcBallVector(oldX,oldY); // from the mouse
        QVector3D u = getArcBallVector(newX, newY);

        float angle = std::acos(std::min(1.0f, QVector3D::dotProduct(u,v)));

        QVector3D rotAxis = QVector3D::crossProduct(v,u);

        QMatrix4x4 eye2ObjSpaceMat = m_camera.inverted();

        QVector3D objSpaceRotAxis = eye2ObjSpaceMat * rotAxis;

        //qDebug() << 4 * qRadiansToDegrees(angle);


        //modelview.rotate(4 * qRadiansToDegrees(angle), rotAxis);

        //oldRot = newRot;

        //oldX = newX;
        //oldY = newY;

        //qDebug() << objSpaceRotAxis.normalized();


    if(true)
    {
        QMatrix4x4 tmp;
        tmp.rotate(4 * qRadiansToDegrees(angle), objSpaceRotAxis);
        m_camera = tmp * m_camera;
    }

}*/

//void OGLWidget::mousePressEvent(QMouseEvent *event)
//{
//    rotate=false;
//
//    if(event->button() == Qt::LeftButton)
//    {
//        oldX = event->x(); // Set this to the mouse position
//        oldY = event->y(); // Set this to the mouse position
//
//       newX = event->x();
//      newY = event->y();
//
//        //qDebug() << oldX << oldY << newX << newY;
//
//        rotate = true;
//
//        useArcBall = true;
//     }
//
//    /*//mouse_last_pos = QPointF(float(event->pos().x()) / float(width), float(event->pos().y()) / float(height));
//        m_lastPos = event->pos();
//
//        if (event->buttons() & Qt::LeftButton) {
//            //printf("Image saved");
//            //fbo->toImage().save("image.png");
//            //printf("%f,%f \n", mouse_last_pos.x(), mouse_last_pos.y());
//        }
//       else if (event->buttons() & Qt::RightButton) {
//
//    }*/
//}

void OGLWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons ()==Qt::LeftButton)
    {
        m_lastPos=e->pos();
    }
}

//void OGLWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    if(event->buttons() & Qt::LeftButton)
//    {
//        //qDebug() << QString::number(e->x());
//        if(rotate)
//        {
//            newX = event->x();
//            newY = event->y();
//            updateMouse();
//        }
//
//        oldX = event->x();
//        oldY = event->y();
//        update();
//    }
//
//    /*//mouse_pos = QPointF(float(event->pos().x()) / float(width), float(event->pos().y()) / float(height));
//
//
//        int dx = event->x() - m_lastPos.x();
//        int dy = event->y() - m_lastPos.y();
//
//        if (event->buttons() & Qt::LeftButton) {
//            setXRotation(m_xRot + 8 * dy);
//            setYRotation(m_yRot + 8 * dx);
//        }
//        else if (event->buttons() & Qt::RightButton) {
//            //setXRotation(m_xRot + 8 * dy);
//            //setZRotation(m_zRot + 8 * dx);
//            m_camera.translate(0, 0, (float(1 - dy) / float(height))*1.5f);
//        }
//        else if (event->buttons() & Qt::MiddleButton) {
//            m_camera.translate((float(dx)/float(width))*0.5f,( float(1-dy)/float(height))*0.5f, 0);
//        }
//
//    m_lastPos = event->pos();*/
//
//}

void OGLWidget::mouseMoveEvent(QMouseEvent * e)
{
    if(e->buttons ()==Qt::LeftButton)
    {
        if(!m_lastPos.isNull () && m_lastPos!=e->pos ())
        {
            //rotate using an arcBall for freeform rotation
            QVector3D vec1 = getArcballVector(m_lastPos, width, height);
            QVector3D vec2 = getArcballVector(e->pos(), width, height);

            std::cout << width << "\t" << height << "\t" << e->pos().x() << "\t" << e->pos().y() << std::endl;

            //use bisector to get half-angle cos and sin from dot and cross product
            // for quaternion initialisation
            QVector3D vec3 = vec1+vec2;
            vec3.normalize();
            QVector3D rotaxis = QVector3D::crossProduct(vec1, vec3);
            double cos = QVector3D::dotProduct(vec1, vec3);

            //vec1.normalize();
            //vec2.normalize();
            //QVector3D rotaxis = QVector3D::crossProduct(vec1, vec2);
            //double cos = QVector3D::dotProduct(vec1, vec2);

            QQuaternion quat(cos,rotaxis);
            quat.normalize();
            //we want to left-multiply rMat with quat but that isn't available
            //so we'll have to do it the long way around
            QMatrix4x4 rot;
            rot.rotate(quat);
            //rMat=rot*rMat;
            m_world = rot*m_world;

            //updateGL();
            update();
        }
        m_lastPos=e->pos();
    }
}

void OGLWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if(event->button() == Qt::LeftButton)
    {
        useArcBall = false;
        rotate = false;
    }

    /*//QPoint mouse_at_release = event->pos();

        // example code
        //QPoint dx = mouse_at_release - mouse_last_pos;

        if (event->buttons() & Qt::LeftButton) {

        }
        else if (event->buttons() & Qt::RightButton) {

    }*/
}
