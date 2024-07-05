#ifndef DEBUG_DRAW_HPP
#define DEBUG_DRAW_HPP

#pragma once

#include <Box2D/Box2D.h>
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "Settings.hpp"

class DebugDraw : public b2Draw
{
public:
    DebugDraw(sf::RenderWindow* window);
    void setRenderWindow(sf::RenderWindow* window);
    sf::RenderWindow* getRenderWindow() const;

    #ifdef COMMANDHANDLER_H
    /// @note this relies on this class be accessible when ever the command is called
    inline void initCommands()
    {
        Command::Handler::addCommand(Command::command("Physics", "Namespace for physics commands", {&Command::helpCommand, "Physics"}, {},
            {Command::command("setDebug", "[flags] sets the given debug flags (com, aabb, pair, joint, shape, all)", {[this](Command::Data* data)
            {
                this->SetFlags(0);
                while (data->getNumTokens())
                {
                    auto token = data->getToken(0);
                    StringHelper::toLower(token);

                    if (token == "com")
                        this->AppendFlags(b2Draw::e_centerOfMassBit);
                    else if (token == "aabb")
                        this->AppendFlags(b2Draw::e_aabbBit);
                    else if (token == "pair")
                        this->AppendFlags(b2Draw::e_pairBit);
                    else if (token == "joint")
                        this->AppendFlags(b2Draw::e_jointBit);
                    else if (token == "shape")
                        this->AppendFlags(b2Draw::e_shapeBit);
                    else if (token == "all")
                        this->SetFlags(b2Draw::e_centerOfMassBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_jointBit | b2Draw::e_shapeBit);
                    
                    data->removeToken();
                }
            }}, {"COM", "AABB", "Pair", "Joint", "Shape", "All"})
        }));
    }
    #endif

    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
	void DrawTransform(const b2Transform& xf) override;
	void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;
    /// @note does nothing at the moment
	void DrawString(int x, int y, const char* string, ...); 
    /// @note does nothing at the moment
	void DrawString(const b2Vec2& p, const char* string, ...);
	void DrawAABB(b2AABB* aabb, const b2Color& color);

private:
    sf::RenderWindow* m_renderWindow;
};

#endif

