#pragma once

#include "Common.h"
#include "GameState.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

struct PlayerConfig 
{ 
    float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY; 
    std::string WEAPON; 
};

class GameState_Play : public GameState
{

protected:

    EntityManager           m_entityManager;
    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;

	int                     m_coins = 0;
	sf::Text                m_text_coins;
	sf::Clock               m_gameplay_clock;
	sf::Text                m_text_time_passed;
	int                     m_flag_x = 0;
	bool                    m_stage_clear = false;
	sf::Text                m_text_stage_clear;
	int                     m_time_till_win = 0;

    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    
    void init(const std::string & levelPath);

    void loadLevel(const std::string & filename);

    void update();
    void spawnPlayer();
    void spawnBullet(std::shared_ptr<Entity> entity);
    
    void sMovement();
    void sLifespan();
    void sUserInput();
    void sAnimation();
    void sCollision();
	void sHandleWin();
    void sRender();

public:

    GameState_Play(GameEngine & game, const std::string & levelPath);

};