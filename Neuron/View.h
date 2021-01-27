#pragma once

#include <QOpenGLWidget>
#include "ui_View.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
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
	enum Style
	{
		STYLE_SIMPLE = 0,
		STYLE_TILED,
		STYLE_DONUT,

		STYLE_COUNT
	};
	struct StyleData
	{
		QOpenGLVertexArrayObject mVao;
		QOpenGLBuffer mVertexBuffer;
		int mIndexCount;
		GLuint mIndexBuffer;
		bool m3D;
	};
public:
	View(QWidget *parent = Q_NULLPTR);
	~View();

	void resizeTexture(int width, int height);
	void updateTexture(uint32_t * data);
	void setZoom(float zoom);
	void setProjection();
	void setOrtho();
	void setPerspective();

public slots:
	void setStyle(const QString & style);

private: // from QOpenGLWidget
	void resizeGL(int w, int h) override;
	void paintGL() override;
	void initializeGL() override;

private:
	void createMesh();
	void createSimpleStyle();
	void createTiledStyle();
	void createDonutStyle();
	void paintTexture();
	void checkGlError();

private:
	Ui::View ui;
	QMatrix4x4 mModelView;
	QOpenGLShader *mVertexShader;
	QOpenGLShader *mFragmentShader;
	QOpenGLShaderProgram *mProgram;
	std::unique_ptr<QOpenGLTexture> mTexture;
	int mAttrVertex;
	int mAttrTexCoord;
	int mAttrMatrix;
	int mAttrTexture;
	float mZoom;
	float mAspect;
	Style mStyle;
	StyleData mStyleData[STYLE_COUNT];
};
