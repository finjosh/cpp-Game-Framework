#ifndef DEBUG_DRAW_HPP
#define DEBUG_DRAW_HPP

#pragma once

#include "EngineSettings.hpp"
#include <box2d/types.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class DebugDraw
{
public:
    DebugDraw();
    static DebugDraw& get();
    void draw(b2WorldId world, sf::RenderTarget* renderTarget);

    inline void drawAll(bool flag = true) 
    {
        this->m_drawStruct.drawShapes = flag;
        this->m_drawStruct.drawJoints = flag;
        this->m_drawStruct.drawJointExtras = flag;
        this->m_drawStruct.drawAABBs = flag;
        this->m_drawStruct.drawMass = flag;
        this->m_drawStruct.drawContacts = flag;
        this->m_drawStruct.drawGraphColors = flag;
        this->m_drawStruct.drawContactNormals = flag;
        this->m_drawStruct.drawContactImpulses = flag;
        this->m_drawStruct.drawFrictionImpulses = flag;
    }
    inline void drawShapes(bool flag = true) { this->m_drawStruct.drawShapes = flag; }
    inline void drawJoints(bool flag = true) { this->m_drawStruct.drawJoints = flag; }
    inline void drawJointExtras(bool flag = true) { this->m_drawStruct.drawJointExtras = flag; }
    inline void drawAABBs(bool flag = true) { this->m_drawStruct.drawAABBs = flag; }
    inline void drawMass(bool flag = true) { this->m_drawStruct.drawMass = flag; }
    inline void drawContacts(bool flag = true) { this->m_drawStruct.drawContacts = flag; }
    inline void drawGraphColors(bool flag = true) { this->m_drawStruct.drawGraphColors = flag; }
    inline void drawContactNormals(bool flag = true) { this->m_drawStruct.drawContactNormals = flag; }
    inline void drawContactImpulses(bool flag = true) { this->m_drawStruct.drawContactImpulses = flag; }
    inline void drawFrictionImpulses(bool flag = true) { this->m_drawStruct.drawFrictionImpulses = flag; }
    inline static void setLineThickness(float thickness = 1.f) { m_lineThickness = thickness; }
    inline static float getLineThickness() { return m_lineThickness; }

    #ifdef COMMANDHANDLER_H
    /// @note this relies on this class be accessible when ever the command is called
    inline void initCommands()
    {
        Command::Handler::addCommand(Command::command("Physics", "Namespace for physics commands", {&Command::helpCommand, "Physics"}, {},
            {Command::command("setDebug", "[flags] sets the given debug flags", {[this](Command::Data* data)
            {
                this->drawAll(false); // reseting debug flags
                while (data->getNumTokens())
                {
                    auto token = data->getToken(0);
                    StringHelper::toLower(token);

                    if (token == "shapes")
                        this->drawShapes(true);
                    else if (token == "joints")
                        this->drawShapes(true);
                    else if (token == "jointextras")
                        this->drawShapes(true);
                    else if (token == "aabbs")
                        this->drawShapes(true);
                    else if (token == "mass")
                        this->drawShapes(true);
                    else if (token == "contacts")
                        this->drawShapes(true);
                    else if (token == "graphcolors")
                        this->drawShapes(true);
                    else if (token == "contactnormals")
                        this->drawShapes(true);
                    else if (token == "contactimpulses")
                        this->drawShapes(true);
                    else if (token == "frictionimpulses")
                        this->drawShapes(true);
                    else if (token == "all")
                        this->drawAll(true);
                    
                    data->removeToken();
                }
            }}, {"Shapes", "Joints", "JointExtras", "AABBs", "Mass", "Contacts", "GraphColors", "ContactNormals", "ContactImpulses", "FrictionImpulses", "All"})
        }));
    }
    #endif

    static void DrawPolygon(const b2Vec2* vertices, int vertexCount, b2HexColor color, void *context);
	static void DrawSolidPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void *context);
	static void DrawCircle(b2Vec2 center, float radius, b2HexColor color, void *context);
	static void DrawSolidCircle(b2Transform transform, float radius, b2HexColor color, void *context);
    // /// @brief draws with point count of 30 (30 total points for the curves)
    // static void DrawCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void *context);
    /// @brief draws with point count of 30 (30 total points for the curves)
    static void DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void *context);
	static void DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context);
	static void DrawTransform(b2Transform transform, void *context);
	static void DrawPoint(b2Vec2 p, float size, b2HexColor color, void *context);
    /// @note does nothing at the moment
	static void DrawString(b2Vec2 p, const char *s, void *context);

private:
    b2DebugDraw m_drawStruct;
    sf::RenderTarget* m_renderTarget = nullptr;
    static float m_lineThickness;
};

#endif
