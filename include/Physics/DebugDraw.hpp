#ifndef DEBUG_DRAW_HPP
#define DEBUG_DRAW_HPP

#pragma once

#include "EngineSettings.hpp"
#include "Vector2.hpp"
#include "WorldHandler.hpp"
#include <box2d/types.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class DebugDraw
{
public:
    DebugDraw();
    static DebugDraw& get();
    void draw(WorldHandler& world, sf::RenderTarget* renderTarget);

    inline void drawAll(bool flag = true) 
    {
        this->m_drawStruct.drawShapes = flag;
        this->m_drawStruct.drawJoints = flag;
        this->m_drawStruct.drawJointExtras = flag;
        this->m_drawStruct.drawBounds = flag;
        this->m_drawStruct.drawMass = flag;
        this->m_drawStruct.drawBodyNames = flag;
        this->m_drawStruct.drawContacts = flag;
        this->m_drawStruct.drawGraphColors = flag;
        this->m_drawStruct.drawContactNormals = flag;
        this->m_drawStruct.drawContactImpulses = flag;
        this->m_drawStruct.drawContactFeatures = flag;
        this->m_drawStruct.drawFrictionImpulses = flag;
        this->m_drawStruct.drawIslands = flag;
    }

    inline Vector2 getLowerBound() const { return Vector2(this->m_drawStruct.drawingBounds.lowerBound); }
    inline Vector2 getUpperBound() const { return Vector2(this->m_drawStruct.drawingBounds.upperBound); }
    inline bool isUsingDrawingBounds() const { return this->m_drawStruct.useDrawingBounds; };
    inline bool isDrawShapes() const { return this->m_drawStruct.drawShapes; };
    inline bool isDrawJoints() const { return this->m_drawStruct.drawJoints; };
    inline bool isDrawJointExtras() const { return this->m_drawStruct.drawJointExtras; };
    inline bool isDrawBounds() const { return this->m_drawStruct.drawBounds; };
    inline bool isDrawMass() const { return this->m_drawStruct.drawMass; };
    inline bool isDrawBodyNames() const { return this->m_drawStruct.drawBodyNames; };
    inline bool isDrawContacts() const { return this->m_drawStruct.drawContacts; };
    inline bool isDrawGraphColors() const { return this->m_drawStruct.drawGraphColors; };
    inline bool isDrawContactNormals() const { return this->m_drawStruct.drawContactNormals; };
    inline bool isDrawContactImpulses() const { return this->m_drawStruct.drawContactImpulses; };
    inline bool isDrawContactFeatures() const { return this->m_drawStruct.drawContactFeatures; };
    inline bool isDrawFrictionImpulses() const { return this->m_drawStruct.drawFrictionImpulses; };
    inline bool isDrawIslands() const { return this->m_drawStruct.drawIslands; };

    inline void setDrawingBounds(Vector2 lowerBound, Vector2 upperBound) { this->m_drawStruct.drawingBounds = b2AABB{b2Vec2{lowerBound.x, lowerBound.y}, b2Vec2{upperBound.x, upperBound.y}}; };
    inline void useDrawingBounds(bool flag = true) { this->m_drawStruct.useDrawingBounds = flag; };
    inline void drawShapes(bool flag = true) { this->m_drawStruct.drawShapes = flag; };
    inline void drawJoints(bool flag = true) { this->m_drawStruct.drawJoints = flag; };
    inline void drawJointExtras(bool flag = true) { this->m_drawStruct.drawJointExtras = flag; };
    inline void drawBounds(bool flag = true) { this->m_drawStruct.drawBounds = flag; };
    inline void drawMass(bool flag = true) { this->m_drawStruct.drawMass = flag; };
    inline void drawBodyNames(bool flag = true) { this->m_drawStruct.drawBodyNames = flag; };
    inline void drawContacts(bool flag = true) { this->m_drawStruct.drawContacts = flag; };
    inline void drawGraphColors(bool flag = true) { this->m_drawStruct.drawGraphColors = flag; };
    inline void drawContactNormals(bool flag = true) { this->m_drawStruct.drawContactNormals = flag; };
    inline void drawContactImpulses(bool flag = true) { this->m_drawStruct.drawContactImpulses = flag; };
    inline void drawContactFeatures(bool flag = true) { this->m_drawStruct.drawContactFeatures = flag; };
    inline void drawFrictionImpulses(bool flag = true) { this->m_drawStruct.drawFrictionImpulses = flag; };
    inline void drawIslands(bool flag = true) { this->m_drawStruct.drawIslands = flag; };

    inline static void setLineThickness(float thickness = 1.f) { m_lineThickness = thickness; }
    inline static float getLineThickness() { return m_lineThickness; }

    #ifdef COMMAND_HANDLER_H
    /// @note this relies on this class be accessible when ever the command is called
    inline void initCommands()
    {
        Command::Handler::get().addCommand("Physics", "Namespace for physics commands", {&Command::helpCommand, "Physics"});
        Command::Handler::get().findCommand("Physics")
        ->addCommand("setDebug", "[flags] sets the given debug flags", 
            [this](Command::Data* data)
            {
                this->drawAll(false); // reseting debug flags
                while (data->getNumTokens())
                {
                    auto token = data->getToken(0);
                    StringHelper::toLower(token);

                    if (token == "shapes")
                        this->drawShapes(true);
                    else if (token == "joints")
                        this->drawJoints(true);
                    else if (token == "jointExtras")
                        this->drawJointExtras(true);
                    else if (token == "bounds")
                        this->drawBounds(true);
                    else if (token == "mass")
                        this->drawMass(true);
                    else if (token == "bodyNames")
                        this->drawBodyNames(true);
                    else if (token == "contacts")
                        this->drawContacts(true);
                    else if (token == "graphColors")
                        this->drawGraphColors(true);
                    else if (token == "contactNormals")
                        this->drawContactNormals(true);
                    else if (token == "contactImpulses")
                        this->drawContactImpulses(true);
                    else if (token == "contactFeatures")
                        this->drawContactFeatures(true);
                    else if (token == "frictionImpulses")
                        this->drawFrictionImpulses(true);
                    else if (token == "islands")
                        this->drawIslands(true);
                    else if (token == "all")
                        this->drawAll(true);
                    else
                    {
                        data->addError(Command::ERROR_COLOR + "Invalid debug flag: " + token);
                    }
                    
                    data->removeToken();
                }
            }, {"All", "Shapes", "Joints", "JointExtras", "Bounds", "Mass", "BodyNames", "Contacts", "GraphColors", "ContactNormals", "ContactImpulses", "ContactFeatures", "FrictionImpulses", "Islands"})
        .addCommand("useDrawingBounds", "[flag] sets the use drawing bounds flag\nBounds are autoset to main camera viewport\nCan cause issues resulting in some shapes not being drawn", 
            [this](Command::Data* data)
            {
                if (data->getNumTokens() == 0)
                    return;
                auto token = data->getToken(0);
                StringHelper::toLower(token);
                if (token == "true" || token == "1")
                    this->useDrawingBounds(true);
                else if (token == "false" || token == "0")
                    this->useDrawingBounds(false);
                else
                {
                    data->addError(Command::ERROR_COLOR + "Invalid flag: " + token);
                }
            }, {"True", "False"});
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
	static void DrawString(b2Vec2 p, const char* s, b2HexColor color, void* context);

private:
    b2DebugDraw m_drawStruct;
    sf::RenderTarget* m_renderTarget = nullptr;
    static float m_lineThickness;
};

#endif
