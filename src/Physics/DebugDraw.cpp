#include "Physics/DebugDraw.hpp"
#include "Graphics/CameraManager.hpp"
#include "box2d/box2d.h"
#include "Vector2.hpp"

float DebugDraw::m_lineThickness = 1.f;

DebugDraw::DebugDraw()
{
    this->drawAll(false);
    m_drawStruct.useDrawingBounds = false;
    m_drawStruct.context = this;
    m_drawStruct.DrawPolygonFcn = DebugDraw::DrawPolygon;
    m_drawStruct.DrawSolidPolygonFcn = DebugDraw::DrawSolidPolygon;
    m_drawStruct.DrawCircleFcn = DebugDraw::DrawCircle;
    m_drawStruct.DrawSolidCircleFcn = DebugDraw::DrawSolidCircle;
    // m_drawStruct.DrawCapsule = DebugDraw::DrawCapsule;
    m_drawStruct.DrawSolidCapsuleFcn = DebugDraw::DrawSolidCapsule;
    m_drawStruct.DrawSegmentFcn = DebugDraw::DrawSegment;
    m_drawStruct.DrawTransformFcn = DebugDraw::DrawTransform;
    m_drawStruct.DrawPointFcn = DebugDraw::DrawPoint;
    m_drawStruct.DrawStringFcn = DebugDraw::DrawString;
}

DebugDraw& DebugDraw::get()
{
    static DebugDraw singleton;
    return singleton;
}

void DebugDraw::draw(WorldHandler& world, sf::RenderTarget* target)
{
    assert(target != nullptr && "Render target must be valid to draw!");
    m_renderTarget = target;

    Vector2 center = CameraManager::getMainCamera()->getCameraView().getCenter();
    Vector2 size = CameraManager::getMainCamera()->getCameraView().getSize();

    this->setDrawingBounds((center - size/2) / PIXELS_PER_METER, (center + size/2) / PIXELS_PER_METER);
    b2World_Draw(world.getWorld(), &m_drawStruct);
}

sf::Color B2SFColor(const b2HexColor &color, int alpha = 255)
{
	sf::Color result;
    result.r = (std::uint8_t)(((color >> 16) & 0xff));
    result.g = (std::uint8_t)(((color >> 8) & 0xff));
    result.b = (std::uint8_t)(((color) & 0xff));
    result.a = (std::uint8_t) alpha;
	return result;
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor color, void *context)
{
    sf::ConvexShape polygon(vertexCount);
	for (int i = 0; i<vertexCount; i++)
	{
		b2Vec2 vertex = vertices[i];
		polygon.setPoint(i, sf::Vector2f(vertex.x*PIXELS_PER_METER, vertex.y*PIXELS_PER_METER));
	}
    polygon.setFillColor(sf::Color::Transparent);
    polygon.setOutlineColor(B2SFColor(color));
	polygon.setOutlineThickness(m_lineThickness);
	DebugDraw::get().m_renderTarget->draw(polygon);
}

void DebugDraw::DrawSolidPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void *context)
{
    sf::Color fillColor(B2SFColor(color));
    fillColor.r *= 0.5f;
    fillColor.g *= 0.5f;
    fillColor.b *= 0.5f;
    fillColor.a = 255 * 0.5f;

	sf::ConvexShape polygon(vertexCount);
	for (int i = 0; i<vertexCount; i++)
	{
		b2Vec2 vertex = vertices[i];
		polygon.setPoint(i, sf::Vector2f(vertex.x*PIXELS_PER_METER, vertex.y*PIXELS_PER_METER));
	}
    polygon.setFillColor(fillColor);
    polygon.setOutlineColor(B2SFColor(color));
	polygon.setOutlineThickness(m_lineThickness);
    polygon.setRotation((sf::Angle)Rotation(transform.q));
    polygon.setPosition({transform.p.x*PIXELS_PER_METER, transform.p.y*PIXELS_PER_METER});
	DebugDraw::get().m_renderTarget->draw(polygon);
}

void DebugDraw::DrawCircle(b2Vec2 center, float radius, b2HexColor color, void *context)
{
    radius *= PIXELS_PER_METER;
    sf::CircleShape circle((radius));
    circle.setPosition({center.x*PIXELS_PER_METER-radius, center.y*PIXELS_PER_METER-radius});
    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineColor(B2SFColor(color));
    circle.setOutlineThickness(m_lineThickness);
    DebugDraw::get().m_renderTarget->draw(circle);
}

