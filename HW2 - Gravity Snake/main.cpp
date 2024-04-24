#include <iostream>
#include <Box2D/Box2D.h>
#define SFML_STATIC
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "snake.h"
#include <conio.h>

using namespace sf;
using namespace std;

//CONVERSION
// Pixel Render: (800, 600)
// B2D Render: (27, 20)
// 
// 1 Meter = 30 Pixels
// 
// BOX2D POS -> SF POS
// b2 Pos * 30 = SF Pos
//
//SF POS -> BOX2D POS
//SF Pos / 30 = b2 Pos

Vector2f b2d_to_sf(float xPos, float yPos) {
	xPos = xPos * 30; //meter to pixels (sf)
	yPos = yPos * 30;

	yPos = -(yPos); //flip

	return Vector2f(xPos, yPos);
}

b2Vec2 sf_to_b2d(float xPos, float yPos) {
	xPos = xPos / 30; //pixel to meters (box2D)
	yPos = yPos / 30;

	yPos = -(yPos); //flip

	return b2Vec2(xPos, yPos);
}

int main() {
	srand(time(0));

	//sound
	//jump sound
	SoundBuffer jumpBuffer;
	jumpBuffer.loadFromFile("jump.wav");
	Sound jumpFX;
	jumpFX.setBuffer(jumpBuffer);

	SoundBuffer pickupBuffer;
	pickupBuffer.loadFromFile("pickupCoin.wav");
	Sound pickupFX;
	pickupFX.setBuffer(pickupBuffer);

	//score
	int score = 0;

	//font
	Font* font = new Font();
	font->loadFromFile("vgafix.ttf");

	//intro text
	Text* introText = new Text();
	introText->setFont(*font);
	introText->setString("Welcome to Gravity Snake!\nCollect targets by moving towards them.\nTo win, colect 20 targets.\nUse WASD to move and Space to jump\nPress enter to begin");
	introText->setCharacterSize(24);
	introText->setFillColor(Color(36, 135, 33));
	//get bounds and adjust origin and pos
	FloatRect bounds = introText->getLocalBounds();
	introText->setOrigin(bounds.left + bounds.width / 2,  bounds.top + bounds.height / 2);
	introText->setPosition(b2d_to_sf(13.5, -10));

	//end text
	Text* endText = new Text();
	endText->setFont(*font);
	endText->setCharacterSize(24);
	endText->setFillColor(Color(36, 135, 33));

	//score text
	Text* scoreText = new Text();
	scoreText->setFont(*font);
	scoreText->setCharacterSize(24);
	scoreText->setFillColor(Color(36, 135, 33));
	std::string scoreString = "Score: " + to_string(score);
	scoreText->setString(scoreString);
	scoreText->setPosition(b2d_to_sf(1, -0.5));

	//b2 gravity
	b2Vec2 gravity(0.0f, -9.8f); //less gravity
	//b2 world
	b2World world(gravity);

	//ground
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(13.5, -20);
	b2Body* groundBody = world.CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(13.5, 0.5); //half width and half height - actual size: (27, 1)
	groundBody->CreateFixture(&groundBox, 0.0f);

	//sf ground for render
	RectangleShape* ground = new RectangleShape(b2d_to_sf(27, 1)); //b2 box half (50,10) -> full (100, 20)
	ground->setFillColor(Color(36, 135, 33)); //green
	ground->setOrigin(b2d_to_sf(13.5, 0.5)); //half width and height in meters
	ground->move(b2d_to_sf(13.5, -20)); //x value centers rect in middle, y value for lowest value of 20 meter tall window

	//----------------------------------------------------------------------------------------

	//WALLS
	//Left Wall -- at (-15, 0), is 1x50
	b2BodyDef leftWallBodyDef;
	leftWallBodyDef.position.Set(0.0f, -10.0f);
	b2Body* leftWallBody = world.CreateBody(&leftWallBodyDef);

	b2PolygonShape leftWallBox;
	leftWallBox.SetAsBox(0.5f, 10.0f);
	leftWallBody->CreateFixture(&leftWallBox, 0.0f);

	//sf lef twall for render
	RectangleShape* leftWall = new RectangleShape(b2d_to_sf(1.0f, 20.0f)); //b2 box half (0.5,10) -> full (1, 20)
	leftWall->setFillColor(Color(36, 135, 33)); //white
	leftWall->setOrigin(b2d_to_sf(0.5f, 10.0f)); //half width and height in meters
	leftWall->move(b2d_to_sf(0.0f, -10.0f)); //x value centers rect in middle, y value for lowest value of 20 meter tall window

	//Right Wall -- at (15, 0), is 1x50
	b2BodyDef rightWallBodyDef;
	rightWallBodyDef.position.Set(27.0f, -10.0f);
	b2Body* rightWallBody = world.CreateBody(&rightWallBodyDef);

	b2PolygonShape rightWallBox;
	rightWallBox.SetAsBox(0.5f, 10.0f);
	rightWallBody->CreateFixture(&rightWallBox, 0.0f);

	//sf right wall for render
	RectangleShape* rightWall = new RectangleShape(b2d_to_sf(1.0f, 20.0f)); //b2 box half (0.5,10) -> full (1, 20)
	rightWall->setFillColor(Color(36, 135, 33)); //white
	rightWall->setOrigin(b2d_to_sf(0.5f, 10.0f)); //half width and height in meters
	rightWall->move(b2d_to_sf(27.0f, -10.0f)); //x value centers rect in middle, y value for lowest value of 20 meter tall window


	//Ceiling
	b2BodyDef ceilingBodyDef;
	ceilingBodyDef.position.Set(13.5, 0);
	b2Body* ceilingBody = world.CreateBody(&ceilingBodyDef);

	b2PolygonShape ceilingBox;
	ceilingBox.SetAsBox(13.5, 0.5); //half width and half height - actual size: (27, 1)
	ceilingBody->CreateFixture(&ceilingBox, 0.0f);

	//sf ceiling for render
	RectangleShape* ceiling = new RectangleShape(b2d_to_sf(27, 1)); //b2d box half (13.5, 0.5) -> full (27, 1)
	ceiling->setFillColor(Color(36, 135, 33)); //green
	ceiling->setOrigin(b2d_to_sf(13.5, 0.5)); //half width and height in meters
	ceiling->move(b2d_to_sf(13.5, 0)); //x value centers rect in middle, y value for lowest value of 20 meter tall window
	

	//----------------------------------------------------------------------------------------
	
	//box
	b2BodyDef snakeBodyDef;
	snakeBodyDef.type = b2_dynamicBody;
	snakeBodyDef.position.Set(13.5, -10);
	b2Body* snakeBody = world.CreateBody(&snakeBodyDef);

	b2PolygonShape snakeDynamicBox;
	snakeDynamicBox.SetAsBox(1.0f, 1.0f); //HALF | ACTUAL = (2.0f, 2,0f)

	//snake
	b2FixtureDef snakeFixtureDef;
	snakeFixtureDef.shape = &snakeDynamicBox;
	snakeFixtureDef.density = 1.0f;
	snakeFixtureDef.friction = 1.3f;
	snakeBody->CreateFixture(&snakeFixtureDef);

	//snake SF Render
	RectangleShape* snake = new RectangleShape(b2d_to_sf(2.0f, 2.0f)); //full size
	snake->setFillColor(Color(255, 255, 255)); //white
	snake->setOrigin(b2d_to_sf(1.0f, 1.0f)); //half width and height in meters
	snake->move(b2d_to_sf(13.5, -10)); //x value centers rect in middle, y value for lowest value of 20 meter tall window

	//target
	//right + 0.75 or left - 0.75
	//above + 0.75 or below - 0.75
	b2Vec2 target;
	moveTarget(&target);

	//target sf
	RectangleShape* targetRender = new RectangleShape(b2d_to_sf(1,1));
	targetRender->setFillColor(Color::Red);
	targetRender->setOrigin(b2d_to_sf(0.5, 0.5));
	

	//Intro
	//cout << "Welcome to Gravity Snake!\nCollect targets by moving towards them. To win, colect 20 targets.\nUse WASD to move\nPress enter to begin";
	//cin.get();

	//-----------------------------------------------------------------------
	//WINDOW RENDER

	// create the window
	RenderWindow window(VideoMode(810, 600), "Snake");

	bool running = false;
	bool end = false;
	Clock deltaClock;

	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == Event::Closed)
				window.close();
		}

		// clear the window with sky blue color
		window.clear(Color(135, 206, 235));

		//update
		if (running == true && end == false) { //during game
			update(&world, &deltaClock, snakeBody, &target, &jumpFX);
			hitTarget(&target, &score, &pickupFX);

			//edit score text
			scoreString = "Score: " + to_string(score);
			scoreText->setString(scoreString);

			targetRender->setPosition(b2d_to_sf(target.x, target.y));

			//dynamic objs
			snake->setPosition(b2d_to_sf(snakeBody->GetPosition().x, snakeBody->GetPosition().y));

			if (score >= 20) {
				running = false;
				end = true;
			}

			window.draw(*snake);
			window.draw(*targetRender);
		}
		else if(running == false && end == false) { //beginning of game
			window.draw(*introText);
			if (Keyboard::isKeyPressed(Keyboard::Enter)) {
				running = true;
			}
		}
		else if (running == false && end == true) { //end of game
			String endString = "You win!";
			endText->setString(endString);
			FloatRect endBounds = endText->getLocalBounds();
			endText->setOrigin(endBounds.left + endBounds.width / 2, endBounds.top + endBounds.height / 2);
			endText->setPosition(b2d_to_sf(13.5, -10));

			//end text
			window.draw(*endText);
		}

		//draw
		window.draw(*ground);
		window.draw(*leftWall);
		window.draw(*rightWall);
		window.draw(*ceiling);
		window.draw(*scoreText);

		//end the current frame
		window.display();
	}

	delete ground;
	delete snake;
	delete leftWall;
	delete rightWall;
	delete targetRender;
	delete ceiling;
	delete font;
	delete introText;
	delete endText;

	//end
	cout << "Congratulations! You beat the game with a score of: " << score << endl;
}