#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    // TODO: return the overlap rectangle size of the bounding boxes of entity a and b
	Vec2 a_position = a->getComponent<CTransform>().pos;
	Vec2 b_position = b->getComponent<CTransform>().pos;
	Vec2 b_halfBB = b->getComponent<CBoundingBox>().halfSize;
	Vec2 a_halfBB = a->getComponent<CBoundingBox>().halfSize;

	float overlap_x = (a_halfBB.x + b_halfBB.x) - std::abs(b_position.x - a_position.x);
	float overlap_y = (a_halfBB.y + b_halfBB.y) - std::abs(b_position.y - a_position.y);

	return Vec2(overlap_x, overlap_y);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    // TODO: return the previous overlap rectangle size of the bounding boxes of entity a and b
    //       previous overlap uses the entity's previous position
	Vec2 a_position = a->getComponent<CTransform>().prevPos;
	Vec2 b_position = b->getComponent<CTransform>().pos;
	Vec2 b_halfBB = b->getComponent<CBoundingBox>().halfSize;
	Vec2 a_halfBB = a->getComponent<CBoundingBox>().halfSize;

	float overlap_x = (a_halfBB.x + b_halfBB.x) - std::abs(b_position.x - a_position.x);
	float overlap_y = (a_halfBB.y + b_halfBB.y) - std::abs(b_position.y - a_position.y);

	return Vec2(overlap_x, overlap_y);
}
