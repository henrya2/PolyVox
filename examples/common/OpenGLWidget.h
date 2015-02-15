/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution. 	
*******************************************************************************/

#ifndef __BasicExample_OpenGLWidget_H__
#define __BasicExample_OpenGLWidget_H__

#include "PolyVox/Mesh.h"

#include <QOpenGLFunctions_3_1>

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

// This structure holds all the data required
// to render one of our meshes through OpenGL. 
struct OpenGLMeshData
{
	GLuint noOfIndices;
	GLenum indexType;
	GLuint indexBuffer;
	GLuint vertexBuffer;
	GLuint vertexArrayObject;
	QVector3D translation;
	float scale;
};

// Our OpenGLWidget is used by all the examples to render the extracted meshes. It is
// fairly specific to our needs (you probably won't want to use it in your own project)
// but should provide a useful illustration of how PolyVox meshes can be rendered.
class OpenGLWidget : public QGLWidget, protected QOpenGLFunctions_3_1
{
public:
	// Constructor
	OpenGLWidget(QWidget *parent);

	// Convert a PolyVox mesh to OpenGL index/vertex buffers. Inlined because it's templatised.
	template <typename MeshType>
	void addMesh(const MeshType& surfaceMesh, const PolyVox::Vector3DInt32& translation = PolyVox::Vector3DInt32(0, 0, 0), float scale = 1.0f)
	{
		// Convienient access to the vertices and indices
		const auto& vecIndices = surfaceMesh.getIndices();
		const auto& vecVertices = surfaceMesh.getVertices();

		// This struct holds the OpenGL properties (buffer handles, etc) which will be used
		// to render our mesh. We copy the data from the PolyVox mesh into this structure.
		OpenGLMeshData meshData;

		// Create the VAO for the mesh
		glGenVertexArrays(1, &(meshData.vertexArrayObject));
		glBindVertexArray(meshData.vertexArrayObject);

		// The GL_ARRAY_BUFFER will contain the list of vertex positions
		glGenBuffers(1, &(meshData.vertexBuffer));
		glBindBuffer(GL_ARRAY_BUFFER, meshData.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(typename MeshType::VertexType), vecVertices.data(), GL_STATIC_DRAW);

		// and GL_ELEMENT_ARRAY_BUFFER will contain the indices
		glGenBuffers(1, &(meshData.indexBuffer));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(typename MeshType::IndexType), vecIndices.data(), GL_STATIC_DRAW);

		// Every surface extractor outputs valid positions for the vertices, so tell OpenGL how these are laid out
		glEnableVertexAttribArray(0); // Attrib '0' is the vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, position))); //take the first 3 floats from every sizeof(decltype(vecVertices)::value_type)

		// Some surface extractors also generate normals, so tell OpenGL how these are laid out. If a surface extractor
		// does not generate normals then nonsense values are written into the buffer here and sghould be ignored by the
		// shader. This is mostly just to simplify this example code - in a real application you will know whether your
		// chosen surface extractor generates normals and can skip uploading them if not.
		glEnableVertexAttribArray(1); // Attrib '1' is the vertex normals.
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, normal)));

		// Finally a surface extractor will probably output additional data. This is highly application dependant. For this example code 
		// we're just uploading it as a set of bytes which we can read individually, but real code will want to do something specialised here.
		glEnableVertexAttribArray(2); //We're talking about shader attribute '2'
		GLint size = (std::min)(sizeof(typename MeshType::VertexType::DataType), size_t(4)); // Can't upload more that 4 components (vec4 is GLSL's biggest type)
		glVertexAttribIPointer(2, size, GL_UNSIGNED_BYTE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, data)));

		// We're done uploading and can now unbind.
		glBindVertexArray(0);

		// A few additional properties can be copied across for use during rendering.
		meshData.noOfIndices = vecIndices.size();
		meshData.translation = QVector3D(translation.getX(), translation.getY(), translation.getZ());
		meshData.scale = scale;

		// Set 16 or 32-bit index buffer size.
		meshData.indexType = sizeof(typename MeshType::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

		// Now add the mesh to the list of meshes to render.
		addMeshData(meshData);
	}

	void addMeshData(OpenGLMeshData meshData)
	{
		mMeshData.push_back(meshData);
	}

	// For our purposes we use a single shader for the whole volume, and
	// this example framework is only meant to show a single volume at a time
	void setShader(QSharedPointer<QGLShaderProgram> shader);

	// The viewable region can be adjusted so that this example framework can be used for different volume sizes.
	void setViewableRegion(PolyVox::Region viewableRegion);

	// Mouse handling
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);

protected:

	// Qt OpenGL functions
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	virtual void initialize()
	{
	}

private:

	void setupWorldToCameraMatrix();

	// Index/vertex buffer data
	std::vector<OpenGLMeshData> mMeshData;
	
	QSharedPointer<QGLShaderProgram> mShader;

	// Matrices
	QMatrix4x4 worldToCameraMatrix;
	QMatrix4x4 cameraToClipMatrix;

	// Mouse data
	QPoint m_LastFrameMousePos;
	QPoint m_CurrentMousePos;

	// Camera setup
	PolyVox::Region m_viewableRegion;
	int m_xRotation;
	int m_yRotation;
};

#endif //__BasicExample_OpenGLWidget_H__
