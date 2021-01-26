#pragma once

#include <QOpenGLWidget>
#include "ui_View.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QMatrix4x4>
#include <QTime>
#include <QVector>

QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)
QT_FORWARD_DECLARE_CLASS(QOpenGLShader)
QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class View : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

private:
	struct Vertex
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLfloat u;
		GLfloat v;
	};
public:
	View(QWidget *parent = Q_NULLPTR);
	~View();

	void resizeTexture(int width, int height);
	void updateTexture(uint32_t * data);
	void setZoom(float zoom);
	void setOrtho();

private: // from QOpenGLWidget
	void resizeGL(int w, int h) override;
	void paintGL() override;
	void initializeGL() override;

private:
	void createQuad();
	void paintTexture();
	void fixQuadAspectRatio(int width, int height);

private:
	Ui::View ui;
	QMatrix4x4 mModelView;
	QOpenGLShader *mVertexShader;
	QOpenGLShader *mFragmentShader;
	QOpenGLShaderProgram *mProgram;
	std::unique_ptr<QOpenGLTexture> mTexture;
	QOpenGLBuffer mVertexBuffer;
	std::vector<Vertex> mVertices;
	int mAttrVertex;
	int mAttrTexCoord;
	int mAttrMatrix;
	int mAttrTexture;
	float mZoom;
};
