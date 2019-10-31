#include "GameState_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"

GameState_Play::GameState_Play(GameEngine & game, const std::string & levelPath)
    : GameState(game)
    , m_levelPath(levelPath)
{
    init(m_levelPath);
}

void GameState_Play::init(const std::string & levelPath)
{
    loadLevel(levelPath);
	m_coins = 0;
	m_stage_clear = false;
	m_text_stage_clear.setString("");
	m_paused = false;
	m_gameplay_clock.restart();
	//m_player->getComponent<Animation>().setName("in_air");
	//std::string player_animation = m_player->getComponent<Animation>().get_Name();
	//std::cout << player_animation << std::endl;
}

void GameState_Play::loadLevel(const std::string & filename)
{
    // reset the entity manager every time we load a level
    m_entityManager = EntityManager();

	// read config and add entities //
	std::ifstream in_file;
	in_file.open(filename, std::iostream::in);

	std::string attr_name;

	while (in_file.peek() != EOF)
	{
		in_file >> attr_name;

		if (attr_name == "Player")
		{
			in_file >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.JUMP >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;
		}

		if (attr_name == "Tile" || attr_name == "Dec")
		{
			std::string animation;
			int xPos, yPos;
			in_file >> animation >> xPos >> yPos;

			// set up entity //
			auto entity = m_entityManager.addEntity(attr_name);

			entity->addComponent<CTransform>(
				Vec2(xPos, yPos)
			);
			entity->addComponent<CAnimation>(
				m_game.getAssets().getAnimation(animation),
				true
			);

			if (attr_name == "Tile")
			{
				entity->addComponent<CBoundingBox>(
					m_game.getAssets().getAnimation(animation).getSize()
				);
			};

			// store flag x position if there exists a flag //
			if (animation == "Flag")
			{
				m_flag_x = xPos;
			}
		}
	}

	// setup text coins //
	m_text_coins.setFont(
		m_game.getAssets().getFont("Mario")
	);
	m_text_coins.setCharacterSize(
        30
    );
	m_text_coins.setString(
		"Coins: " + std::to_string(m_coins)
	);

	// setup text time passed //
	m_text_time_passed.setFont(
		m_game.getAssets().getFont("Megaman")
	);
	m_text_time_passed.setCharacterSize(
		20
	);
	m_text_time_passed.setString(
		"Time: " + 0
	);

	// setup win text //
	m_text_stage_clear.setFont(
		m_game.getAssets().getFont("Megaman")
	);
	m_text_stage_clear.setCharacterSize(
		40
	);
	m_text_stage_clear.setColor(sf::Color::Green);
	//m_text_stage_clear.setString(
	//	"Stage Clear"
	//);

	spawnPlayer();
}

void GameState_Play::spawnPlayer()
{
    //// here is a sample player entity which you can use to construct other entities
    //m_player = m_entityManager.addEntity("player");
    //m_player->addComponent<CTransform>(Vec2(200, 200));
    //m_player->addComponent<CBoundingBox>(Vec2(48, 48));
    //m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("Stand"), true);

    //// TODO: be sure to add the remaining components to the player

	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CTransform>(
		Vec2(m_playerConfig.X, m_game.window().getSize().y - m_playerConfig.Y)
	);
	m_player->addComponent<CBoundingBox>(
		Vec2(m_playerConfig.CX, m_playerConfig.CY)
	);
	m_player->addComponent<CAnimation>(
		m_game.getAssets().getAnimation("Stand"),
		true
	);
	m_player->addComponent<CState>("jumping");
	m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
	m_player->addComponent<CInput>();
}

void GameState_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
    // TODO: this should spawn a bullet at the given entity, going in the direction the entity is facing
	if (entity->getComponent<CInput>().canShoot)
	{
		auto bullet = m_entityManager.addEntity("bullet");
		bullet->addComponent<CTransform>(
            Vec2(entity->getComponent<CTransform>().pos.x, entity->getComponent<CTransform>().pos.y)
        );
		bullet->getComponent<CTransform>().velocity.x = 
			m_playerConfig.SPEED * 3 * entity->getComponent<CTransform>().scale.x;
		bullet->getComponent<CTransform>().scale = 
			entity->getComponent<CTransform>().scale;
		bullet->addComponent<CAnimation>(
			m_game.getAssets().getAnimation(m_playerConfig.WEAPON), 
			true
		);
		bullet->addComponent<CBoundingBox>(
			bullet->getComponent<CAnimation>().animation.getSize()
		);
		bullet->addComponent<CLifeSpan>(3000);
	}
}

