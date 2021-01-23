#include "View.h"

#include <QPainter>
#include <QPaintEngine>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <qtimer.h>

#include "NetControl.h"
#include "NeuronSim/Constants.h"

View::View(QWidget *parent)
	: QOpenGLWidget(parent),
	mProgram(0)
{
	ui.setupUi(this);
}

View::~View()
{
	makeCurrent();
	delete mProgram;
	delete mVertexShader;
	delete mFragmentShader;
	mVertexBuffer.destroy();
	doneCurrent();
}

void View::resizeTexture(int width, int height)
{	
	mTexture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
	mTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
	mTexture->create();

	mTexture->setSize(width, height, 1);
	mTexture->setFormat(QOpenGLTexture::RGBA8_UNorm);
	mTexture->allocateStorage();

	fixQuadAspectRatio(width, height);
}

void View::updateTexture(uint32_t * data)
{
	mTexture->bind();
	mTexture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, data);
}

void View::resizeGL(int width, int height)
{
	float cellWidth = 512.0f;
	float cellHeight = 512.0f;
	if (mTexture)
	{
		cellWidth = mTexture->width();
		cellHeight = mTexture->height();
	}
	float cellAspect = cellWidth / cellHeight;
	float windowAspect = float(width) / float(height);

	float left = -1.0f;
	float right = 1.0f;
	float top = -1.0f;
	float bottom = 1.0f;
	if (windowAspect > cellAspect)
	{
		left = -windowAspect;
		right = windowAspect;
	}
	else
	{
		float aspect = 1.0f / windowAspect;
		top = -aspect;
		bottom = aspect;
	}

	mModelView.setToIdentity();
	mModelView.ortho(left, right, bottom, top, -1.0f, 1.0f);
	mModelView.scale(0.9f);
}

void View::initializeGL()
{
	initializeOpenGLFunctions();

	mVertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
	const char *vertexSource =
		"attribute highp vec4 vertex;\n"
		"attribute highp vec4 texCoord;\n"
		"uniform mediump mat4 matrix;\n"
		"varying highp vec4 texc;\n"
		"void main(void)\n"
		"{\n"
		"    gl_Position = matrix * vertex;\n"
		"    texc = texCoord;\n"
		"}\n";
	mVertexShader->compileSourceCode(vertexSource);

	mFragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
	const char *fragmentSource =
		"varying highp vec4 texc;\n"
		"uniform sampler2D tex;\n"
		"void main(void)\n"
		"{\n"
		"    gl_FragColor = vec4(texture2D(tex, texc.st).rgba);\n"
		"}\n";
	mFragmentShader->compileSourceCode(fragmentSource);

	mProgram = new QOpenGLShaderProgram;
	mProgram->addShader(mVertexShader);
	mProgram->addShader(mFragmentShader);
	mProgram->link();

	mAttrVertex = mProgram->attributeLocation("vertex");
	mAttrTexCoord = mProgram->attributeLocation("texCoord");
	mAttrMatrix = mProgram->uniformLocation("matrix");
	mAttrTexture = mProgram->uniformLocation("tex");

	createQuad();

	glClearColor(0x00, 0x00, 0x00, 0xFF);
}

void View::paintGL()
{
	QPainter painter;
	painter.begin(this);
	painter.beginNativePainting();

	glClear(GL_COLOR_BUFFER_BIT);

	// Eventually we want to zoom in and scroll, and this will be how we do that.
	//mModelView.scale(mScale);
	//mModelView.translate(0.0f, 0.0f, 0.0f);

	mProgram->bind();
	mProgram->setUniformValue(mAttrMatrix, mModelView);
	paintTexturedCube();
	mProgram->release();

	painter.endNativePainting();
	painter.end();

	// We're using a timer at the moment but we should change this to be an option
	// It will just block on the swap and go as fast as possible
	if (false)
		update();
}

void View::createQuad()
{
	mVertices =
	{
		{-0.5f, -0.5f, 0.0f, 0.0f, 0.0f},
		{ 0.5f,  0.5f, 0.0f, 1.0f, 1.0f},
		{-0.5f,  0.5f, 0.0f, 0.0f, 1.0f},
		{-0.5f, -0.5f, 0.0f, 0.0f, 0.0f},
		{ 0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
		{ 0.5f,  0.5f, 0.0f, 1.0f, 1.0f}
	};

	mVertexBuffer.create();
	mVertexBuffer.bind();
	mVertexBuffer.allocate(mVertices.size() * sizeof(Vertex));
	mVertexBuffer.write(0, &mVertices[0], mVertices.size() * sizeof(Vertex));
	mVertexBuffer.release();
}

void View::fixQuadAspectRatio(int width, int height)
{
	mVertexBuffer.bind();
	GLfloat* vertices = static_cast<GLfloat*>(mVertexBuffer.map(QOpenGLBuffer::WriteOnly));
	assert(vertices);
	if (vertices)
	{
		float left = -1.0;
		float right = 1.0;
		float top = -1.0;
		float bottom = 1.0;
		if (width > height)
		{
			float aspect = float(height) / float(width);
			top = -aspect;
			bottom = aspect;
		}
		else
		{
			float aspect = float(width) / float(height);
			left = -aspect;
			right = aspect;
		}
		mVertices[0].x = left;
		mVertices[0].y = top;
		mVertices[1].x = right;
		mVertices[1].y = bottom;
		mVertices[2].x = left;
		mVertices[2].y = bottom;
		mVertices[3].x = left;
		mVertices[3].y = top;
		mVertices[4].x = right;
		mVertices[4].y = top;
		mVertices[5].x = right;
		mVertices[5].y = bottom;
		memcpy(vertices, &mVertices[0], mVertices.size() * sizeof(Vertex));
		mVertexBuffer.unmap();
	}
	mVertexBuffer.release();
}

void View::paintTexturedCube()
{
	if (mTexture)
	{
		mTexture->bind();

		mProgram->setUniformValue(mAttrTexture, 0);

		mProgram->enableAttributeArray(mAttrVertex);
		mProgram->enableAttributeArray(mAttrTexCoord);

		mVertexBuffer.bind();
		mProgram->setAttributeBuffer(mAttrVertex, GL_FLOAT, 0, 3, sizeof(Vertex));
		mProgram->setAttributeBuffer(mAttrTexCoord, GL_FLOAT, 3 * sizeof(GLfloat), 2, sizeof(Vertex));
		mVertexBuffer.release();

		glDrawArrays(GL_TRIANGLES, 0, 6);

		mProgram->disableAttributeArray(mAttrVertex);
		mProgram->disableAttributeArray(mAttrTexCoord);
	}
}

