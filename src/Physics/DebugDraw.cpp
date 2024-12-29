#include "Physics/DebugDraw.hpp"

DebugDraw::DebugDraw(sf::RenderWindow* window)
{
    m_renderWindow = window;
}

void DebugDraw::setRenderWindow(sf::RenderWindow* window)
{
    m_renderWindow = window;
}

sf::RenderWindow* DebugDraw::getRenderWindow() const
{
    return m_renderWindow;
}

//convert a Box2D (float 0.0f - 1.0f range) color to a SFML color (uint8 0 - 255 range)
sf::Color B2SFColor(const b2Color &color, int alpha = 255)
{
	sf::Color result((std::uint8_t)(color.r*255), (std::uint8_t)(color.g*255), (std::uint8_t)(color.b*255), (std::uint8_t) alpha);
	return result;
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	sf::ConvexShape polygon(vertexCount);
	for (int32 i=0; i<vertexCount; i++)
	{
		b2Vec2 vertex = vertices[i];
		polygon.setPoint(i, sf::Vector2f(vertex.x*PIXELS_PER_METER, vertex.y*PIXELS_PER_METER));
	}
    polygon.setFillColor(sf::Color::Transparent);
    polygon.setOutlineColor(B2SFColor(color));
	polygon.setOutlineThickness(1.0f);
	m_renderWindow->draw(polygon);
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);

	sf::ConvexShape polygon(vertexCount);
	for (int32 i=0; i<vertexCount; i++)
	{
		b2Vec2 vertex = vertices[i];
		polygon.setPoint(i, sf::Vector2f(vertex.x*PIXELS_PER_METER, vertex.y*PIXELS_PER_METER));
	}
    polygon.setFillColor(B2SFColor(fillColor));
    polygon.setOutlineColor(B2SFColor(color));
	polygon.setOutlineThickness(1.0f);
	m_renderWindow->draw(polygon);
}

void DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
	sf::CircleShape circle((radius*PIXELS_PER_METER));
    circle.setPosition({center.x*PIXELS_PER_METER-radius*PIXELS_PER_METER, center.y*PIXELS_PER_METER-radius*PIXELS_PER_METER});
    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineColor(B2SFColor(color));
    circle.setOutlineThickness(1.f);
    m_renderWindow->draw(circle);
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
	b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
	sf::CircleShape circle((radius*PIXELS_PER_METER));
    circle.setPosition({center.x*PIXELS_PER_METER-radius*PIXELS_PER_METER, center.y*PIXELS_PER_METER-radius*PIXELS_PER_METER});
    circle.setFillColor(B2SFColor(fillColor));
    circle.setOutlineColor(B2SFColor(color));
    circle.setOutlineThickness(1.f);
    m_renderWindow->draw(circle);
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	sf::VertexArray line;
    line.append(sf::Vertex(sf::Vector2f(p1.x*PIXELS_PER_METER, p1.y*PIXELS_PER_METER), B2SFColor(color)));
    line.append(sf::Vertex(sf::Vector2f(p2.x*PIXELS_PER_METER, p2.y*PIXELS_PER_METER), B2SFColor(color)));
    line.setPrimitiveType(sf::PrimitiveType::Lines);
    
    m_renderWindow->draw(line);
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
	float lineProportion = 0.4f;
    b2Vec2 p1 = xf.p, p2;

	//red (X axis)
	p2 = p1 + (lineProportion * xf.q.GetXAxis());
    this->DrawSegment(p1, p2, b2Color(255, 0, 0));

	//green (Y axis)
	p2 = p1 + (lineProportion * xf.q.GetYAxis());
    this->DrawSegment(p1, p2, b2Color(0, 255, 0));
}

void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
    sf::RectangleShape temp({size,size});
    temp.setOrigin({size/2,size/2});
    temp.setPosition({p.x,p.y});
    temp.setFillColor(B2SFColor(color));
	m_renderWindow->draw(temp);
}

void DebugDraw::DrawString(int x, int y, const char* string, ...)
{
	return;
}

void DebugDraw::DrawString(const b2Vec2& pw, const char* string, ...)
{
	return;
}

void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	sf::ConvexShape polygon(4);

    polygon.setPoint(0, sf::Vector2f(aabb->lowerBound.x*PIXELS_PER_METER, aabb->lowerBound.y*PIXELS_PER_METER));
    polygon.setPoint(1, sf::Vector2f(aabb->upperBound.x*PIXELS_PER_METER, aabb->lowerBound.y*PIXELS_PER_METER));
    polygon.setPoint(2, sf::Vector2f(aabb->upperBound.x*PIXELS_PER_METER, aabb->upperBound.y*PIXELS_PER_METER));
    polygon.setPoint(3, sf::Vector2f(aabb->lowerBound.x*PIXELS_PER_METER, aabb->upperBound.y*PIXELS_PER_METER));

    polygon.setFillColor(sf::Color::Transparent);
    polygon.setOutlineColor(B2SFColor(c));
    polygon.setOutlineThickness(1.0f);
	m_renderWindow->draw(polygon);
}
