#include "Game.hpp"
#include <cmath>

void Game::init(sf::RenderWindow& win) {
    window = &win;
    
    playerTex.loadFromFile("assets/player.png");
    player.setTexture(playerTex);
    player.setOrigin(27.5f, 27.5f);
    player.setScale(2, 2);
    
    bgTex.loadFromFile("assets/grass.png");
    bgTex.setRepeated(true);
    bg.setTexture(bgTex);
    bg.setScale(2, 2);
    
    font.loadFromFile("assets/Fredoka-Bold.ttf");
    hpText.setFont(font);
    hpText.setCharacterSize(18);
    hpText.setFillColor(sf::Color::White);
    hpText.setPosition(1060, 20);
    
    reset();
}

void Game::reset() {
    playerPos = {640, 360};
    playerAngle = 0;
    hp = PLAYER_HP_MAX;
    hitTimer = 0;
    dead = false;
    backToLobby = false;
    bullets.clear();
    cam = {0, 0};
    enemy.reset();
    controls.joyStick = controls.joyPos;
    controls.atkHold = false;
    controls.joyActive = false;
}

void Game::update(float dt) {
    if (dead) return;
    
    // Движение
    if (controls.joyActive) {
        sf::Vector2f dir = controls.joyStick - controls.joyPos;
        float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        if (len > 0) {
            dir.x /= len;
            dir.y /= len;
            playerPos.x += dir.x * 260.0f * dt;
            playerPos.y += dir.y * 260.0f * dt;
            playerAngle = std::atan2(dir.y, dir.x) + 3.14159f/2;
            aimDir = dir;
        }
    }
    
    // Атака
    controls.atkPress += (controls.atkHold ? 1 : -1) * dt * 12;
    if (controls.atkPress > 1) controls.atkPress = 1;
    if (controls.atkPress < 0) controls.atkPress = 0;
    
    // Камера
    sf::Vector2f target;
    target.x = playerPos.x - 640;
    target.y = playerPos.y - 360;
    cam.x += (target.x - cam.x) * (1 - std::exp(-dt * 7.3f));
    cam.y += (target.y - cam.y) * (1 - std::exp(-dt * 7.3f));
    
    // Пули
    for (auto it = bullets.begin(); it != bullets.end();) {
        it->pos.x += it->vel.x * dt;
        it->pos.y += it->vel.y * dt;
        it->life -= dt;
        if (it->life <= 0) it = bullets.erase(it);
        else ++it;
    }
    
    // Враг
    enemy.update(dt, playerPos, bullets, [this](int dmg) {
        hp -= dmg;
        hitTimer = 1;
        if (hp <= 0) {
            hp = 0;
            dead = true;
            backToLobby = true;
        }
    });
    
    hitTimer = std::max(0.0f, hitTimer - dt * 3);
    hpText.setString("HP " + std::to_string(std::max(0, hp)) + " / " + std::to_string(PLAYER_HP_MAX));
}

void Game::draw(sf::RenderWindow& w) {
    sf::View view(cam, sf::Vector2f(1280, 720));
    w.setView(view);
    
    // Трава
    for (int x = -100; x < 1500; x += 128) {
        for (int y = -100; y < 1000; y += 128) {
            bg.setPosition(x, y);
            w.draw(bg);
        }
    }
    
    // Пули
    sf::CircleShape bulletCircle(8);
    bulletCircle.setFillColor(sf::Color::Black);
    for (auto& b : bullets) {
        bulletCircle.setPosition(b.pos);
        w.draw(bulletCircle);
    }
    
    enemy.draw(w);
    
    // Игрок
    player.setPosition(playerPos);
    player.setRotation(playerAngle * 180 / 3.14159f);
    if (hitTimer > 0) {
        int red = 255;
        int green = 255 - (int)(hitTimer * 150);
        int blue = 255 - (int)(hitTimer * 150);
        if (green < 0) green = 0;
        if (blue < 0) blue = 0;
        player.setColor(sf::Color(red, green, blue));
    } else {
        player.setColor(sf::Color::White);
    }
    w.draw(player);
    player.setColor(sf::Color::White);
    
    w.setView(w.getDefaultView());
    
    drawHPBar(w, {1060, 30}, hp, PLAYER_HP_MAX);
    w.draw(hpText);
    drawControls(w);
}

