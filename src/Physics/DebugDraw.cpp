#include "Physics/DebugDraw.hpp"
#include "box2d/box2d.h"
#include "Physics/WorldHandler.hpp"
#include "Graphics/WindowHandler.hpp"

DebugDraw::DebugDraw()
{
    this->drawAll(false);
    m_drawStruct.useDrawingBounds = false;
    m_drawStruct.context = this;
    m_drawStruct.DrawPolygon = DebugDraw::DrawPolygon;
    m_drawStruct.DrawSolidPolygon = DebugDraw::DrawSolidPolygon;
    m_drawStruct.DrawCircle = DebugDraw::DrawCircle;
    m_drawStruct.DrawSolidCircle = DebugDraw::DrawSolidCircle;
    m_drawStruct.DrawCapsule = DebugDraw::DrawCapsule;
    m_drawStruct.DrawSolidCapsule = DebugDraw::DrawSolidCapsule;
    m_drawStruct.DrawSegment = DebugDraw::DrawSegment;
    m_drawStruct.DrawTransform = DebugDraw::DrawTransform;
    m_drawStruct.DrawPoint = DebugDraw::DrawPoint;
    m_drawStruct.DrawString = DebugDraw::DrawString;
}

DebugDraw& DebugDraw::get()
{
    static DebugDraw singleton;
    return singleton;
}

void DebugDraw::draw()
{
    b2World_Draw(WorldHandler::get().getWorld(), &m_drawStruct);
}

sf::Color B2SFColor(const b2HexColor &color, int alpha = 255)
{
	sf::Color result((sf::Uint8)(((color >> 16) & 0xFF) / 255.0), (sf::Uint8)(((color >> 8) & 0xFF) / 255.0), (sf::Uint8)(((color) & 0xFF) / 255.0), (sf::Uint8) alpha);
	return result;
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor color, void *context)
{
    sf::ConvexShape polygon(vertexCount);
	for (int32_t i=0; i<vertexCount; i++)
	{
		b2Vec2 vertex = vertices[i];
		polygon.setPoint(i, sf::Vector2f(vertex.x*PIXELS_PER_METER, vertex.y*PIXELS_PER_METER));
	}
    polygon.setFillColor(sf::Color::Transparent);
    polygon.setOutlineColor(B2SFColor(color));
	polygon.setOutlineThickness(1.0f);
	WindowHandler::getRenderWindow()->draw(polygon);
}

void DebugDraw::DrawSolidPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void *context)
{
    sf::Color fillColor(B2SFColor(color));
    fillColor.r *= 0.5f;
    fillColor.g *= 0.5f;
    fillColor.b *= 0.5f;
    fillColor.a = 0.5f;

	sf::ConvexShape polygon(vertexCount);
	for (int32_t i=0; i<vertexCount; i++)
	{
		b2Vec2 vertex = vertices[i];
		polygon.setPoint(i, sf::Vector2f(vertex.x*PIXELS_PER_METER, vertex.y*PIXELS_PER_METER));
	}
    polygon.setFillColor(fillColor);
    polygon.setOutlineColor(B2SFColor(color));
	polygon.setOutlineThickness(1.0f);
	WindowHandler::getRenderWindow()->draw(polygon);
}

void DebugDraw::DrawCircle(b2Vec2 center, float radius, b2HexColor color, void *context)
{
    sf::CircleShape circle((radius*PIXELS_PER_METER));
    circle.setPosition(center.x*PIXELS_PER_METER-radius*PIXELS_PER_METER, center.y*PIXELS_PER_METER-radius*PIXELS_PER_METER);
    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineColor(B2SFColor(color));
    circle.setOutlineThickness(1.f);
    WindowHandler::getRenderWindow()->draw(circle);
}

void DebugDraw::DrawSolidCircle(b2Transform transform, float radius, b2HexColor color, void *context)
{
    sf::Color fillColor(B2SFColor(color));
    fillColor.r *= 0.5f;
    fillColor.g *= 0.5f;
    fillColor.b *= 0.5f;
    fillColor.a = 0.5f;

	sf::CircleShape circle((radius*PIXELS_PER_METER));
    circle.setPosition(transform.p.x*PIXELS_PER_METER-radius*PIXELS_PER_METER, transform.p.y*PIXELS_PER_METER-radius*PIXELS_PER_METER);
    circle.setFillColor(fillColor);
    circle.setOutlineColor(B2SFColor(color));
    circle.setOutlineThickness(1.f);
    WindowHandler::getRenderWindow()->draw(circle);
    DrawTransform(transform, context); // so that it has the direction lines
}

