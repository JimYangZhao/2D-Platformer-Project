#include "Animation.h"
#include <math.h>

Animation::Animation()
{
}

Animation::Animation(const std::string & name, const sf::Texture & t)
    : Animation(name, t, 1, 0)
{

}

Animation::Animation(const std::string & name, const sf::Texture & t, size_t frameCount, size_t speed)
    : m_name        (name)
    , m_sprite      (t)
    , m_frameCount  (frameCount)
    , m_currentFrame(0)
    , m_speed       (speed)
{
    m_size = Vec2((float)t.getSize().x / frameCount, (float)t.getSize().y);
    m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
    m_sprite.setTextureRect(sf::IntRect(floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
}

// updates the animation to show the next frame, depending on its speed
// animation loops when it reaches the end
void Animation::update()
{
    // add the speed variable to the current frame
    m_currentFrame++;
    
    // TODO: 1) calculate the correct frame of animation to play based on currentFrame and speed
    //       2) set the texture rectangle properly (see constructor for sample)
	
	// only switch frames for those who has multiple frames //
	if (m_frameCount > 1)
	{
		// frames switching loop //
		
		if (m_currentFrame >= m_frameCount * m_speed)
		{
			m_currentFrame = 0;
		}

		// switch frame by its frame_n //
		int frame_n = floor(m_currentFrame * 1.0 / m_speed * 1.0);
		m_sprite.setTextureRect(
			sf::IntRect(frame_n * m_size.x, 0, m_size.x, m_size.y)
		);
	
	
	
	}
}

const Vec2 & Animation::getSize() const
{
    return m_size;
}

const std::string & Animation::getName() const
{
    return m_name;
}

int Animation::getNameIndex()
{
	return m_name_index;
}

//// something error with the original provided getName(), so I override this function //
std::string Animation::getName()
{
	return m_name;
}

void Animation::setName(std::string name)
{
	m_name = name;
}

sf::Sprite & Animation::getSprite()
{
    return m_sprite;
}

bool Animation::hasEnded() const
{
    // TODO: detect when animation has ended (last frame was played) and return true
	bool has_ended = (m_currentFrame >= m_frameCount * m_speed);
	return has_ended;
}