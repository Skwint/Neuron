#include "View.h"

#include <QEvent.h>
#include <QPainter>
#include <QPaintEngine>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <qtimer.h>
#include <iostream>
#include <GL/glu.h>

#include "NeuronSim/Automaton.h"
#include "NeuronSim/Constants.h"
#include "NeuronSim/Layer.h"
#include "NeuronSim/Log.h"
#include "Mat33f.h"
#include "Vec3f.h"

#define CHECK_GL_ERROR checkGlError(__FUNCTION__, __LINE__)

View::View( QWidget *parent)
	: QOpenGLWidget(parent),
	mProgram(0),
	mZoom(1.0f),
	mAspect(1.0f),
	mStyle(STYLE_TILED),
	mTranslateX(0.0f),
	mTranslateY(0.0f),
	mMouseDown(false)
{
	ui.setupUi(this);
}

View::~View()
{
	mAutomaton->removeListener(this);
	makeCurrent();
	delete mProgram;
	delete mVertexShader;
	delete mFragmentShader;
	for (int style = 0; style < STYLE_COUNT; ++style)
	{
		mStyleData[style].mVao.destroy();
		mStyleData[style].mVertexBuffer.destroy();
	}
	mTextures.clear();
	doneCurrent();
}

inline QPoint View::layerCoords(const QPoint & pos)
{
	return QPoint((pos.x() - width() / 2) / mPixelSize + mAutomaton->width() / 2,
		(pos.y() - height() / 2) / mPixelSize + mAutomaton->height() / 2);
}

void View::mousePressEvent(QMouseEvent * ev)
{
	if (!mStyleData[mStyle].m3D)
	{
		mMousePos = ev->pos();
		mMouseDown = true;
	}
}

void View::mouseMoveEvent(QMouseEvent * ev)
{
	if (!mStyleData[mStyle].m3D && mMouseDown)
	{
		auto pos = ev->pos();
		mTranslateX += float(pos.x() - mMousePos.x()) * mPixelSize;
		mTranslateY += float(pos.y() - mMousePos.y()) * mPixelSize;
		mMousePos = pos;
		if (mStyle == STYLE_TILED)
		{
			if (mTranslateX < -1.0f * mZoom)
				mTranslateX += 2.0f * mZoom;
			else if (mTranslateX > 1.0f * mZoom)
				mTranslateX -= 2.0f * mZoom;
			if (mTranslateY < -1.0f * mZoom)
				mTranslateY += 2.0f * mZoom;
			else if (mTranslateY > 1.0f * mZoom)
				mTranslateY -= 2.0f * mZoom;
		}
		setOrtho();
		update();
	}
}

void View::mouseReleaseEvent(QMouseEvent * ev)
{
	if (!mStyleData[mStyle].m3D)
	{
		mMouseDown = false;
	}
}

// This can't be set in the constructor because of oddities around QtDesigner widget initialization
// It really should be, however, from a design point of view. Call this as soon as possible after
// creating the GUI object.
void View::setAutomaton(std::shared_ptr<Automaton> automaton)
{
	if (mAutomaton)
	{
		mAutomaton->removeListener(this);
	}
	mAutomaton = automaton;
	mAutomaton->addListener(this);
	automatonSizechanged(mAutomaton->width(), mAutomaton->height());
}

void View::updateTextures()
{
	for (auto & texture : mTextures)
	{
		assert(texture.first->width() * texture.first->height() <= mImageData.size());
		assert(texture.second->width() == texture.first->width());
		assert(texture.second->height() == texture.first->height());
		texture.second->bind();
		texture.first->paint(&mImageData[0]);
		texture.second->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, &mImageData[0]);
		CHECK_GL_ERROR;
	}
}

void View::resizeGL(int width, int height)
{
	mAspect = double(width) / double(height);
	setProjection();
}

void View::setProjection()
{
	if (mStyleData[mStyle].m3D)
	{
		setPerspective();
	}
	else
	{
		setOrtho();
	}
	update();
}

void View::setPerspective()
{
	mModelView.setToIdentity();
	mModelView.perspective(mStyleData[mStyle].mPov, mAspect, 0.1f, 100.0f);
}

