#include "StudentWorld.h"
#include <string>
#include "Field.h"
#include"Actor.h"
#include "GameConstants.h"
#include<sstream>
#include <cmath>
#include "Compiler.h"
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
StudentWorld::~StudentWorld()
{
	delete compilerForEntrant0;
	delete compilerForEntrant1;
	delete compilerForEntrant2;
	delete compilerForEntrant3;
	cleanUp();
}
int StudentWorld::init()
{
	//uses example from the spec almost exactly
	compilerForEntrant0 = nullptr; compilerForEntrant1 = nullptr; compilerForEntrant2 = nullptr; compilerForEntrant3 = nullptr;
	string error;
	std::vector<std::string> fileNames = getFilenamesOfAntPrograms();
	string fieldFileName;
	Field f;
	std::string fieldFile = getFieldFilename();

	if (f.loadField(fieldFile) == Field::LoadResult::load_fail_bad_format || f.loadField(fieldFile) == Field::LoadResult::load_fail_file_not_found)
		return GWSTATUS_LEVEL_ERROR;
	else if (f.loadField(fieldFile) == Field::LoadResult::load_success)
	{
		for (int k = 0; k < VIEW_WIDTH; k++)
			for (int i = 0; i < VIEW_HEIGHT; i++)
			{
				Field::FieldItem x = f.getContentsOf(k, i);
				//uses a switch statement instead of the if else ladder in the spec but pretty much just copies the same implementation.
				//deteremines what the fieldfile says is at that location k,i since the for loops correspond with the location and allocates
				//an appropriate object at the location k,i 
				switch (x)
				{
				case Field::FieldItem::rock:
					arr[peb].push_back(new Pebble(IID_ROCK, k, i, this, GraphObject::Direction::right, 1, .25));
					break;
				case Field::FieldItem::food:
					arr[foo].push_back(new Food(IID_FOOD, k, i, this, GraphObject::Direction::right, 2, .25, 6000));
					break;
				case Field::FieldItem::grasshopper:
					arr[babyG].push_back(new BabyGrasshopper(IID_BABY_GRASSHOPPER, k, i, this, GraphObject::Direction::right, 1, .25, 500));
					break;
				case Field::FieldItem::poison:
					arr[pois].push_back(new Poison(IID_POISON, k, i, this, GraphObject::Direction::right, 2, .25));
					break;
				case Field::FieldItem::water:
					arr[wat].push_back(new WaterPool(IID_WATER_POOL, k, i, this, GraphObject::Direction::right, 2, .25));
					break;
				case Field::FieldItem::anthill0:
					if (getFilenamesOfAntPrograms().size() > 0)
					{
						compilerForEntrant0 = new Compiler();
						if (!compilerForEntrant0->compile(fileNames[0], error))
						{
							setError(fileNames[0] + " " + error);
							return GWSTATUS_LEVEL_ERROR;
						}
						arr[ant0].push_back(new AntHill(IID_ANT_HILL, k, i, this, GraphObject::Direction::right, 2, .25, 8999, compilerForEntrant0, 0));
					}
					break;
				case Field::FieldItem::anthill1:
					if (fileNames.size() > 1)
					{
						compilerForEntrant1 = new Compiler();
						if (!compilerForEntrant1->compile(fileNames[1], error))
						{
							setError(fileNames[1] + " " + error);
							return GWSTATUS_LEVEL_ERROR;
						}
						arr[ant1].push_back(new AntHill(IID_ANT_HILL, k, i, this, GraphObject::Direction::right, 2, .25, 8999, compilerForEntrant1, 1));
						break;
					}
					break;
				case Field::FieldItem::anthill2:
					if (fileNames.size() > 2)
					{
						compilerForEntrant2 = new Compiler();
						if (!compilerForEntrant2->compile(fileNames[2], error))
						{
							setError(fileNames[2] + " " + error);
							return GWSTATUS_LEVEL_ERROR;
						}
						arr[ant2].push_back(new AntHill(IID_ANT_HILL, k, i, this, GraphObject::Direction::right, 2, .25, 8999, compilerForEntrant2, 2));
						break;

					}
					break;
				case Field::FieldItem::anthill3:
					if (fileNames.size() > 3)
					{
						compilerForEntrant3 = new Compiler();
						if (!compilerForEntrant3->compile(fileNames[3], error))
						{
							setError(fileNames[3] + " " + error);
							return GWSTATUS_LEVEL_ERROR;
						}
						arr[ant3].push_back(new AntHill(IID_ANT_HILL, k, i, this, GraphObject::Direction::right, 2, .25, 8999, compilerForEntrant3, 3));
					}
					break;
				default:
					break;
				}
			}
		return GWSTATUS_CONTINUE_GAME;
	}
	else return GWSTATUS_LEVEL_ERROR;
}

