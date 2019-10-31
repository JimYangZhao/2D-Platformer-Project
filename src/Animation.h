#pragma once

#include "Common.h"
#include <vector>

class Animation
{
    sf::Sprite  m_sprite;
    size_t      m_frameCount    = 1;        // total number of frames of animation in the texture
    size_t      m_currentFrame  = 0;        // how many game frames this animation has been active
    size_t      m_speed         = 0;        // the speed at which to play the animation
    Vec2        m_size          = { 1, 1 }; // size of the animation frame
    std::string m_name          = "none";
	int         m_name_index    = 1;    // 0: stand, 1: in_air, 2: running

public:

    Animation();
    Animation(const std::string & name, const sf::Texture & t);
    Animation(const std::string & name, const sf::Texture & t, size_t frameCount, size_t speed);
        
    void update();
    bool hasEnded() const;
    const std::string & getName() const;
	//std::string getName();
	int getNameIndex();

	std::string getName();

	void setName(std::string name);

    const Vec2 & getSize() const;
    sf::Sprite & getSprite();
};