void DebugDraw::DrawCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void *context) // TODO test if this works
{
    const int POINT_COUNT = 15; // point count per curved part

    sf::VertexArray capsule;
    capsule.setPrimitiveType(sf::PrimitiveType::LineStrip);
    
    for (int i = 0; i < POINT_COUNT; i++)
        capsule.append(sf::Vertex{(sf::Vector2f)(Vector2::rotate(Vector2{-1,0}, i*180.0/POINT_COUNT) * radius + p1), B2SFColor(color)});

    for (int i = 0; i < POINT_COUNT; i++)
        capsule.append(sf::Vertex{(sf::Vector2f)(Vector2::rotate(Vector2{1,0}, i*180.0/POINT_COUNT) * radius + p2), B2SFColor(color)});
    
    capsule.append(sf::Vertex{(sf::Vector2f)(Vector2{1,0} * radius + p1), B2SFColor(color)}); // adding the last point

    WindowHandler::getRenderWindow()->draw(capsule);
}

void DebugDraw::DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void *context) // TODO test if this works
{
    const int POINT_COUNT = 15; // point count per curved part

    sf::VertexArray capsule;
    capsule.setPrimitiveType(sf::PrimitiveType::TriangleFan);
    
    capsule.append(sf::Vertex{sf::Vector2f{p1.x, p1.y}, B2SFColor(color)}); // adding the center point

    for (int i = 0; i < POINT_COUNT; i++)
        capsule.append(sf::Vertex{(sf::Vector2f)(Vector2::rotate(Vector2{-1,0}, i*180.0/POINT_COUNT) * radius + p1), B2SFColor(color)});

    for (int i = 0; i < POINT_COUNT; i++)
        capsule.append(sf::Vertex{(sf::Vector2f)(Vector2::rotate(Vector2{1,0}, i*180.0/POINT_COUNT) * radius + p2), B2SFColor(color)});
    
    capsule.append(sf::Vertex{(sf::Vector2f)(Vector2{1,0} * radius + p1), B2SFColor(color)}); // adding the last point

    WindowHandler::getRenderWindow()->draw(capsule);
    DrawCapsule(p1, p2, radius, color, context); // drawing the outline

    b2Transform transform;
    transform.p = p1 - p2;
    transform.q = (b2Rot)Vector2::angle(p1, p2);
    DrawTransform(transform, context); // so that it has the direction lines
}

void DebugDraw::DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context)
{
    sf::VertexArray line;
    line.append(sf::Vertex(sf::Vector2f(p1.x*PIXELS_PER_METER, p1.y*PIXELS_PER_METER), B2SFColor(color)));
    line.append(sf::Vertex(sf::Vector2f(p2.x*PIXELS_PER_METER, p2.y*PIXELS_PER_METER), B2SFColor(color)));
    line.setPrimitiveType(sf::Lines);
    
    WindowHandler::getRenderWindow()->draw(line);
}

void DebugDraw::DrawTransform(b2Transform transform, void *context)
{
    float lineProportion = 0.4f;
    b2Vec2 p1 = transform.p, p2;

	//red (X axis)
	p2 = p1 + (lineProportion * b2Rot_GetXAxis(transform.q));
    DrawSegment(p1, p2, b2HexColor::b2_colorRed, context);

	//green (Y axis)
	p2 = p1 + (lineProportion * b2Rot_GetYAxis(transform.q));
    DrawSegment(p1, p2, b2HexColor::b2_colorGreen, context);
}

void DebugDraw::DrawPoint(b2Vec2 p, float size, b2HexColor color, void *context)
{
    sf::RectangleShape temp({size,size});
    temp.setOrigin(size/2,size/2);
    temp.setPosition(p.x,p.y);
    temp.setFillColor(B2SFColor(color));
	WindowHandler::getRenderWindow()->draw(temp);
}

void DebugDraw::DrawString(b2Vec2 p, const char *s, void *context)
{
    return;
}
