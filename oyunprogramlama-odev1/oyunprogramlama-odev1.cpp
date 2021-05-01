#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

string balls[]{ "ball_blue","ball_green","ball_orange","ball_red","ball_silver","ball_yellow" };
string bats[]{ "bat_black","bat_blue","bat_orange","bat_pink","bat_yellow" };
string walls[]{ "brick_blue","brick","brick_pink_side","brick_red" }; // 256x256, 43.5x43.5
string brickNames[]{ "brick_blue_small","brick_green_small","brick_pink_small","brick_violet_small",
"brick_yellow_small" };
string crackedBrickNames[]{ "brick_blue_small_cracked","brick_green_small_cracked","brick_pink_small_cracked",
	"brick_violet_small_cracked","brick_yellow_small_cracked" };

const int wallCountX = 14, wallCountY = 20;
const float wallSize = 43.5;//fullsize, square
const float screenWidth = wallSize * wallCountX, screenHeight = wallSize * wallCountY;
const float paddleSpeed = 10.0f;
const int blockCountX = 5, blockCountY = 5;
float brickOffsetX, brickOffsetY = 15;
float dX = 6, dY = -5; // ball speed
bool gameStarted = false;
class Brick
{
	public:
	Sprite sprite;
	int brickIndex;
	int live = 2;
	bool hittable = true;
	bool isHitted = false;
};
class Paddle
{
	private:
	void SetTextureRect(IntRect ir, Vector2f scaleFactor)
	{
		sprite.setTextureRect(ir);
		sprite.setOrigin(ir.width / 2, ir.height / 2);
		sprite.setScale(scaleFactor);
		width = sprite.getGlobalBounds().width;
		height = sprite.getGlobalBounds().height;
		sprite.setPosition(screenWidth / 2, screenHeight - 100);
	}
	public:
	Sprite sprite;
	float x, y;
	float width, height;
	float speed = 6.0f;
	Paddle(Texture* t, IntRect ir, Vector2f scaleFactor)
	{
		sprite.setTexture(*t);
		SetTextureRect(ir, scaleFactor);

	}
	void Update()
	{
		if (Keyboard::isKeyPressed(Keyboard::Right)) sprite.move(speed, 0);
		if (Keyboard::isKeyPressed(Keyboard::Left)) sprite.move(-speed, 0);
		x = sprite.getPosition().x;
		y = sprite.getPosition().y;
		if (x < wallSize + (width / 2)) sprite.setPosition(wallSize + (width / 2), y);
		if (x > screenWidth - (wallSize + width / 2)) sprite.setPosition(screenWidth - wallSize - (width / 2), y);
	}
};
class Ball
{
	private:
	void SetTextureRect(IntRect ir, Vector2f scaleFactor)
	{
		sprite.setTextureRect(ir);
		sprite.setOrigin(ir.width / 2, ir.height / 2);
		sprite.setScale(scaleFactor);
		width = sprite.getGlobalBounds().width;
		height = sprite.getGlobalBounds().height;
		sprite.setPosition(screenWidth / 2, screenHeight - 200);
		x = sprite.getPosition().x;
		y = sprite.getPosition().y;
	}
	public:
	Sprite sprite;
	float x, y;
	float width, height;
	float speed = 6.0f;
	bool canMove = false;
	Ball(Texture* t, IntRect ir, Vector2f scaleFactor)
	{
		sprite.setTexture(*t);
		SetTextureRect(ir, scaleFactor);
	}
	void Update()
	{
		sprite.setPosition(x, y);
		x = sprite.getPosition().x;
		y = sprite.getPosition().y;
		if (x - 16 <= wallSize || x + 16 >= screenWidth - wallSize) dX = -dX;
		if (y - 16 <= wallSize || y + 16 >= screenHeight - wallSize) dY = -dY;

	}
};
vector<Sprite> WallInitializer(vector<Texture>& wallTextures)
{
	vector<Sprite> wt;
	for (size_t i = 0; i < wallCountX; i++)
	{
		for (size_t j = 0; j < wallCountY; j++)
		{
			if (j == 0 || j == wallCountY - 1 || i == 0 || i == wallCountX - 1)
			{
				int randomWallIndex = rand() % 4;
				Sprite tempWallSprite;
				tempWallSprite.setTexture(wallTextures.at(randomWallIndex));
				tempWallSprite.setTextureRect(IntRect(43, 45, 174, 174));
				tempWallSprite.setOrigin(87, 87);
				Vector2f scaleFactor(.25f, .25f);
				tempWallSprite.setScale(scaleFactor);//43.5 x 43.5	
				Vector2f pos((i * 174 / 4) + 174 / 8, (j * 174 / 4) + 174 / 8);
				tempWallSprite.setPosition(pos);
				wt.push_back(tempWallSprite);
			}
		}
	}
	return wt;
}
vector<Brick> BrickInitializer(vector<Texture>& brickTextures, int x, int y)
{
	float emptyAreaWidth = screenWidth - (2 * wallSize);
	float emptyAreaWithoutBricks = emptyAreaWidth - (x* 57.75);// 231/4 =57.75
	float idealOffset = emptyAreaWithoutBricks / (x + 1);
	brickOffsetX = idealOffset;
	vector<Brick> brickList;
	for (size_t i = 0; i < x; i++)
	{
		for (size_t j = 0; j < y; j++)
		{
			int randomBrickIndex = rand() % 5;
			Brick tempBrick;
			tempBrick.sprite.setTexture(brickTextures.at(randomBrickIndex));
			tempBrick.sprite.setTextureRect(IntRect(141, 178, 231, 101));
			tempBrick.sprite.setOrigin(115.5, 50.5);
			tempBrick.sprite.setScale(.25f, .125f);
			tempBrick.sprite.setPosition((i * 57.25) + wallSize + (brickOffsetX * (i+1)) + (57.25 / 2), 
				(j * 25.25f) + wallSize + (brickOffsetY * (j+1)) + (57.25 / 2));
			tempBrick.brickIndex = randomBrickIndex;
			brickList.push_back(tempBrick);
		}
	}
	return brickList;
}
void BallBrickCollision(vector<Brick> &brickList, Ball &ball, vector<Texture> &crackedBrickTextures)
{
	ball.x += dX;
	//check for if ball collide
	for (int i = 0; i < brickList.size(); i++)
	{
		if (brickList[i].hittable)
		{
			if (FloatRect(ball.x - 8, ball.y - 8, 15, 15).intersects(brickList[i].sprite.getGlobalBounds()))
			{
				brickList[i].live--;
				if (brickList[i].live <= 0)
					brickList[i].sprite.setPosition(-100, 0);
				else
				{
					brickList[i].sprite.setTexture(crackedBrickTextures.at(brickList[i].brickIndex));
					brickList[i].sprite.setTextureRect(IntRect(140, 177, 230, 102));
				}
				dX = -dX;
				brickList[i].hittable = false;
			}
		}
	}
	ball.y += dY;
	//check for if ball collide
	for (int i = 0; i < brickList.size(); i++)
	{
		if (brickList[i].hittable)
		{
			if (FloatRect(ball.x - 8, ball.y - 8, 15, 15).intersects(brickList[i].sprite.getGlobalBounds()))
			{
				brickList[i].live--;
				if (brickList[i].live <= 0)
					brickList[i].sprite.setPosition(-100, 0);
				else
				{
					brickList[i].sprite.setTexture(crackedBrickTextures[brickList[i].brickIndex]);
					brickList[i].sprite.setTextureRect(IntRect(140, 177, 230, 102));
				}
				dY = -dY;
				brickList[i].hittable = false;
			}
		}
	}
	for (int i = 0; i < brickList.size(); i++)
	{
		if (!FloatRect(ball.x - 8, ball.y - 8, 20, 20).intersects(brickList[i].sprite.getGlobalBounds()))
		{
			brickList[i].hittable = true;
		}
	}
}
void BallPaddleCollision(Paddle &paddle, Ball &ball)
{
	if (FloatRect(ball.x - 8, ball.y - 8, 15, 15).intersects(paddle.sprite.getGlobalBounds())) dY = -dY;

}
int main()
{
	srand((unsigned int)time(NULL));
	//GAME SETTINGS
	RenderWindow window(VideoMode(wallSize * wallCountX, wallSize * wallCountY), "SFML!");
	window.setFramerateLimit(60);
	float  ballPosX = 400, ballPosY = 500;
	//TEXTURE LOADING START
	//Walls
	vector<Texture> wallTextures;
	for (size_t i = 0; i < 4; i++)
	{
		Texture tempTexture;
		tempTexture.loadFromFile("./resimler/Walls/" + walls[i] + ".png");
		wallTextures.push_back(tempTexture);
	}
	//Bricks
	vector<Texture> brickTextures, crackedBrickTextures;
	for (size_t i = 0; i < 5; i++)
	{
		Texture tempTexture;
		tempTexture.loadFromFile("./resimler/Bricks/" + brickNames[i] + ".png");
		brickTextures.push_back(tempTexture);
		tempTexture.loadFromFile("./resimler/Bricks/" + crackedBrickNames[i] + ".png");
		crackedBrickTextures.push_back(tempTexture);
	}
	//Ball
	Texture tBall;
	int randomBallIndex = rand() % 6;
	tBall.loadFromFile("./resimler/Balls/" + balls[randomBallIndex] + ".png");
	Ball ball(&tBall, IntRect(72, 76, 366, 366), Vector2f(.0625f, .0625f));
	//Paddle
	Texture tPaddle;
	int randomBatIndex = rand() % 5;
	tPaddle.loadFromFile("./resimler/Bats/" + bats[randomBatIndex] + ".png");
	Paddle paddle(&tPaddle, IntRect(24, 180, 464, 102), Vector2f(.125f, .125f));
	// TEXTURE LOADING END
	// 
	//INIALIZING GAMEOBJECTS
	//Walls
	vector<Sprite> wallList = WallInitializer(wallTextures);
	// Bricks
	vector<Brick> brickList = BrickInitializer(brickTextures, blockCountX, blockCountY);
	//
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
			if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) window.close();
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::Space)) gameStarted = true;

		if (gameStarted)
		{
			//check ball-brick collision
			BallBrickCollision(brickList, ball, crackedBrickTextures);
			//check ball-paddle collision
			BallPaddleCollision(paddle, ball);
		}
	

		//update movements
		paddle.Update();
		ball.Update();

		//Draw start
		window.clear();
		window.draw(paddle.sprite);
		window.draw(ball.sprite);
		//draw walls
		for (size_t i = 0; i < wallList.size(); i++)
		{
			window.draw(wallList[i]);
		}
		//draw bricks
		for (int i = 0; i < brickList.size(); i++)
		{
			window.draw(brickList[i].sprite);
		}
		window.display();
		//Draw end
	}
	return 0;
}
