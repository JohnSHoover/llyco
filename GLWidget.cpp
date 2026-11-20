
#include "GLWidget.h"
#include <QDebug>

void MyGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // 1. Compile and link shaders
    if (!m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, kVertexShaderSource))
        qWarning() << "Vertex shader error:" << m_program.log();
    if (!m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, kFragmentShaderSource))
        qWarning() << "Fragment shader error:" << m_program.log();
    if (!m_program.link())
        qWarning() << "Shader link error:" << m_program.log();

    // 2. Bind VAO first so it captures the VBO and attribute pointers
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // 3. Create VBO and upload vertex data
    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(kTriangleVertices, sizeof(kTriangleVertices));

    // 4. Configure vertex attribute pointers (Stride = 6 floats)
    const int stride = 6 * sizeof(float);

    // Location 0: Position (3 floats, offset 0)
    m_program.enableAttributeArray(0);
    m_program.setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

    // Location 1: Color (3 floats, offset 3 floats)
    m_program.enableAttributeArray(1);
    m_program.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, stride);

    m_vbo.release();
}

void resizeGL(int w, int h) override {
    glViewport(0, 0, w, h);

    // Update perspective projection matrix
    const float aspect = float(w) / float(h ? h : 1);
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, aspect, 0.1f, 100.0f);
}

void paintGL() override {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Build Model-View-Projection matrix
    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -2.5f);

    QMatrix4x4 model; // Identity (or apply rotations/translations here)
    QMatrix4x4 mvp = m_projection * view * model;

    m_program.bind();
    m_program.setUniformValue("uMvp", mvp);

    {
        QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    m_program.release();
}
