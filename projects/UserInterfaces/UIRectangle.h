#ifndef __UIRECTANGLE_H_
#define __UIRECTANGLE_H_

#include "UIElement.h"

using glm::vec4; 

class UIRectangle : public UIElement
{
public: 

	UIRectangle(unsigned int a_shader,
		const vec2& a_dimensions = vec2(1, 1),
		const vec4& a_colour = vec4(1)); 
	virtual ~UIRectangle(); 

	virtual void	draw(const glm::mat4& a_projection);

protected:

	unsigned int m_shader; 
	unsigned int m_vao, m_vbo;

	vec2		m_dimensions; 
	vec4		m_colour; 
};




#endif //_UIRECTANGLE_H_