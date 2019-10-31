#pragma once

#include <bitset>
#include <array>
#include "Animation.h"
#include "Assets.h"

class Component;
class Entity;

const size_t MaxComponents = 32;

class Component
{
public:
    bool has = false;
};

class CTransform : public Component
{
public:
    Vec2 pos        = { 0.0, 0.0 };
    Vec2 prevPos    = { 0.0, 0.0 };
    Vec2 scale      = { 1.0, 1.0 };
    Vec2 velocity   = { 0.0, 0.0 };
    float angle     = 0;

    CTransform() {}
    CTransform(const Vec2 & p)
        : pos(p) {}
    CTransform(const Vec2 & p, const Vec2 & v, const Vec2 & sc, float a)
        : pos(p), prevPos(p), velocity(v), scale(sc), angle(a) {}

};

class CLifeSpan : public Component
{
public:
    sf::Clock clock;
    int lifespan = 0;
    CLifeSpan() {}
    CLifeSpan(int l) : lifespan(l) {}
};

class CInput : public Component
{
public:
    bool up         = false;
    bool down       = false;
    bool left       = false;
    bool right      = false;
    bool shoot      = false;
    bool canShoot   = true;

    CInput() {}
};

class CBoundingBox : public Component
{
public:
    Vec2 size;
    Vec2 halfSize;
    CBoundingBox() {}
    CBoundingBox(const Vec2 & s)
        : size(s), halfSize(s.x / 2, s.y / 2) {}
};

class CAnimation : public Component
{
public:
    Animation animation;
    bool repeat = false;
    CAnimation() {}
    CAnimation(const Animation & animation, bool r)
        : animation(animation), repeat(r) {}
};

class CGravity : public Component
{
public:
    float gravity = 0;
    CGravity() {}
    CGravity(float g) : gravity(g) {}
};

class CState : public Component
{
public:
    std::string state = "jumping";
	bool is_on_ground = false;
    CState() {}
    CState(const std::string & s) : state(s) {}
};
