#include "Buffer.h"

using namespace Renderer;

Buffer::Buffer(void)
{
	m_Type = None;
	m_Count = 0;
}

Buffer::~Buffer(void)
{
	// Clean up buffers
	if (m_Type != None)
	{
		glDeleteBuffers(m_Buffers.size(), &m_Buffers[0]);
	}
}

bool Buffer::init(const BufferData* p_BufferData, GLsizei p_BufferDataSize, GLuint program)
{
	return init(p_BufferData, p_BufferDataSize, NULL, 0, program);
}

bool Buffer::init(const BufferData* p_BufferData, GLsizei p_BufferDataSize,
	const GLuint* p_Indices, GLsizei p_IndexDataSize, GLuint program)
{
	// Make sure there's no weird behaviour
	if (m_Type != None || p_BufferData == NULL || p_BufferDataSize == 0)
		return false;

	// Define what type of buffer that will be used
	m_Type = (p_Indices == NULL || p_IndexDataSize == 0) ? VertexBased : IndexBased;

	// Create buffers
	const GLsizei bufferSize = VertexBased ? p_BufferDataSize : p_BufferDataSize + 1;
	m_Buffers.resize(bufferSize);
	glGenBuffers(bufferSize, &m_Buffers[0]);

	// Initialize every vertex buffer
//-- Ta hand om alla buffrar h�r-----------------------
	glBindAttribLocation(program, 0, "VertexPosition");
	glBindAttribLocation(program, 1, "Pad1");
	glBindAttribLocation(program, 2, "VertexNormal");
	glBindAttribLocation(program, 3, "Pad2");
	//glBindAttribLocation(program, 2, "VertexTangent");
	//glBindAttribLocation(program, 3, "VertexBiTangent");
	//glBindAttribLocation(program, 2, "VertexTexCoord");


	for (int i = 0; i < p_BufferDataSize; i++)
	{
		// Allocate and buffer data
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i]);
		glBufferData(GL_ARRAY_BUFFER, p_BufferData[i].dataSize, p_BufferData[i].data, GL_STATIC_DRAW);
	}

	/*for (int i = 0; i < p_BufferDataSize; i++)
	{
		glVertexAttribPointer(i, p_BufferData[i].componentCount, GL_FLOAT, GL_FALSE, 0, p_BufferData[i].data);
		glEnableVertexAttribArray(i);
	}*/

//--------------------------------------------------------

	// Initialize index buffer if specified
	if (m_Type == IndexBased)
	{
		// Allocate and buffer data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers.back());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_IndexDataSize, p_Indices, GL_STATIC_DRAW);
	}

	for (unsigned int i = 0; i < p_BufferDataSize; i++)
		m_BufferData.push_back(p_BufferData[i]);

	return true;
}

void Buffer::draw(void)
{
	if (m_Count > 0)
	{
		draw(0, m_Count);
	}
}

void Buffer::draw(GLint base, GLsizei count)
{
	// Make sure there's no weird behaviour
	if (m_Type == None)
		return;

	// Initialize every vertex buffer
	for (int i = 0; i < (int)m_Buffers.size(); i++)
	{
		// Allocate and buffer data
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i]);
		// Define attribute data (for shaders)
		glVertexAttribPointer(m_BufferData[i].location, m_BufferData[i].componentCount, m_BufferData[i].type, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(m_BufferData[i].location);
	}

	// Draw based on based buffer type
	switch (m_Type)
	{
	case VertexBased:
		glDrawArrays(GL_TRIANGLES, base, count);
		break;
	case IndexBased:
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(sizeof(GLuint)* base));
		break;
	}
}

void Buffer::setCount(GLsizei count)
{
	m_Count = count;
}