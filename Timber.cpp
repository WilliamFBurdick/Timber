#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>

using namespace sf;

void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

struct Cloud {
	bool active;
	Sprite sprite;
	float speed;
};

int main()
{
	//Set video mode
	VideoMode vm(1920, 1080);
	//Initialize window object
	RenderWindow window(vm, "Timber!!!", Style::Default);
	
	//Create textures
		//Background
	Texture textureBackground;
	textureBackground.loadFromFile("graphics/background.png");
		//Tree
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
		//Bee
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Texture textureCloud;
		//Cloud
	textureCloud.loadFromFile("graphics/cloud.png");
	//Use textures to create sprites
		//Background
	Sprite spriteBackground;
	spriteBackground.setTexture(textureBackground);
		//Tree
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
		//Bee
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
		//Clouds
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);
	//Assign sprites' positions
	spriteBackground.setPosition(0, 0);
	spriteTree.setPosition(810, 0);
	spriteBee.setPosition(0, 800);
	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 250);
	spriteCloud3.setPosition(0, 500);
	
	//Assign actor variables
		//Bee
	bool beeActive = false;
	float beeSpeed = 0.0f;
		//Clouds
	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;
	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	//Time variables
	Clock clock;
	bool paused = true;
	//Timer bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);
	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	//UI Variables
	int score = 0;
	Text messageText;
	Text scoreText;
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");
	messageText.setFont(font);
	scoreText.setFont(font);
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);
	//Adjust the origin of the message text so it can be placed directly in the window's center
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);

	//Prepare the branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");
	for (int i = 0; i < NUM_BRANCHES; i++)
	{
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		branches[i].setOrigin(220, 20);
	}

	//Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);
	side playerSide = side::LEFT;
	//Prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);
	//Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);
	//Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;
	//Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	bool acceptInput = false;
	//Prepare the sound effects
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);
	while (window.isOpen())
	{
		/* Handle player input */
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
			{
				acceptInput = true;
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
		//Begin game
		if (Keyboard::isKeyPressed(Keyboard::Return))
		{
			paused = false;
			score = 0;
			timeRemaining = 6;

			for (int i = 1; i < NUM_BRANCHES; i++) branchPositions[i] = side::NONE;
			spriteRIP.setPosition(675, 2000);
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}
		//Handle the player's control input (only if accepting input)
		if (acceptInput)
		{
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				playerSide = side::RIGHT;
				score++;
				timeRemaining += (2 / score) + .15;
				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);
				updateBranches(score);
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;
				acceptInput = false;
				chop.play();
			}

			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				playerSide = side::LEFT;
				score++;
				timeRemaining += (2 / score) + .15;
				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);
				updateBranches(score);
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;
				chop.play();
			}
		}

		/* Update the scene */
		if (!paused)
		{
			//Measure time
			Time dt = clock.restart();
			//Reduce time remaining
			timeRemaining -= dt.asSeconds();
			//Adjust time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));
			//Check if game is over
			if (timeRemaining <= 0.0f)
			{
				paused = true;
				messageText.setString("Game Over!");
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
				outOfTime.play();
			}
			//Set up the bee
			if (!beeActive)
			{
				//randomize speed
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 200;
				//randomize height
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;
			}
			else
			{
				spriteBee.setPosition(spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()), spriteBee.getPosition().y);
				//check if bee reached end of screen
				if (spriteBee.getPosition().x < -100) beeActive = false;
			}
			//Manage the clouds
			//Cloud 1
			if (!cloud1Active)
			{
				//randomize speed
				srand((int)time(0) * 10);
				cloud1Speed = (rand() % 200);
				//randomize height
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-200, height);
				cloud1Active = true;
			}
			else
			{
				spriteCloud1.setPosition(spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()), spriteCloud1.getPosition().y);
				//check if cloud1 reached end of screen
				if (spriteCloud1.getPosition().x > 1920) cloud1Active = false;
			}
			//Cloud 2
			if (!cloud2Active)
			{
				//randomize speed
				srand((int)time(0) * 20);
				cloud2Speed = (rand() % 200);
				//randomize height
				srand((int)time(0) * 20);
				float height = (rand() % 300) - 150;
				spriteCloud2.setPosition(-200, height);
				cloud2Active = true;
			}
			else
			{
				spriteCloud2.setPosition(spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()), spriteCloud2.getPosition().y);
				//check if cloud1 reached end of screen
				if (spriteCloud2.getPosition().x > 1920) cloud2Active = false;
			}
			//Cloud 3
			if (!cloud3Active)
			{
				//randomize speed
				srand((int)time(0) * 30);
				cloud3Speed = (rand() % 200);
				//randomize height
				srand((int)time(0) * 30);
				float height = (rand() % 450) - 150;
				spriteCloud3.setPosition(-200, height);
				cloud3Active = true;
			}
			else
			{
				spriteCloud3.setPosition(spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()), spriteCloud3.getPosition().y);
				//check if cloud1 reached end of screen
				if (spriteCloud3.getPosition().x > 1920) cloud3Active = false;
			}

			//Update the score
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			//Update the branches
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				float height = i * 150;
				switch (branchPositions[i]) {
				case side::LEFT:
					branches[i].setPosition(610, height);
					branches[i].setRotation(180);
					break;
				case side::RIGHT:
					branches[i].setPosition(1330, height);
					branches[i].setRotation(0);
					break;
				default:
					branches[i].setPosition(3000, height);
					break;
				}
			}
			if (logActive)
			{
				spriteLog.setPosition(spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()), spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()));
				if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000)
				{
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			if (branchPositions[5] == playerSide)
			{
				death.play();
				//the player has died
				paused = true;
				acceptInput = false;
				//draw the grave sprite
				spriteRIP.setPosition(525, 760);
				spritePlayer.setPosition(2000, 660);
				messageText.setString("RIP!!");
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
			}
		}


		/*Draw the scene */
		//Clear everything from previous frame
		window.clear();
		//Draw game scene
		window.draw(spriteBackground);
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);
		//Draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++) window.draw(branches[i]);

		window.draw(spriteTree);

		window.draw(spritePlayer);
		window.draw(spriteAxe);
		window.draw(spriteLog);
		window.draw(spriteRIP);

		window.draw(spriteBee);

		//Draw the UI
		window.draw(timeBar);
		window.draw(scoreText);
		if (paused) window.draw(messageText);

		//Display what was drawn
		window.display();
		
	}
	return 0;
}

void updateBranches(int seed)
{
	//Move the branches down the tree
	for (int j = NUM_BRANCHES - 1; j > 0; j--) branchPositions[j] = branchPositions[j - 1];
	//Create a new branch at the top of the tree
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
		break;
	}
}