void View::setOrtho()
{
	float left = -1.0f;
	float right = 1.0f;
	float bottom = 1.0f;
	float top = -1.0f;
	if (mAspect > 1.0f)
	{
		left *= mAspect;
		right *= mAspect;
		mPixelSize = 2.0f / height();
	}
	else
	{
		top /= mAspect;
		bottom /= mAspect;
		mPixelSize = 2.0f / width();
	}
	mModelView.setToIdentity();
	mModelView.ortho(left, right, bottom, top, -2.0f, 2.0f);
	mModelView.translate(mTranslateX, mTranslateY);
	mModelView.scale(mZoom);

	CHECK_GL_ERROR;
}

void View::initializeGL()
{
	initializeOpenGLFunctions();

	mVertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
	const char *vertexSource =
		"attribute highp vec4 vertex;\n"
		"attribute highp vec4 texCoord;\n"
		"uniform highp vec4 pos;\n"
		"uniform mediump mat4 matrix;\n"
		"varying highp vec4 texc;\n"
		"void main(void)\n"
		"{\n"
		"    gl_Position = matrix * (pos + vertex);\n"
		"    texc = texCoord;\n"
		"}\n";
	mVertexShader->compileSourceCode(vertexSource);

	mFragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
	const char *fragmentSource =
		"varying highp vec4 texc;\n"
		"uniform sampler2D tex;\n"
		"uniform vec4 color;\n"
		"void main(void)\n"
		"{\n"
		"    gl_FragColor = color * vec4(texture2D(tex, texc.st).rgba);\n"
		"}\n";
	mFragmentShader->compileSourceCode(fragmentSource);

	mProgram = new QOpenGLShaderProgram;
	mProgram->addShader(mVertexShader);
	mProgram->addShader(mFragmentShader);
	mProgram->link();

	mAttrVertex = mProgram->attributeLocation("vertex");
	mAttrTexCoord = mProgram->attributeLocation("texCoord");
	mAttrPos = mProgram->uniformLocation("pos");
	mAttrMatrix = mProgram->uniformLocation("matrix");
	mAttrTexture = mProgram->uniformLocation("tex");
	mAttrColor = mProgram->uniformLocation("color");

	createMesh();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	CHECK_GL_ERROR;
}

void View::paintGL()
{
	QPainter painter;
	painter.begin(this);
	painter.beginNativePainting();

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	// Eventually we want to scroll, and this will be how we do that.
	//mModelView.translate(0.0f, 0.0f, 0.0f);

	mProgram->bind();
	if (mStyleData[mStyle].m3D)
	{
		QMatrix4x4 matrix;
		matrix.lookAt(mStyleData[mStyle].mEyePos, mStyleData[mStyle].mLookAt, mStyleData[mStyle].mUp);
		mProgram->setUniformValue(mAttrMatrix, mModelView * matrix);
	}
	else
	{
		mProgram->setUniformValue(mAttrMatrix, mModelView);
	}
	paintTexture();
	mProgram->release();

	painter.endNativePainting();
	painter.end();

	CHECK_GL_ERROR;
}

void View::paintTexture()
{
	StyleData * data = &mStyleData[mStyle];
	if (mStyle == STYLE_STACKED && mTextures.size() > 1)
	{
		static const float top = 0.5f;
		static const float bottom = -0.5f;
		float y = top;
		float deltay = (bottom - top) / (mTextures.size() - 1);
		for (auto & texture : mTextures)
		{
			texture.second->bind();
			mProgram->setUniformValue(mAttrColor, QColor(0xFFFFFFFF));
			mProgram->setUniformValue(mAttrPos, 0.0f, y, 0.0f, 0.0f);

			data->mVao.bind();

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->mIndexBuffer);
			glDrawElements(GL_TRIANGLES, data->mIndexCount, GL_UNSIGNED_SHORT, 0);

			data->mVao.release();

			y += deltay;
		}
	}
	else
	{
		mProgram->setUniformValue(mAttrPos, 0.0f, 0.0f, 0.0f, 0.0f);
		for (auto & texture : mTextures)
		{
			texture.second->bind();
			mProgram->setUniformValue(mAttrColor, QColor(0xFFFFFFFF));
			mProgram->setUniformValue(mAttrPos, 0.0f, 0.0f, 0.0f, 0.0f);

			data->mVao.bind();

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->mIndexBuffer);
			glDrawElements(GL_TRIANGLES, data->mIndexCount, GL_UNSIGNED_SHORT, 0);

			data->mVao.release();
		}
	}
}

