#pragma once

#include <QOpenGLWidget>
#include "ui_View.h"

#include <map>
#include <memory>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QMatrix4x4>
#include <QTime>
#include <QVector>

#include "NeuronSim/Automaton.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)
QT_FORWARD_DECLARE_CLASS(QOpenGLShader)
QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class Layer;

class View : public QOpenGLWidget, protected QOpenGLFunctions, public Automaton::Listener
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

	void setAutomaton(std::shared_ptr<Automaton> automaton);
	void updateTextures();
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

private: // from Automaton::Listener
	void automatonLayerCreated(std::shared_ptr<Layer> layer);
	void automatonLayerRemoved(std::shared_ptr<Layer> layer);

private:
	void createMesh();
	void createSimpleStyle();
	void createTiledStyle();
	void createDonutStyle();
	void paintTexture();
	void checkGlError();

private:
	Ui::View ui;
	std::shared_ptr<Automaton> mAutomaton;
	QMatrix4x4 mModelView;
	QOpenGLShader *mVertexShader;
	QOpenGLShader *mFragmentShader;
	QOpenGLShaderProgram *mProgram;
	std::map<std::shared_ptr<Layer>, std::shared_ptr<QOpenGLTexture>> mTextures;
	int mAttrVertex;
	int mAttrTexCoord;
	int mAttrMatrix;
	int mAttrTexture;
	float mZoom;
	float mAspect;
	Style mStyle;
	StyleData mStyleData[STYLE_COUNT];
	std::vector<uint32_t> mImageData;
};