int StudentWorld::move()
{
	while (m_time <=2000)
	{
		for (int k = 0; k < NUM_ELEMENTS; k++)//calls do something for everything that isn't dead 
		{
			std::list<Actor*>::iterator it = arr[k].begin();
			while (it != arr[k].end())
			{
				if (((*it)->isDead()) == false)
					(*it)->doSomething();
				it++;
			}
		}
		for (int k = 1; k < NUM_ELEMENTS; k++)//eliminates everything that isnt dead 
		{
			std::list<Actor*>::iterator it = arr[k].begin();
			while (it != arr[k].end())
			{
				if ((*it)->isDead())
				{
					delete (*it);
					it = arr[k].erase(it);
				}
				else it++;
			}
		}
		setDisplayText();
		m_time++;
		return GWSTATUS_CONTINUE_GAME;
	}
	if (determineWinner())
		return GWSTATUS_PLAYER_WON;
	else
	return GWSTATUS_NO_WINNER;
}

void StudentWorld::setDisplayText()
{/*This function and the following function are properly display the right information at the top of the game
 In the process, it has to keep track of who is the current winner so that we can correctly format the text (include the * next to the winner)
 */
	int ticks = m_time;
	int antsAnt0, antsAnt1, antsAnt2, antsAnt3;
	int currentMax=5;
	antsAnt0 = m_totalcol0;
	antsAnt1 = m_totalCol1;
	antsAnt2 = m_totalcol2;
	antsAnt3 = m_totalcol3;
	
	if ((antsAnt0 > 5) && antsAnt0 > currentMax)
	{
		m_colonyWinner = 0;
		currentMax = antsAnt0;
	}
	if ((antsAnt1 > 5) && antsAnt1 > currentMax)
	{
		m_colonyWinner = 1;
		currentMax = antsAnt1;
	}
	if ((antsAnt2 > 5) && antsAnt2 > currentMax)
	{
		m_colonyWinner = 2;
		currentMax = antsAnt2;
	}
	if ((antsAnt3 > 5) && antsAnt3 > currentMax)
	{
		m_colonyWinner = 3;
		currentMax = antsAnt3;
	}

	string s = someFunctionToFormatThingsNicely(ticks,
		antsAnt0,
		antsAnt1,
		antsAnt2,
		antsAnt3,
		m_colonyWinner
	);
	// Finally, update the display text at the top of the screen with your
	// newly created stats
	setGameStatText(s); // calls our provided GameWorld::setGameStatText

}

std::string StudentWorld::someFunctionToFormatThingsNicely(int ticks, int antsAnt0, int antsAnt1, int antsAnt2, int antsAnt3, int winningAntNumber)
{
	//simple set of if statements to appropriately format text. Rather trivial
	stringstream ss;
	ss << "Ticks:";
	if (2000 - ticks >= 1000)
		ss << " ";
	else if (2000 - ticks >= 100 && 2000 - ticks < 1000)
		ss << "  ";
	else if (2000 - ticks >= 10 && 2000 - ticks < 100)
		ss << "   ";
	else
		ss << "    ";
	ss << 2000 - ticks << " - ";
	if (compilerForEntrant0 != nullptr)
	{
		ss<<compilerForEntrant0->getColonyName();
		if (winningAntNumber == 0)
			ss << "*";
		ss << ": ";
		if (antsAnt0 < 10)
			ss << 0;
		ss << antsAnt0 << "  ";
	}
	if (compilerForEntrant1 != nullptr)
	{
		ss << compilerForEntrant1->getColonyName();
		if (winningAntNumber == 1)
			ss << "*";
		ss << ": ";
		if (antsAnt1 < 10)
			ss << 0;
		ss << antsAnt1 << "  ";
	}
	if (compilerForEntrant2 != nullptr)
	{
		ss << compilerForEntrant2->getColonyName();
		if (winningAntNumber == 2)
			ss << "*";
		ss << ": ";
		if (antsAnt2 < 10)
			ss << 0;
		ss << antsAnt2 << "  ";
	}
	if (compilerForEntrant3 != nullptr)
	{
		ss << compilerForEntrant3->getColonyName();
		if (winningAntNumber == 3)
			ss << "*";
		ss << ": ";
		if (antsAnt3 < 10)
			ss << 0;
		ss << antsAnt3 << "  ";
	}
	return ss.str();
}
bool StudentWorld::determineWinner()
{//takes advantage of our previous function keeping track of the winner to set the winner
	if (m_colonyWinner != -1)
	{
		switch (m_colonyWinner)
		{
		case 0:
			setWinner(compilerForEntrant0->getColonyName());
			return true;
			break;
		case 1:
			setWinner(compilerForEntrant1->getColonyName());
			return true;
			break;
		case 2:
			setWinner(compilerForEntrant2->getColonyName());
			return true;
			break;
		case 3:
			setWinner(compilerForEntrant3->getColonyName());
			return true;
			break;
		}
	}
	return false;
}
void StudentWorld::cleanUp()
{
	for (int k = 0; k < NUM_ELEMENTS; k++)
	{
		list<Actor*>::iterator it = arr[k].begin();
		while (it != arr[k].end())
		{
			delete *it;
			it = arr[k].erase(it);
		}
	}

}