void DebugDraw::DrawSolidCircle(b2Transform transform, float radius, b2HexColor color, void *context)
{
    sf::Color fillColor(B2SFColor(color));
    fillColor.r *= 0.5f;
    fillColor.g *= 0.5f;
    fillColor.b *= 0.5f;
    fillColor.a = 255 * 0.5f;

    radius *= PIXELS_PER_METER;

	sf::CircleShape circle((radius));
    circle.setOrigin({radius,radius});
    circle.setPosition({transform.p.x*PIXELS_PER_METER, transform.p.y*PIXELS_PER_METER});
    circle.setFillColor(fillColor);
    circle.setOutlineColor(B2SFColor(color));
    circle.setOutlineThickness(m_lineThickness);
    DebugDraw::get().m_renderTarget->draw(circle);
    DrawTransform(transform, context); // so that it has the direction lines
}

// void DebugDraw::DrawCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void *context)
// {
//     const int POINT_COUNT = 15; // point count per curved part

//     sf::ConvexShape capsule(POINT_COUNT*2);

//     radius *= PIXELS_PER_METER;
//     p1 *= PIXELS_PER_METER;
//     p2 *= PIXELS_PER_METER;
//     float rotation = Vector2(p1 - p2).rotation().getAngle() + PI/2;

//     for (int i = 0; i < POINT_COUNT; i++)
//         capsule.setPoint(i, (sf::Vector2f)(Vector2::rotate(Vector2{-1,0}, rotation + i*PI/POINT_COUNT) * radius + p1));

//     for (int i = 0; i < POINT_COUNT; i++)
//         capsule.setPoint(i + POINT_COUNT, (sf::Vector2f)(Vector2::rotate(Vector2{1,0}, rotation + i*PI/POINT_COUNT) * radius + p2));

//     capsule.setOutlineColor(B2SFColor(color));
//     capsule.setOutlineThickness(m_lineThickness);
//     DebugDraw::get().m_renderTarget->draw(capsule);

//     DebugDraw::DrawSegment(p1, p2, color, context);
// }

void DebugDraw::DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void *context)
{
    const int POINT_COUNT = 15; // point count per curved part
    sf::Color fillColor(B2SFColor(color));
    fillColor.r *= 0.5f;
    fillColor.g *= 0.5f;
    fillColor.b *= 0.5f;
    fillColor.a = 255 * 0.5f;

    DebugDraw::DrawSegment(p1, p2, color, context);

    sf::ConvexShape capsule(POINT_COUNT*2);

    radius *= PIXELS_PER_METER;
    p1 *= PIXELS_PER_METER;
    p2 *= PIXELS_PER_METER;
    float rotation = Vector2(p1 - p2).rotation().getAngle() + PI/2;

    for (int i = 0; i < POINT_COUNT; i++)
        capsule.setPoint(i, (sf::Vector2f)(Vector2::rotate(Vector2{-1,0}, rotation + i*PI/POINT_COUNT) * radius + p1));

    for (int i = 0; i < POINT_COUNT; i++)
        capsule.setPoint(i + POINT_COUNT, (sf::Vector2f)(Vector2::rotate(Vector2{1,0}, rotation + i*PI/POINT_COUNT) * radius + p2));

    capsule.setFillColor(fillColor);
    capsule.setOutlineColor(B2SFColor(color));
    capsule.setOutlineThickness(m_lineThickness);
    DebugDraw::get().m_renderTarget->draw(capsule);
}

void DebugDraw::DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context)
{
    p1 *= PIXELS_PER_METER;
    p2 *= PIXELS_PER_METER;

    Vector2 normal = Vector2::normalize(p2 - p1);
    {
        float temp = normal.x;
        normal.x = -normal.y;
        normal.y = temp;
    }
    Vector2 thickness = normal * (m_lineThickness/2);

    sf::VertexArray line;
    line.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
    line.append(sf::Vertex{sf::Vector2f(p1.x - thickness.x, p1.y - thickness.y), B2SFColor(color)}); // top left
    line.append(sf::Vertex{sf::Vector2f(p1.x + thickness.x, p1.y + thickness.y), B2SFColor(color)}); // bottom left
    line.append(sf::Vertex{sf::Vector2f(p2.x - thickness.x, p2.y - thickness.y), B2SFColor(color)}); // top right
    line.append(sf::Vertex{sf::Vector2f(p2.x + thickness.x, p2.y + thickness.y), B2SFColor(color)}); // bottom right

    DebugDraw::get().m_renderTarget->draw(line);
}

void DebugDraw::DrawTransform(b2Transform transform, void *context)
{
    float lineProportion = 0.4f * m_lineThickness;
    b2Vec2 p1 = transform.p; // this is multiplied by pixels per meter in the draw segment function
    b2Vec2 p2;

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
    temp.setOrigin({size/2,size/2});
    temp.setPosition({p.x*PIXELS_PER_METER,p.y*PIXELS_PER_METER});
    temp.setFillColor(B2SFColor(color));
	DebugDraw::get().m_renderTarget->draw(temp);
}

void DebugDraw::DrawString(b2Vec2 p, const char* s, b2HexColor color, void* context)
{
    return;
}
