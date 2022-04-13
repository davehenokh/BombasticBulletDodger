#include "game.h"
#include <chrono>
#include <fstream>
#include <cstdlib>

//detect keyboard input function
int kbhit()
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
    ungetc(ch, stdin);
    return 1;
    }

    return 0;
}

//functions involving the Bullets

/*
constructed Bullets should be have its starting position at the edge of the map

*/
void Bullet::Move()
{
    this->x += this->V_x;
    this->y += this->V_y;
}

void Bullet::init(int Mapsize_x,int Mapsize_y)
{
    int randNum = rand()%4 + 1;
    if (randNum == 1){
      this->x = 0;
      this->y = rand() % Mapsize_y;
      this->V_x = 1;
      this->V_y = rand()%3 - 1;
    }

    if (randNum == 2){
      this->x = Mapsize_x - 1;
      this->y = rand() % Mapsize_y;
      this->V_x = -1;
      this->V_y = rand()%3 - 1;
    }

    if (randNum == 3){
      this->x = rand() % Mapsize_x;
      this->y = 0;
      this->V_x = rand()%3 - 1;
      this->V_y = 1;
    }

    if (randNum == 4){
      this->x = rand() % Mapsize_x;
      this->y = Mapsize_y - 1;
      this->V_x = rand()%3 - 1;
      this->V_y = -1;
    }

    this->Size = rand() % 2 + 1;
}

int Bullet::getBulletPos_x()
{
    return this->x;
}

int Bullet::getBulletPos_y()
{
    return this->y;
}

int Bullet::getBulletSize()
{
    return this->Size;
}


//retrieve the x coordinate of the player
int Player::getPlayerPos_x()
{
    return this->x;
}

//retrieve the y coordinate of the player
int Player::getPlayerPos_y()
{
    return this->y;
}

//detect Keypressing and change the player's coordinate
void Player::Move(int Mapsize_x, int Mapsize_y)
{
    if(kbhit())
    {
        char c = getchar();
        if(this->y > 0 && (c == 'A' || c == 'a'))this->y--;
        if(this->y < Mapsize_y - 1 && (c == 'D' || c == 'd'))this->y++;
        if(this->x > 0 && (c == 'W' || c == 'w'))this->x--;
        if(this->x < Mapsize_x - 1 && (c == 'S' || c == 's'))this->x++;
    }
}

void Player::init()
{
    this->x = 5;
    this->y = 10;
    this->HP = 3;
    this->Clock = 0;
}
//Private functions

//Initializing functions

/*
Initialize the map size
*/
void Game::initMap()
{
    this->MapSize_x = 10;
    this->MapSize_y = 20;
}

/*
    Initializing:
        the player elements
        the bullets elements
        UI elements
*/
void Game::initVariables()
{
    this->gameOver = false;

    //subject to change
    this->player.init();

    this->bullets.clear();

    this->maxBulletsNumber = 5;
}


//constructor / destructor
Game::Game()
{
    this->initMap();
    this->initVariables();
}

//functions

//---------------------------------------------------------------------------RUNNING------------------------------------------------------------------------------------------//
bool Game::isRunning()
{
    return !gameOver;
}

//---------------------------------------------------------------------------UPDATING STATUS------------------------------------------------------------------------------------------//

void Game::updateMap()
{
    //reseting the map
    for(int i = 0; i < this->MapSize_x; i++)
    {
        for(int j = 0; j < this->MapSize_y; j++)
        {
            this->Map[i][j]='.';
        }
    }

    //updating the positions of all bullets in the map
    for(unsigned int i = 0; i < this->bullets.size(); i++)
    {
        int sz = this->bullets[i].getBulletSize();
        int x = this->bullets[i].getBulletPos_x(), y = this->bullets[i].getBulletPos_y();

        for(int j = 0; j < sz; j++)
            for(int k = 0; k < sz; k++)
                if(x + j >= 0 && x + j < this->MapSize_x && y + k >= 0 && y + k < this->MapSize_y)
                    this->Map[x + j][y + k] = '*';
    }

    //checking for player's collision with bullets
    if(this->Map[this->player.getPlayerPos_x()][this->player.getPlayerPos_y()] == '*')
    {
        this->player.HP--;
        if(player.HP <= 0)this->gameOver = true;
    }
    //updating the position of the player
    this->Map[this->player.getPlayerPos_x()][this->player.getPlayerPos_y()] = '@';
}

void Game::updatePlayerStatus()
{
    player.Move(this->MapSize_x, this->MapSize_y);
}

void Game::updateBulletStatus()
{
    //Moving all the current bullet
    for(unsigned int i = 0; i < this->bullets.size();)
    {
        this->bullets[i].Move();


        int x = this->bullets[i].getBulletPos_x(), y = this->bullets[i].getBulletPos_y(), sz=this->bullets[i].getBulletSize();
        //check if the bullet is outside the map, delete it if so
        if(x >= this->MapSize_x || y >= this->MapSize_y || x + sz -1 < 0 || y + sz - 1 < 0)
            this->bullets.erase(bullets.begin() + i);
        else i++;
    }

    // creating a new bullets by chance
    if(this->maxBulletsNumber > this->bullets.size() && rand() % 5 == 0)
    {
        Bullet newBullet;
        newBullet.init(this->MapSize_x, this->MapSize_y);
        this->bullets.push_back(newBullet);
    }

}

void Game::update()
{
    updateBulletStatus();
    updatePlayerStatus();
    updateMap();
}

//---------------------------------------------------------------------------RENDERING------------------------------------------------------------------------------------------//
void Game::renderUI()
{
    std::cout << "Health: " << this->player.HP << "\n";
}

void Game::renderMap()
{
    for(int i = 0; i < this->MapSize_x; i++)
    {
        for(int j = 0; j < this->MapSize_y; j++)
        {
            std::cout << this->Map[i][j];
        }
        std::cout << "\n";
    }
    std::cout<<this->bullets.size()<<"\n";
}

void Game::render()
{
    renderUI();
    renderMap();
}

//---------------------------------------------------------------------------SCOREBOARD------------------------------------------------------------------------------------------//
void Game::printHighestScore()
{

	std::ifstream fin ("Scoreboard.txt");

    std::cout << "+-------------------------------------------+" << "\n";
    int x, highest = -1;
    while (fin >> x) {
        if (x > highest)
            highest = x;
    }
    fin.close();

    if (highest <= 9)
    std::cout << "|  Longest time a player survived: 0" << highest << " secs |" << "\n";
    else if (highest > 9)
    std::cout << "|  Longest time a player survived:  " << highest << " secs |" << "\n";
    std::cout << "+-------------------------------------------+" << "\n";

}

void Game::storeScore(int duration)
{

	std::ofstream fout("Scoreboard.txt", std::ios::app);
    this->player.Clock = duration;
	fout << this->player.Clock << "\n";
    fout.close();
}