bool StudentWorld::canMoveThere(int x, int y)const
{//just checks if the location is blocked by a pebble or not
	if (x >= VIEW_WIDTH || x <= 0 || y >= VIEW_HEIGHT || y <= 0)
		return false;
	list<Actor*>::const_iterator it = arr[peb].begin();
	while (it != arr[peb].end())
	{
		if ((*it)->getX() == x && (*it)->getY() == y)
			return false;
		it++;
	}
	return true;
}
bool StudentWorld::isOn(int x, int y,int what,bool hill)const
{//checks whether we are on food, anthill, or enemy ants
	//first iterates through the array list that contains all the food objects checking for if its food
	list<Actor*>::const_iterator it;
	if (what == foo)
	{
		list<Actor*>::const_iterator it = arr[foo].begin();
		while (it != arr[foo].end())
		{
			if ((*it)->getX() == x && (*it)->getY() == y)
				return true;
			it++;
		}
		return false;
	}
	//this is for if youre looking for enemies first iterates through list that holds all of the grass hoppers baby and adult
	else if (hill == false && (what == ant0 || what == ant1 || what == ant2 || what == ant3))
	{
		for (int k = babyG; k <= 3; k++)
		{
			list<Actor*>::const_iterator it = arr[k].begin();
			while (it != arr[k].end())
			{
				if ((*it)->isDead() == false && (*it)->getX() == x && (*it)->getY() == y)
					return true;
				it++;
			}
		}
		//the iterates throuh lists containing all the ants
		for (int k = ant0; k <= 9; k++)
		{
			if (k == what&&what == ant3)
				return false;
			else if (k == what)//this is to skip ants that are of your same colony because theyre not considered enemies
				k++;
			list<Actor*>::const_iterator it = arr[k].begin();
			if (arr[k].size()!=0&&(*it)->whatAmI()=="AntHill")
				it++;
			while (it != arr[k].end())
			{
				if ((*it)->isDead() == false && (*it)->getX() == x && (*it)->getY() == y)
					return true;
				it++;
			}
		}
	}
	//this is if youre looking for if youre on an anthill or not
	else if (hill == true&&(what == ant0 || what == ant1 || what == ant2 || what == ant3))
	{	
		it = arr[what].begin();
		return((*it)->whatAmI()=="AntHill"&&(*it)->getX() == x && (*it)->getY() == y);
	}
	
	else return false;
}

bool StudentWorld::whereIsPheromone(int x, int y, int which)const
{//iterates through list of actors pointing to pheremones and sees if youre on that spot
	list<Actor*>::const_iterator it = arr[which].begin();
	while (it != arr[which].end())
	{
		if ((*it)->getX() == x && (*it)->getY() == y)
			return true;
		it++;
	}
	return false;
}
bool StudentWorld::whereIsPoison(int x, int y)const
{//does the same as above but this time looking for if youre on poison by going though array that holds list of actors to poison
	list<Actor*>::const_iterator it = arr[pois].begin();
	while (it != arr[pois].end())
	{
		if ((*it)->getX() == x && (*it)->getY() == y)
			return true;
		it++;
	}
	return false;
}
int StudentWorld::removeAmountFood(int x, int y,int amount)
{//first looks for if there is any food on that spot
	int returnVal = 0;
	list<Actor*>::iterator it = arr[foo].begin();
	while (it != arr[foo].end())
	{
		if ((*it)->getX() == x && (*it)->getY() == y)
			break;
		it++;
	}
	//if there is food there
	if (it != arr[foo].end())
	{
		if (static_cast<EnergyHolder*>(*it)->numHealth() <= amount)//remove the amount of food, or however much you can if there isnt enough to remove the desired amount
		{
			returnVal = (static_cast<EnergyHolder*>(*it)->numHealth());
			static_cast<EnergyHolder*>(*it)->decreaseHealthBy(static_cast<EnergyHolder*>(*it)->numHealth());
			
		}
		else
		{
			returnVal = amount;
			static_cast<EnergyHolder*>(*it)->decreaseHealthBy(amount);
		}
		if (static_cast<EnergyHolder*>(*it)->numHealth() <= 0)
		{
			(*it)->setDead();
		}
			
	}
	return returnVal;
}

