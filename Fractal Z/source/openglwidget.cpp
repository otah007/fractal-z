#include "../header/openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget * parent)
{
}

void OpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

	// Create block vertices
	{
		int a = BLOCKS_TOTAL_VERTICAL / 2;
		int b = (BLOCKS_TOTAL_VERTICAL + 1) - a;
		int m = BLOCKS_TOTAL_HORIZONTAL / 2;
		int n = (BLOCKS_TOTAL_HORIZONTAL + 1) - m;

		//std::cout << a << ", " << b << ", " << m << ", " << n << std::endl;

		// Tesselate render area with triangles
		for (int i = -a; i < b; ++i)
		{
			for (int j = -m; j < n; ++j)
			{
				float x = float(j) / (BLOCKS_HORIZONTAL / 2);
				float y = float(i) / (BLOCKS_VERTICAL / 2);
				vertices.push_back(x);		// X co-ord
				vertices.push_back(y);		// Y co-ord
				vertices.push_back(0.0f);	// Z co-ord (0 because on a flat plane)
			}
		}
	}

	// Create block indices (works with vertex co-ords for more efficient rendering)
	{
		int a = BLOCKS_TOTAL_HORIZONTAL + 1;

		// Loop through each triangle pair (square) getting their vertices in order
		for (int i = 0; i < BLOCKS_TOTAL_VERTICAL; ++i)
		{
			for (int j = 0; j < BLOCKS_TOTAL_HORIZONTAL; ++j)
			{
				int index = ((i * a) + j) * 6;
				indices.push_back((i * a) + j);			// Top left
				indices.push_back((i * a) + 1 + j);		// Top right
				indices.push_back((i * a) + 1 + a + j);	// Bottom right
				indices.push_back((i * a) + j);			// Top left
				indices.push_back((i * a) + a + j);		// Bottom left
				indices.push_back((i * a) + a + 1 + j);	// Bottom right
			}
		}
	}

	m_vao.create();
	m_vao.bind();

	QOpenGLBuffer m_vvbo(QOpenGLBuffer::VertexBuffer);
	m_vvbo.create();
	m_vvbo.bind();
	m_vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_vvbo.allocate(vertices.data(), vertices.size() * sizeof(float));

	QOpenGLBuffer m_ebo(QOpenGLBuffer::IndexBuffer);
	m_ebo.create();
	m_ebo.bind();
	m_ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_ebo.allocate(indices.data(), indices.size() * sizeof(int));

	m_vao.release();

	m_vvbo.release();
	m_ebo.release();

	// Create image (for load/store)
	float* data = new float[IMAGE_WIDTH * IMAGE_HEIGHT];
	for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; ++i)
		data[i] = float(i) / float(IMAGE_WIDTH * IMAGE_HEIGHT);

	GLuint tex[1];
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, IMAGE_WIDTH, IMAGE_HEIGHT);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_RED, GL_FLOAT, data);

	delete[] data;

	// Fill mode for polygons
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Shaders
	computeProgram = new QOpenGLShaderProgram();
	computeProgram->addShaderFromSourceFile(QOpenGLShader::Compute, "shaders/test/test_compute.glsl");
	computeProgram->link();

	renderProgram = new QOpenGLShaderProgram();
	renderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/test/test_vertex.glsl");
	renderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/test/test_fragment.glsl");
	renderProgram->link();

	computeProgram->bind();
	glBindImageTexture(0, tex[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	computeProgram->release();

	renderProgram->bind();
	m_vao.bind();
	m_vvbo.bind();
	renderProgram->enableAttributeArray("position");
	renderProgram->setAttributeBuffer("position", GL_FLOAT, 0, 3);
	m_vvbo.release();
	m_vao.release();
	glBindImageTexture(0, tex[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	renderProgram->release();

	updateTimer = new QTimer(this);
	updateTimer->setInterval(1000.f / 30.f);
	connect(updateTimer, SIGNAL(timeout()), this, SLOT(drawFrame()));
	updateTimer->start();

	runCompute();
	update();
}

void OpenGLWidget::paintGL()
{
	const qreal retinaScale = devicePixelRatio();
	glClear(GL_COLOR_BUFFER_BIT);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	renderProgram->bind();

	renderProgram->setUniformValue("WINDOW_WIDTH", WINDOW_WIDTH);
	renderProgram->setUniformValue("WINDOW_HEIGHT", WINDOW_HEIGHT);
	renderProgram->setUniformValue("BLOCKS_EXTRA_HORIZONTAL", BLOCKS_EXTRA_HORIZONTAL);
	renderProgram->setUniformValue("BLOCKS_EXTRA_VERTICAL", BLOCKS_EXTRA_VERTICAL);

	m_vao.bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	m_vao.release();

	renderProgram->release();
}

void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
}

void OpenGLWidget::drawFrame()
{
	update();
}

void OpenGLWidget::runCompute()
{
	computeProgram->bind();

	computeProgram->setUniformValue("WINDOW_WIDTH", WINDOW_WIDTH);
	computeProgram->setUniformValue("WINDOW_HEIGHT", WINDOW_HEIGHT);

	glDispatchCompute(BLOCKS_TOTAL_HORIZONTAL, BLOCKS_TOTAL_VERTICAL, 1);

	computeProgram->release();
}