void GameState_Play::update()
{
    m_entityManager.update();

    // TODO: implement pause functionality
	
	if (!m_paused)
	{
		sMovement();
		sLifespan();
		sCollision();
		sAnimation();
		sHandleWin();
	}
    sUserInput();
	//if (!m_stage_clear)
	//{
		sRender();
	//}
}

// 
void GameState_Play::sHandleWin()
{
	if (m_flag_x != 0)
	{
		if (m_player->getComponent<CTransform>().pos.x > m_flag_x + 64 && m_player->getComponent<CTransform>().pos.y <= 96)
		{
			m_paused = !m_paused;

			if (!m_stage_clear)
			{
				m_text_stage_clear.setString(
					"Stage Clear"
				);
			}

			m_stage_clear = true;

		}
	}
}

void GameState_Play::sMovement()
{
	// TODO: Implement player movement / jumping based on its CInput component
	// TODO: Implement gravity's effect on the player
	// TODO: Implement the maxmimum player speed in both X and Y directions
	// NOTE: Setting an entity's scale.x to -1/1 will make it face to the left/right

	Vec2 velocity_curr = m_player->getComponent<CTransform>().velocity;

	// left & right //
	if (m_player->getComponent<CInput>().left && !(m_player->getComponent<CInput>().right))
	{
		m_player->getComponent<CTransform>().scale.x = -1;
		velocity_curr.x = (-1) * m_playerConfig.SPEED;

		// player state: running //
		if (m_player->getComponent<CState>().state != "running" && m_player->getComponent<CState>().is_on_ground)
		{
			m_player->getComponent<CState>().state = "running";
		}
	}
	else if (m_player->getComponent<CInput>().right && !(m_player->getComponent<CInput>().left))
	{
		m_player->getComponent<CTransform>().scale.x = 1;
		velocity_curr.x = (+1) * m_playerConfig.SPEED;

		// player state: running //
		if (m_player->getComponent<CState>().state != "running" && m_player->getComponent<CState>().is_on_ground)
		{
			m_player->getComponent<CState>().state = "running";
		}
	}
	else
	{
		velocity_curr.x = 0;

		// player state: stand //
		m_player->getComponent<CState>().state = "standing";
	}
	// vertical //
	if (!m_player->getComponent<CState>().is_on_ground)
	{
		velocity_curr.y += m_player->getComponent<CGravity>().gravity;

		// player state: in_air //
		m_player->getComponent<CState>().state = "in_air";
	}
	else
	{
		if (m_player->getComponent<CInput>().up)
		{
			velocity_curr.y = m_playerConfig.JUMP + m_player->getComponent<CGravity>().gravity;
			m_player->getComponent<CState>().is_on_ground = false;
		}
		else
		{
			velocity_curr.y = 0;
		}
	}

	// restrict velocities //
	if (velocity_curr.x >0 && velocity_curr.x > m_playerConfig.MAXSPEED)
	{
		velocity_curr.x = m_playerConfig.MAXSPEED;
	}
	else if (velocity_curr.x < 0 && velocity_curr.x < -m_playerConfig.MAXSPEED)
	{
		velocity_curr.x = -m_playerConfig.MAXSPEED;
	}
	if (velocity_curr.y > 0 && velocity_curr.y > m_playerConfig.MAXSPEED)
	{
		velocity_curr.y = m_playerConfig.MAXSPEED;
	}
	else if (velocity_curr.y < 0 && velocity_curr.y < -m_playerConfig.MAXSPEED)
	{
		velocity_curr.y = -m_playerConfig.MAXSPEED;
	}

	// apply changes //
	m_player->getComponent<CTransform>().velocity = velocity_curr;
	m_player->getComponent<CTransform>().prevPos = m_player->getComponent<CTransform>().pos;
	m_player->getComponent<CTransform>().pos += m_player->getComponent<CTransform>().velocity;

	// bullet movement //
	for (auto bullet : m_entityManager.getEntities("bullet"))
	{
		bullet->getComponent<CTransform>().pos.x += bullet->getComponent<CTransform>().velocity.x;
	}
}

void GameState_Play::sLifespan()
{
    // TODO: Check lifespawn of entities that have them, and destroy them if they go over
	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CLifeSpan>())
		{
			auto lived_time = e->getComponent<CLifeSpan>().clock.getElapsedTime().asMilliseconds();
			if (lived_time > e->getComponent<CLifeSpan>().lifespan)
			{
				e->destroy();
			}
		}
	}
}

