#pragma once

#include <QOpenGLWidget>
#include "ui_View.h"

#include <map>
#include <memory>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <qopengltexture.h>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QMatrix4x4>
#include <QTime>
#include <QVector>

#include "NeuronSim/Automaton.h"

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
		STYLE_STACKED,

		STYLE_COUNT
	};
	struct StyleData
	{
		QOpenGLVertexArrayObject mVao;
		QOpenGLBuffer mVertexBuffer;
		int mIndexCount;
		GLuint mIndexBuffer;
		bool m3D;
		QVector3D mEyePos;
		QVector3D mLookAt;
		QVector3D mUp;
		float mPov;
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
	void setStyle(const QString & style);

private:
	void mousePressEvent(QMouseEvent * ev) override;
	void mouseMoveEvent(QMouseEvent * ev) override;
	void mouseReleaseEvent(QMouseEvent * ev) override;

private: // from QOpenGLWidget
	void resizeGL(int w, int h) override;
	void paintGL() override;
	void initializeGL() override;

private: // from Automaton::Listener
	void automatonSizechanged(int width, int height);
	void automatonLayerCreated(std::shared_ptr<Layer> layer);
	void automatonLayerRemoved(std::shared_ptr<Layer> layer);

private:
	void createMesh();
	void createSimpleStyle();
	void createTiledStyle();
	void createDonutStyle();
	void createStackedStyle();
	void paintTexture();
	void checkGlError(char * fun, int line);
	void createTextureForLayer(std::shared_ptr<Layer> layer);
	inline QPoint View::layerCoords(const QPoint & pos);

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
	int mAttrPos;
	int mAttrMatrix;
	int mAttrTexture;
	int mAttrColor;
	float mZoom;
	float mPixelSize;
	float mAspect;
	Style mStyle;
	StyleData mStyleData[STYLE_COUNT];
	std::vector<uint32_t> mImageData;
	QPoint mMousePos;
	float mTranslateX;
	float mTranslateY;
	bool mMouseDown;
};