void View::createMesh()
{
	createSimpleStyle();
	createTiledStyle();
	createDonutStyle();
	createStackedStyle();
}

void View::createSimpleStyle()
{
	int vertexCount = 4;
	Vertex quad[] =
	{
		{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
		{ 1.0f,  1.0f, 0.0f, 1.0f, 1.0f},
		{-1.0f,  1.0f, 0.0f, 0.0f, 1.0f},
		{ 1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
	};

	StyleData * data = &mStyleData[STYLE_SIMPLE];
	data->m3D = false;
	data->mVao.create();
	data->mVao.bind();

	data->mVertexBuffer.create();
	data->mVertexBuffer.bind();
	data->mVertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	data->mVertexBuffer.allocate(vertexCount * sizeof(Vertex));
	data->mVertexBuffer.write(0, &quad[0], vertexCount * sizeof(Vertex));

	mProgram->enableAttributeArray(mAttrVertex);
	mProgram->enableAttributeArray(mAttrTexCoord);
	mProgram->setAttributeBuffer(mAttrVertex, GL_FLOAT, 0, 3, sizeof(Vertex));
	mProgram->setAttributeBuffer(mAttrTexCoord, GL_FLOAT, 3 * sizeof(GLfloat), 2, sizeof(Vertex));

	data->mVertexBuffer.release();
	data->mVao.release();

	GLushort indices[] = { 0,2,1,3,0,1 };
	data->mIndexCount = sizeof(indices);
	glGenBuffers(1, &data->mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->mIndexCount * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

	CHECK_GL_ERROR;
}

void View::createTiledStyle()
{
	StyleData * data = &mStyleData[STYLE_TILED];
	data->m3D = false;

	const int tiles = 3;
	std::vector<Vertex> vertices;
	int vertexCount = (tiles + 1) * (tiles + 1);
	vertices.reserve(vertexCount);
	double rowPos = -tiles;
	for (int row = 0; row <= tiles; ++row)
	{
		double colPos = -tiles;
		double rowOff = double(row - (tiles / 2));
		for (int col = 0; col <= tiles; ++col)
		{
			Vertex vert;
			vert.x = colPos;
			vert.y = rowPos;
			vert.z = 0.0f;
			vert.u = 0.5f + 0.5 * colPos;
			vert.v = 0.5f + 0.5 * rowPos;
			vertices.push_back(vert);
			colPos += 2.0;
		}
		rowPos += 2.0;
	}

	data->mVao.create();
	data->mVao.bind();

	data->mVertexBuffer.create();
	data->mVertexBuffer.bind();
	data->mVertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	data->mVertexBuffer.allocate(vertices.size() * sizeof(Vertex));
	data->mVertexBuffer.write(0, &vertices[0], vertices.size() * sizeof(Vertex));

	mProgram->enableAttributeArray(mAttrVertex);
	mProgram->enableAttributeArray(mAttrTexCoord);
	mProgram->setAttributeBuffer(mAttrVertex, GL_FLOAT, 0, 3, sizeof(Vertex));
	mProgram->setAttributeBuffer(mAttrTexCoord, GL_FLOAT, 3 * sizeof(GLfloat), 2, sizeof(Vertex));

	data->mVertexBuffer.release();
	data->mVao.release();

	std::vector<GLushort> indices;
	for (int row = 0; row < tiles; ++row)
	{
		for (int col = 0; col < tiles; ++col)
		{
			int idx = row * (tiles + 1) + col;
			indices.push_back(idx);
			indices.push_back(idx + tiles + 1);
			indices.push_back(idx + 1);
			indices.push_back(idx + tiles + 2);
			indices.push_back(idx + 1);
			indices.push_back(idx + tiles + 1);
		}
	}
	data->mIndexCount = indices.size();
	glGenBuffers(1, &data->mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->mIndexCount * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

	CHECK_GL_ERROR;
}

void View::createDonutStyle()
{
	const float majorRadius(1.0);
	const float minorRadius(0.3);
	const int segmentCount(64);
	const int stripCount(32);

	std::vector<Vertex> vertices;
	vertices.reserve((segmentCount + 1) * (stripCount + 1));
	float deltaTheta = PI * 2.0f / segmentCount;
	float deltaPhi = PI * 2.0f / stripCount;
	Mat33f rotateMajor = Mat33f::rotationAroundY(deltaTheta);
	Mat33f rotateMinor = Mat33f::rotationAroundZ(deltaPhi);
	Mat33f orientMajor;
	Vec3f major(majorRadius, 0.0f, 0.0f);
	Vertex vert;
	for (int segment = 0; segment < segmentCount + 1; ++segment)
	{
		Mat33f orientMinor;
		Vec3f minor(minorRadius, 0.0f, 0.0f);
		for (int strip = 0; strip < stripCount + 1; ++strip)
		{
			Vec3f pos = orientMajor * (major + orientMinor * minor);
			vert.x = pos.x;
			vert.y = pos.y;
			vert.z = pos.z;
			vert.u = float(segment) / float(segmentCount);
			vert.v = float(strip) / float(stripCount);
			vertices.push_back(vert);
			orientMinor = rotateMinor * orientMinor;
		}
		orientMajor = rotateMajor * orientMajor;
	}

	auto * data = &mStyleData[STYLE_DONUT];
	data->m3D = true;
	data->mEyePos = QVector3D(0.0f, 0.7f, -1.4f);
	data->mLookAt = QVector3D(0.0f, -0.7f, 0.0f);
	data->mUp = QVector3D(0.0f, 0.707f, 0.707f);
	data->mPov = 90.0f;

	data->mVao.create();
	data->mVao.bind();

	data->mVertexBuffer.create();
	data->mVertexBuffer.bind();
	data->mVertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	data->mVertexBuffer.allocate(vertices.size() * sizeof(Vertex));
	data->mVertexBuffer.write(0, &vertices[0], vertices.size() * sizeof(Vertex));

	mProgram->enableAttributeArray(mAttrVertex);
	mProgram->enableAttributeArray(mAttrTexCoord);
	mProgram->setAttributeBuffer(mAttrVertex, GL_FLOAT, 0, 3, sizeof(Vertex));
	mProgram->setAttributeBuffer(mAttrTexCoord, GL_FLOAT, 3 * sizeof(GLfloat), 2, sizeof(Vertex));

	data->mVertexBuffer.release();
	data->mVao.release();

	std::vector<GLushort> indices;
	for (int segment = 0; segment < segmentCount; ++segment)
	{
		Mat33f orientMinor;
		Vec3f minor(minorRadius, 0.0f, 0.0f);
		for (int strip = 0; strip < stripCount; ++strip)
		{
			int idx = segment * (stripCount + 1) + strip;
			indices.push_back(idx);
			indices.push_back(idx + stripCount + 1);
			indices.push_back(idx + 1);
			indices.push_back(idx + stripCount + 2);
			assert(idx + stripCount + 2 < vertices.size());
			indices.push_back(idx + 1);
			indices.push_back(idx + stripCount + 1);
		}
	}
	assert(indices.size() < pow(2, 16));

	data->mIndexCount = indices.size();
	glGenBuffers(1, &data->mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->mIndexCount * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

	CHECK_GL_ERROR;
}

void View::createStackedStyle()
{
	int vertexCount = 4;
	Vertex quad[] =
	{
		{-1.0f, 0.0f, -1.0f, 0.0f, 0.0f},
		{ 1.0f, 0.0f,  1.0f, 1.0f, 1.0f},
		{-1.0f, 0.0f,  1.0f, 0.0f, 1.0f},
		{ 1.0f, 0.0f, -1.0f, 1.0f, 0.0f}
	};

	StyleData * data = &mStyleData[STYLE_STACKED];
	data->m3D = true;
	data->mEyePos = QVector3D(0.0f, 2.0f, -5.0f);
	data->mLookAt = QVector3D(0.0f, 0.0f, 0.0f);
	data->mUp = QVector3D(0.0f, 0.707f, 0.707f);
	data->mPov = 30.0f;

	data->mVao.create();
	data->mVao.bind();

	data->mVertexBuffer.create();
	data->mVertexBuffer.bind();
	data->mVertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	data->mVertexBuffer.allocate(vertexCount * sizeof(Vertex));
	data->mVertexBuffer.write(0, &quad[0], vertexCount * sizeof(Vertex));

	mProgram->enableAttributeArray(mAttrVertex);
	mProgram->enableAttributeArray(mAttrTexCoord);
	mProgram->setAttributeBuffer(mAttrVertex, GL_FLOAT, 0, 3, sizeof(Vertex));
	mProgram->setAttributeBuffer(mAttrTexCoord, GL_FLOAT, 3 * sizeof(GLfloat), 2, sizeof(Vertex));

	data->mVertexBuffer.release();
	data->mVao.release();

	GLushort indices[] = { 0,2,1,3,0,1 };
	data->mIndexCount = sizeof(indices);
	glGenBuffers(1, &data->mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->mIndexCount * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

	CHECK_GL_ERROR;
}

void View::setZoom(float zoom)
{
	float oldZoom = mZoom;
	mZoom = zoom;
	if (mZoom == 1.0f && mStyle == STYLE_SIMPLE)
	{
		// When we fully zoom out in simple mode, center the network
		mTranslateX = 0.0f;
		mTranslateY = 0.0f;
	}
	else
	{
		// When we change zoom we need to scale the translation also
		mTranslateX *= mZoom / oldZoom;
		mTranslateY *= mZoom / oldZoom;
	}
	setProjection();
	update();
}

void View::checkGlError(char * fun, int line)
{
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError()) != GL_NO_ERROR)
	{
		errString = gluErrorString(errCode);
		LOG("OpenGL error in [" << fun << "] line [" << line << "] : " << errCode << " = " << errString);
	}
}

void View::setStyle(const QString & style)
{
	if (!style.compare("Simple"))
	{
		mStyle = STYLE_SIMPLE;
	}
	else if (!style.compare("Infinite Tiles"))
	{
		mStyle = STYLE_TILED;
	}
	else if (!style.compare("Donut"))
	{
		mStyle = STYLE_DONUT;
	}
	else if (!style.compare("Stacked"))
	{
		mStyle = STYLE_STACKED;
	}
	else
	{
		assert(false);
		return;
	}
	setProjection();
}

void View::automatonSizechanged(int width, int height)
{
	int size = width * height;
	if (mImageData.size() < size)
	{
		mImageData.resize(size);
		LOG("resizing image buffer to [" << mImageData.size() << "]");
	}

	for (auto texture : mTextures)
	{
		createTextureForLayer(texture.first);
	}
}

void View::automatonLayerCreated(std::shared_ptr<Layer> layer)
{
	createTextureForLayer(layer);
}

void View::automatonLayerRemoved(std::shared_ptr<Layer> layer)
{
	mTextures.erase(layer);
}

void View::createTextureForLayer(std::shared_ptr<Layer> layer)
{
	std::shared_ptr<QOpenGLTexture> texture;
	makeCurrent();
	if (mTextures.count(layer))
	{
		texture = mTextures[layer];
		texture->destroy();
	}
	texture = std::make_shared<QOpenGLTexture>(QOpenGLTexture::Target2D);
	texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Nearest);
	texture->create();
	texture->setSize(layer->width(), layer->height(), 1);
	texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
	texture->allocateStorage();
	texture->setWrapMode(QOpenGLTexture::Repeat);
	mTextures[layer] = texture;
	CHECK_GL_ERROR;
}