void GameState_Play::sCollision()
{
    // TODO: Implement Physics::GetOverlap() function, use it inside this function

    // TODO: Implement bullet / tile collisions
    //       Destroy the tile if it has a Brick animation
    // TODO: Implement player / tile collisions and resolutions
    //       Update the CState component of the player to store whether
    //       it is currently on the ground or in the air. This will be
    //       used by the Animation system
    // TODO: Check to see if the player has fallen down a hole (y < 0)
    // TODO: Don't let the player walk off the left side of the map

	// defaultly assume player in air //
	bool player_in_air = true;

	// collisions with tile //
	for (auto tile : m_entityManager.getEntities("Tile"))
	{
		if (tile->hasComponent<CBoundingBox>())
		{
			// player & tile ------------------------------------------------------------------------------
			Vec2 overLap = Physics::GetOverlap(m_player, tile);

			if (overLap.x >= 0 && overLap.y >= 0)
			{
				Vec2 overlap_prev = Physics::GetPreviousOverlap(m_player, tile);

				// force from x //
				if (overlap_prev.y > 0)
				{
					if (m_player->getComponent<CTransform>().prevPos.x < tile->getComponent<CTransform>().pos.x)
					{
						m_player->getComponent<CTransform>().pos.x -= overLap.x;
					}
					else
					{
						m_player->getComponent<CTransform>().pos.x += overLap.x;
					}
				}
				// force from y //
				if (overlap_prev.x > 0)
				{
					if (m_player->getComponent<CTransform>().prevPos.y < tile->getComponent<CTransform>().pos.y)
					{
						m_player->getComponent<CTransform>().pos.y -= overLap.y;
						m_player->getComponent<CTransform>().velocity.y = 0;    // head hit

						// hit brick //
						if (tile->getComponent<CAnimation>().animation.getName() == "Brick")
						{
							tile->addComponent<CAnimation>(m_game.getAssets().getAnimation("Explosion"), false);
							tile->addComponent<CLifeSpan>(960);
						}
						// hit question //
						else if (tile->getComponent<CAnimation>().animation.getName() == "Question")
						{
							tile->addComponent<CAnimation>(m_game.getAssets().getAnimation("Question2"), true);

							auto coin = m_entityManager.addEntity("coin");
							coin->addComponent<CTransform>(
								Vec2(
									tile->getComponent<CTransform>().pos.x,                                                                                                                                                                                          
									tile->getComponent<CTransform>().pos.y + 64
								)
							);
							coin->addComponent<CAnimation>(
								m_game.getAssets().getAnimation("Coin"), true
							);
							coin->addComponent<CLifeSpan>(500);

							m_coins++;
						}
					}
					else
					{
						m_player->getComponent<CState>().is_on_ground = true;
						m_player->getComponent<CTransform>().pos.y += overLap.y;
						player_in_air = false;
					}
				}
			}
			// bullet & tile ------------------------------------------------------------------------------
			for (auto bullet : m_entityManager.getEntities("bullet"))
			{
				Vec2 overLap = Physics::GetOverlap(bullet, tile);

				if (overLap.x >= 0 && overLap.y >= 0)
				{
					bullet->destroy();

					if (tile->getComponent<CAnimation>().animation.getName() == "Brick")
					{
						tile->addComponent<CAnimation>(m_game.getAssets().getAnimation("Explosion"), false);
						tile->addComponent<CLifeSpan>(960);
					}
				}
			}
		}

		// After a series check if still in air //
		if (player_in_air)
		{
			m_player->getComponent<CState>().is_on_ground = false;
		}
	}

	// player fall out of map //
	if (m_player->getComponent<CTransform>().pos.y < 0)
	{
		init(m_levelPath);
	}

	// restrict movement area //
	if (m_player->getComponent<CTransform>().pos.x < m_player->getComponent<CBoundingBox>().halfSize.x)
	{
		m_player->getComponent<CTransform>().pos.x = m_player->getComponent<CBoundingBox>().halfSize.x;
	}
}

