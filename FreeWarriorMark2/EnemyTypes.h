#pragma once

#include "Squares.h"

enum class EnemyMove {
    Move,
    Attack,
    Wait,
    None
};

//struct enemyMoveData
//{
//    enemyMove movetype;
//    Squares* targetPos;
//    float targetX = 0;
//    float targetY = 0;
//    int targetID = -1; // 攻撃対象などに使える
//};

enum class AIMode {
    decide,
    Do
};