void Game::drawHPBar(sf::RenderWindow& w, sf::Vector2f pos, int hp, int max) {
    sf::RectangleShape bg({200, 18});
    bg.setFillColor(sf::Color(0,0,0,128));
    bg.setPosition(pos);
    w.draw(bg);
    
    sf::RectangleShape hpBar({200 * (float)hp/max, 18});
    hpBar.setFillColor(sf::Color(76, 217, 89));
    hpBar.setPosition(pos);
    w.draw(hpBar);
}

void Game::drawControls(sf::RenderWindow& w) {
    // Джойстик
    sf::CircleShape joyBase(45);
    joyBase.setFillColor(sf::Color(0,0,0,50));
    joyBase.setPosition(controls.joyPos - sf::Vector2f(45,45));
    w.draw(joyBase);
    
    sf::CircleShape joyStick(18);
    joyStick.setFillColor(sf::Color::Black);
    joyStick.setPosition(controls.joyStick - sf::Vector2f(18,18));
    w.draw(joyStick);
    
    // Кнопка атаки
    float scale = 1 - controls.atkPress * 0.12f;
    sf::CircleShape atkBtn(52 * scale);
    int r = 140 - (int)(controls.atkPress * 50);
    int g = 51;
    int b = 217 - (int)(controls.atkPress * 76);
    atkBtn.setFillColor(sf::Color(r, g, b));
    atkBtn.setPosition(controls.atkPos - sf::Vector2f(52*scale, 52*scale));
    w.draw(atkBtn);
    
    sf::Text atkText("SHOT", font, 24);
    atkText.setFillColor(sf::Color::White);
    atkText.setPosition(controls.atkPos - sf::Vector2f(40, 12));
    w.draw(atkText);
    
    // Back
    sf::RectangleShape backBtn({140, 55});
    backBtn.setFillColor(sf::Color(140, 51, 217));
    backBtn.setPosition(20, 20);
    w.draw(backBtn);
    
    sf::Text backText("BACK", font, 24);
    backText.setFillColor(sf::Color::White);
    backText.setPosition(60, 32);
    w.draw(backText);
}

void Game::spawnBullet(sf::Vector2f dir) {
    Bullet b;
    b.pos = playerPos;
    b.vel.x = dir.x * BULLET_SPEED;
    b.vel.y = dir.y * BULLET_SPEED;
    bullets.push_back(b);
}

void Game::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::TouchBegan || e.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f pos;
        if (e.type == sf::Event::TouchBegan) pos = {e.touch.x, e.touch.y};
        else pos = {e.mouseButton.x, e.mouseButton.y};
        
        if (std::abs(pos.x - controls.joyPos.x) < 45 && std::abs(pos.y - controls.joyPos.y) < 45) {
            controls.joyActive = true;
            if (e.type == sf::Event::TouchBegan) controls.joyId = e.touch.finger;
            controls.joyStick = pos;
        }
        if (std::abs(pos.x - controls.atkPos.x) < 52 && std::abs(pos.y - controls.atkPos.y) < 52) {
            controls.atkHold = true;
            if (e.type == sf::Event::TouchBegan) controls.atkId = e.touch.finger;
        }
        if (pos.x > 20 && pos.x < 160 && pos.y > 20 && pos.y < 75) {
            backToLobby = true;
        }
    }
    
    if (e.type == sf::Event::TouchMoved) {
        if (controls.joyActive && controls.joyId == e.touch.finger) {
            sf::Vector2f pos(e.touch.x, e.touch.y);
            sf::Vector2f dir = pos - controls.joyPos;
            float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
            if (len > 45) {
                dir.x = dir.x / len * 45;
                dir.y = dir.y / len * 45;
            }
            controls.joyStick = controls.joyPos + dir;
        }
    }
    
    if (e.type == sf::Event::TouchEnded) {
        if (controls.joyId == e.touch.finger) {
            controls.joyActive = false;
            controls.joyStick = controls.joyPos;
            controls.joyId = -1;
        }
        if (controls.atkId == e.touch.finger) {
            controls.atkHold = false;
            spawnBullet(aimDir);
            controls.atkId = -1;
        }
    }
    
    if (e.type == sf::Event::MouseButtonReleased) {
        if (controls.atkHold) {
            controls.atkHold = false;
            spawnBullet(aimDir);
        }
        controls.joyActive = false;
        controls.joyStick = controls.joyPos;
    }
}
