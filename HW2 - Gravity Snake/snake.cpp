#include "snake.h"
#include <Box2D/Box2D.h>
#define SFML_STATIC
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <conio.h>
#include <iostream>
#include <math.h>

using namespace sf;
using namespace std;

b2Vec2 vel; //velocity
b2Vec2 sPosition; //snake pos
Time deltaTime;

void update(b2World* world, Clock* deltaClock, b2Body* body, b2Vec2* target, Sound* jumpFX) { // use pointer to pass in reference
	//delta time using B2D and SFML
	deltaTime = deltaClock->getElapsedTime();
	deltaClock->restart();

	world->Step(deltaTime.asSeconds(), 6, 2);

	//applyForces(body); //detect player movement
	if (Keyboard::isKeyPressed(Keyboard::W)) {
		//up
		if (body->GetPosition().y <= -18.40) {
			jumpFX->play();
			vel.y += 15.0f;
		}
	}
	else if (Keyboard::isKeyPressed(Keyboard::Space)) {
		//up
		if (body->GetPosition().y <= -18.40) {
			jumpFX->play();
			vel.y += 21.8f; //max jump height
		}
	}
	else if (Keyboard::isKeyPressed(Keyboard::A)) {
		//left
		vel.x += -5.0f;

	}
	else if (Keyboard::isKeyPressed(Keyboard::S)) {
		//down
		vel.y += -15.0f;

	}
	else if (Keyboard::isKeyPressed(Keyboard::D)) {
		//right
		vel.x += 5.0f;

	}

	cout << vel.x << ", " << vel.y << endl;
	if (vel.x != 0 || vel.y != 0) {
		vel.y += -4.5;
		body->SetLinearVelocity(vel);
		vel.SetZero(); //zero velocity
	}

	
	sPosition = body->GetPosition(); //update pos value for display


	display(target);
}

void display(b2Vec2* target) { // displays the position of the target and the player

	//cout << "(" << position.x << ", " << position.y << ")" << endl;
	printf("Target: %4.2f, %4.2f --> Snake: %4.2f, %4.2f \n", target->x, target->y, sPosition.x, sPosition.y);
}

void moveTarget(b2Vec2* target) { //moves target to a new location
	float x = 3 + rand() / (RAND_MAX / 20.0); 
	float y = -17 + rand() / (RAND_MAX / 15.0);
	target->Set(x, y);
}

//collision based on pythagorean therorem
void hitTarget(b2Vec2* target, int* score, Sound* pickupFX) {
	double distance = sqrt(pow((sPosition.x - target->x), 2) + pow((sPosition.y - target->y), 2));
	if (distance < (1 + 0.5)) //1 = snake     0.5 = target
	{
		pickupFX->play();
		*score += 1;
		moveTarget(target);
	}
}