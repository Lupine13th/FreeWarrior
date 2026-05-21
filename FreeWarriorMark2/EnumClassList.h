#pragma once

enum class GAME_SCENES
{
	AWAKE,
	INIT,
	TITLE,
	READY,
	IN_GAME,
	GAME_OVER,
};

enum class HIT_ORDER
{
	HIT_PLAYER_BODY,
	HIT_PLAYER_ATTACK,
	HIT_PLAYER_SHIELD,
	HIT_ENEMY_BODY,
	HIT_ENEMY_ATTACK,
	HIT_ENEMY_SHIELD,
	HIT_ITEM,
};

enum class DamegeAnimationState
{
	None,
	Delay,
	Play
};

enum class Vector
{
	X = 0,
	Y = 1
};