void GameState_Play::sUserInput()
{
    // TODO: implement the rest of the player input

    sf::Event event;
    while (m_game.window().pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_game.quit();
        }

        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:
                    m_game.popState();
                    break;
                case sf::Keyboard::Z:
                    init(m_levelPath);
                    break;
                case sf::Keyboard::R:
                    m_drawTextures = !m_drawTextures;
                    break;
                case sf::Keyboard::F:
                    m_drawCollision = !m_drawCollision;
                    break;
                case sf::Keyboard::P:
                    setPaused(!m_paused);
                    break;
				case sf::Keyboard::W:
                    m_player->getComponent<CInput>().up = true;
                    break;
				case sf::Keyboard::S:
                    m_player->getComponent<CInput>().down = true;
                    break;
				case sf::Keyboard::D:
                    m_player->getComponent<CInput>().right = true;
                    break;
				case sf::Keyboard::A:
                    m_player->getComponent<CInput>().left = true;
                    break;
				case sf::Keyboard::Space:
                    spawnBullet(m_player);
                    break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:
                    break;
				case sf::Keyboard::W:
                    m_player->getComponent<CInput>().up = false;
                    break;
				case sf::Keyboard::S:
                    m_player->getComponent<CInput>().down = false;
                    break;
				case sf::Keyboard::D:
                    m_player->getComponent<CInput>().right = false;
                    break;
				case sf::Keyboard::A:
                    m_player->getComponent<CInput>().left = false;
                    break;
            }
        }
    }

}

void GameState_Play::sAnimation()
{
    // TODO: set the animation of the player based on its CState component
    // TODO: for each entity with an animation, call entity->getComponent<CAnimation>().animation.update()
    //       if the animation is not repeated, and it has ended, destroy the entity

	// player //

	// COMMENT:
	//     something really weird with SFML, always error on some tuple class. I cannot call any method of Animation.

	std::string player_state = m_player->getComponent<CState>().state;
	std::string player_animation = m_player->getComponent<Animation>().getName();

	if (player_state == "running" && player_animation != "Run")
	{
		m_player->addComponent<CAnimation>(
			m_game.getAssets().getAnimation("Run"), true
		);
	}
	else if (player_state == "in_air" && player_animation != "Air")
	{
		m_player->addComponent<CAnimation>(
			m_game.getAssets().getAnimation("Air"), true
		);
	}
	else if (player_state == "standing" && player_animation != "Stand")
	{
		m_player->addComponent<CAnimation>(
			m_game.getAssets().getAnimation("Stand"), true
		);
	}

	// entities //
	for (auto entity : m_entityManager.getEntities())
	{
		if (entity->hasComponent<CAnimation>())
		{
			if (entity->getComponent<CAnimation>().repeat == false)
			{
				if (entity->getComponent<CAnimation>().animation.hasEnded())
				{
					entity->destroy();
				}
			}
			entity->getComponent<CAnimation>().animation.update();
		}
		
	}

}

void GameState_Play::sRender()
{
    // NOTE: This is implemented for you, you shouldn't HAVE to change this

    // color the background darker so you know that the game is paused
    if (!m_paused) { m_game.window().clear(sf::Color(100, 100, 255)); }
    else { m_game.window().clear(sf::Color(50, 50, 150)); }

    // set the viewport of the window to be centered on the player if it's far enough right
    auto & pPos = m_player->getComponent<CTransform>().pos;
    float windowCenterX = std::max(m_game.window().getSize().x / 2.0f, pPos.x);
    sf::View view = m_game.window().getView();
    view.setCenter(windowCenterX, m_game.window().getSize().y - view.getCenter().y);
    m_game.window().setView(view);

    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities())
        {
            auto & transform = e->getComponent<CTransform>();

            if (e->hasComponent<CAnimation>())
            {
                auto & animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, m_game.window().getSize().y - transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangleshape
    if (m_drawCollision)
    {
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto & box = e->getComponent<CBoundingBox>();
                auto & transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, m_game.window().getSize().y - transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color(255, 255, 255, 255));
                rect.setOutlineThickness(1);
                m_game.window().draw(rect);
            }
        }
    }

	// coins counter //
	m_text_coins.setString(
		"Coins: " + std::to_string(m_coins*100)
	);
	m_text_coins.setPosition(view.getCenter().x - m_game.window().getSize().x/2 + 10.0f, 10.0f);
	m_game.window().draw(m_text_coins);

	// time counter //
	m_text_time_passed.setPosition(view.getCenter().x + m_game.window().getSize().x/2 - 200.0f, 10.0f);
	if (!m_stage_clear)
	{
		int time_passed = m_gameplay_clock.getElapsedTime().asMilliseconds() / 1000;
		m_time_till_win = time_passed;

		m_text_time_passed.setString(
			"Time: " + std::to_string(time_passed)
		);
	}
	else
	{
		m_text_time_passed.setString(
			"Time: " + std::to_string(m_time_till_win)
		);
	}
	m_game.window().draw(m_text_time_passed);
	
	// game win text //
	m_text_stage_clear.setPosition(view.getCenter().x - 180.0f, view.getCenter().y - 30.0f);
	m_game.window().draw(m_text_stage_clear);



    m_game.window().display();
}
