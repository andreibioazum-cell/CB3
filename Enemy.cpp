#include "Enemy.hpp"
#include "Game.hpp"  // Для Bullet

Enemy::Enemy() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    tex.loadFromFile("assets/player.png");
    sprite.setTexture(tex);
    sprite.setOrigin(27.5f, 27.5f);
    sprite.setScale(2, 2);
    reset();
}

void Enemy::reset() {
    alive = false;
    respawnTimer = RESPAWN_TIME;
    hp = MAX_HP;
}

void Enemy::spawn(sf::Vector2f playerPos) {
    float minR = std::min(1280.0f, 720.0f) * 0.30f;
    float maxR = std::min(1280.0f, 720.0f) * 0.45f;
    float angle = std::uniform_real_distribution<float>(0, 6.28319f)(rng);
    float dist = minR + std::uniform_real_distribution<float>(0, 1)(rng) * (maxR - minR);
    pos = playerPos + sf::Vector2f(std::cos(angle), std::sin(angle)) * dist;
    alive = true;
    hp = MAX_HP;
    state = WANDER;
    attackCooldown = 0;
}

void Enemy::update(float dt, sf::Vector2f playerPos, 
                   std::vector<Game::Bullet>& bullets,
                   std::function<void(int)> onHit) {
    if (!alive) {
        respawnTimer -= dt;
        if (respawnTimer <= 0) spawn(playerPos);
        return;
    }
    
    sf::Vector2f dir = playerPos - pos;
    float dist = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    if (dist > 0) dir /= dist;
    
    // AI
    if (dist < SIGHT) {
        if (dist > ATTACK_RANGE) state = CHASE;
        else if (dist < KEEP_DIST) state = RETREAT;
        else state = ATTACK;
    } else {
        state = WANDER;
    }
    
    // Движение
    if (state == CHASE) pos += dir * SPEED * dt;
    else if (state == RETREAT) pos -= dir * SPEED * 0.8f * dt;
    else if (state == ATTACK) {
        attackCooldown -= dt;
        if (attackCooldown <= 0) {
            attackCooldown = ATTACK_CD;
            if (onHit) onHit(DAMAGE);
        }
    } else if (state == WANDER) {
        wanderTimer -= dt;
        if (wanderTimer <= 0) {
            wanderTimer = 1 + std::uniform_real_distribution<float>(0, 2)(rng);
            float a = std::uniform_real_distribution<float>(0, 6.28319f)(rng);
            wanderDir = {std::cos(a), std::sin(a)};
        }
        pos += wanderDir * SPEED * 0.35f * dt;
    }
    
    angle = std::atan2(dir.y, dir.x) + 3.14159f/2;
    hitTimer = std::max(0.0f, hitTimer - dt * 3);
    
    // Попадания
    for (auto it = bullets.begin(); it != bullets.end();) {
        sf::Vector2f diff = it->pos - pos;
        if (std::sqrt(diff.x*diff.x + diff.y*diff.y) < SIZE * 0.55f) {
            hp--;
            hitTimer = 1;
            it = bullets.erase(it);
            if (hp <= 0) {
                alive = false;
                respawnTimer = RESPAWN_TIME;
                return;
            }
        } else {
            ++it;
        }
    }
}

void Enemy::draw(sf::RenderWindow& w) {
    if (!alive) return;
    
    sprite.setPosition(pos);
    sprite.setRotation(angle * 180 / 3.14159f);
    if (hitTimer > 0) {
        sprite.setColor(sf::Color(255, 255 - hitTimer*127, 255 - hitTimer*127));
    }
    w.draw(sprite);
    sprite.setColor(sf::Color::White);
    
    // HP бар
    sf::RectangleShape bg({56, 8});
    bg.setFillColor(sf::Color(0,0,0,128));
    bg.setPosition(pos.x - 28, pos.y - 45);
    w.draw(bg);
    
    sf::RectangleShape hpBar({56 * (float)hp/MAX_HP, 8});
    hpBar.setFillColor(sf::Color(230, 51, 51));
    hpBar.setPosition(pos.x - 28, pos.y - 45);
    w.draw(hpBar);
}
