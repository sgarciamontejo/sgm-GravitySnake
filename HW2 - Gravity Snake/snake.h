#pragma once
#include <Box2D/Box2D.h>
#define SFML_STATIC
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

void update(b2World* world, Clock* deltaClock, b2Body* body, b2Vec2* target, Sound* jumpFX);
void display(b2Vec2* target);
void moveTarget(b2Vec2* target);
void hitTarget(b2Vec2* target, int* score, Sound* pickupFX);