void StudentWorld::allocateNewFood(int x, int y,int amount) 
{//checks to see if there is food there already and adds more health to that food object, otherwise allocates a new food object
	if (amount <= 0)
		return;
	list<Actor*>::iterator it = arr[foo].begin();
	while (it != arr[foo].end())
	{
		if ((*it)->getX() == x && (*it)->getY() == y)
		{
			static_cast<EnergyHolder*>(*it)->increaseHealthBy(amount);
			return;
		}
		it++;
	}
	if(it==arr[foo].end())
		arr[foo].push_back(new Food(IID_FOOD, x, y, this, GraphObject::Direction::right, 2, .25, amount));
}

void StudentWorld::allocateNewAdultGrassHopper(int x, int y) { arr[3].push_back(new GrassHopper(IID_ADULT_GRASSHOPPER, x, y, this, GraphObject::Direction::right, 1, .25, 1600)); }

void StudentWorld::allocateNewAnt(int x, int y, int col, Compiler* a)
{//self explanatory- just determines what colony of ant to allocate and allocates a new ant object adding it to the structure
	switch (col)
	{
	case 0:
		arr[ant0].push_back(new Ant(IID_ANT_TYPE0, x, y, this, GraphObject::Direction::right, 1, .25, 1500, a, col));
		break;
	case 1:
		arr[ant1].push_back(new Ant(IID_ANT_TYPE1, x, y, this, GraphObject::Direction::right, 1, .25, 1500, a, col));
		break;
	case 2:
		arr[ant2].push_back(new Ant(IID_ANT_TYPE2, x, y, this, GraphObject::Direction::right, 1, .25, 1500, a, col));
		break;
	case 3:
		arr[ant3].push_back(new Ant(IID_ANT_TYPE3, x, y, this, GraphObject::Direction::right, 1, .25, 1500, a, col));
		break;
	}
}

void StudentWorld::giveMeAllToPoison(int x, int y)
{//goes through baby grasshopper actor pointer list to see if you should poison anything 
	list<Actor*>::iterator it = arr[babyG].begin();
	while (it != arr[babyG].end())
	{
		if ((*it)->getX() == x && (*it)->getY() == y)
			static_cast<EnergyHolder*>(*it)->decreaseHealthBy(150);
		if (static_cast<EnergyHolder*>(*it)->numHealth() <= 0)
		{
			(*it)->setDead();
			allocateNewFood((*it)->getX(), (*it)->getY(), 100);
		}
		it++;
	}
	for (int k = ant0; k <= 9; k++)//then goes through each of the ant colony checking to see if you should poison any
	{
		list<Actor*>::iterator it1 = arr[k].begin();
		while (it1 != arr[k].end())
		{
			if ((*it1)->getX() == x && (*it1)->getY() == y)
				static_cast<EnergyHolder*>(*it1)->decreaseHealthBy(150);
			if (static_cast<EnergyHolder*>(*it1)->numHealth() <= 0)
			{
				(*it1)->setDead();
				allocateNewFood((*it1)->getX(), (*it1)->getY(), 100);
			}
			it1++;
		}
	}
}

void StudentWorld::giveMeAllToStun(int x, int y)
{//goes through all the list of baby grasshopper and stuns the right one(s)
	list<Actor*>::iterator it = arr[babyG].begin();
	while (it != arr[babyG].end())
	{
		if((*it)->getX() ==x &&(*it)->getY() == y)
		if (!(static_cast<Mover*>(*it)->wasStunnedByWater()))
		{
			static_cast<Mover*>(*it)->incrementStunBy(2);
			static_cast<Mover*>(*it)->setWasStunnedByWater(true);
		}
		it++;
	}
	for (int k = ant0; k <= 9; k++)//goes throug the ant lists and stuns the apppropriate ones
	{
		list<Actor*>::iterator it1 = arr[k].begin();
		while (it1 != arr[k].end())
		{
			if ((*it1)->getX() == x && (*it1)->getY() == y)
				if (!(static_cast<Mover*>(*it1)->wasStunnedByWater()))
				{
					static_cast<Mover*>(*it1)->incrementStunBy(2);
					static_cast<Mover*>(*it1)->setWasStunnedByWater(true);
				}
			it1++;
		}
	}
}

