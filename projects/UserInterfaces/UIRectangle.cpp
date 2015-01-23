#include "UIRectangle.h"
#include <GL/glew.h>
#include <glm/ext.hpp>

UIRectangle::UIRectangle(unsigned int a_shader,
						const vec2& a_dimensions,
						const vec4& a_colour)
						: m_shader(a_shader),
						m_vao(0), m_vbo(0),
						m_dimensions(a_dimensions),
						m_colour(a_colour)
{
	glm::vec3 vertices[4] = {
		glm::vec3(-0.5f, 0.5f, 1), glm::vec3(-.5f, -.5f, 1),
		glm::vec3(.5f, .5f, 1), glm::vec3(.5f, -.5f, 1)
	};

	glGenVertexArrays(1, &m_vao); 
	glBindVertexArray(m_vao); 

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glBindVertexArray(0);
}

UIRectangle::~UIRectangle()
{
	glDeleteVertexArrays(1, &m_vao); 
	glDeleteBuffers(1, &m_vbo);
}

void UIRectangle::draw(const glm::mat4& a_projection)
{
	int program = -1; 
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);

	if (program != m_shader)
		glUseProgram(m_shader); 

	int location = glGetUniformLocation(m_shader, "projection");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(a_projection));

	mat3 transform = getTransform();
	location = glGetUniformLocation(m_shader, "transform");
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(transform));

	location = glGetUniformLocation(m_shader, "dimensions");
	glUniform2f(location, m_dimensions.x, m_dimensions.y);

	location = glGetUniformLocation(m_shader, "colour");
	glUniform4fv(location, 1, glm::value_ptr(m_colour));

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	UIElement::draw(a_projection);

	if (program != m_shader)
		glUseProgram(program); 
}