bool StudentWorld::Bite(int x, int y, Actor* a, int dmg,int ignore)
{
	list<Actor*> temp;
	int numTemp = 0;
	for (int k = babyG; k <= 3; k++)//looks for a valid target to bite and pushes it onto a temp list
	{
		list<Actor*>::iterator it = arr[k].begin();
		while (it != arr[k].end())
		{
			if ((*it) != a && (*it)->isDead() == false && (*it)->getX() == x && (*it)->getY() == y)
			{
				temp.push_back((*it));
				numTemp++;
			}
			it++;
		}
	}
	for (int k = ant0; k <= 9; k++)//looking for valid targets to bite and pushing them onto a vector
	{
		if (k == ignore&&ignore == ant3)
			break;
		else if (k == ignore)//skips the ants of the same colony
			k++;
		list<Actor*>::iterator it = arr[k].begin();
		if (arr[k].size() != 0)
			it++;
		while (it != arr[k].end())
		{
			if ((*it) != a && (*it)->isDead() == false && (*it)->getX() == x && (*it)->getY() == y)
			{
				temp.push_back((*it));
				numTemp++;
			}
			it++;
		}
	}
	if (numTemp != 0)
	{
		int random = randInt(0, numTemp - 1);//randomly picks a number
		list<Actor*>::iterator it = temp.begin();
		for (int k = 0; k < random; k++)//moves the pointer to the bitable object at that number in the list
			it++;
		static_cast<EnergyHolder*>(*it)->decreaseHealthBy(dmg);//bites it
		if ((*it)->whatAmI() == "Ant")
		{
			static_cast<Ant*>(*it)->setWasBitten(true);//ants have to know if theyre bitten
		}
		if (static_cast<EnergyHolder*>(*it)->numHealth() <= 0)
		{
			(*it)->setDead();
			allocateNewFood((*it)->getX(), (*it)->getY(), 100);
		}
		else if (percentageGenerator(2) && (*it)->whatAmI() == "Adult GrassHopper")//adult grasshoppers can choose to bite back
		{
			static_cast<GrassHopper*>(*it)->gBite((*it),0);
		}
		return true;
	}
	return false;
}

bool StudentWorld::percentageGenerator(int b)//if b==2 then theres 1/2 chance and if b ==10 theres 1/10 chance
{ 
	int x = randInt(1, b);
	if (x == 1)
		return true;
	return false;
	
}

void StudentWorld::allocateNewPheromone(int x, int y,int whos)
{
	//finds out which pheremone to make
	int z = -1;
	switch (whos)
	{
	case 0:
		z = pher0;
		break;
	case 1:
		z = pher1;
		break;
	case 2:
		z = pher2;
		break;
	case 3:
		z = pher3;
		break;
	default:
		return;
	}
	list<Actor*>::iterator it = arr[z].begin();
	while (it != arr[z].end())
	{
		if ((*it)->getX() == x && (*it)->getY() == y)
		{
			if (static_cast<EnergyHolder*>(*it)->numHealth() <= 512)
				static_cast<EnergyHolder*>(*it)->increaseHealthBy(256);
			else
				static_cast<EnergyHolder*>(*it)->increaseHealthBy((768 - (static_cast<EnergyHolder*>(*it)->numHealth())));
			return;
		}
		it++;
	}
	if (it == arr[z].end())
		arr[z].push_back(new Pheromone(IID_PHEROMONE_TYPE0+whos, x, y, this, GraphObject::Direction::right, 2, .25, 256));
}

void StudentWorld::incrementTotalNumberAnts(int colnum)
{
	switch (colnum)
	{
	case 0:
		m_totalcol0++;
		break;
	case 1:
		m_totalCol1++;
		break;
	case 2:
		m_totalcol2++;
		break;
	case 3:
		m_totalcol3++;
		break;
	}
}
bool StudentWorld::canJump(int x, int y)const//checks if at least one square is open in the circle around the grasshopper
{
	for (int k = 1; k <= 10; k++)
	{
		for (int i = 0; i < 360; i++)
		{
			double radian = i*(3.1415926535 / 180);
			int xspace = x + k*cos(radian);
			int yspace = y + k*sin(radian);
			if (xspace != x && yspace != y)
			if (canMoveThere(xspace, yspace))
				return true;
		}
	}
	return false;
}