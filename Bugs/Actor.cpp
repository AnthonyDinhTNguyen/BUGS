#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include"Field.h"
#include <iostream>
#include<list>
void:: Mover::setNewDirection()
{
	int z = randInt(1, 4);
	switch (z)
	{
	case 1:
		setDirection(GraphObject::Direction::up);
		break;
	case 2:
		setDirection(GraphObject::Direction::right);
		break;
	case 3:
		setDirection(GraphObject::Direction::down);
		break;
	case 4:
		setDirection(GraphObject::Direction::left);
		break;
	}
}

bool Mover::canMoveThere(int x, int y)const
{
	return getWorld()->canMoveThere(x, y);
}

bool Mover::moveNow()//moves the actor
{
	if (getDirection() == GraphObject::Direction::up)
	{
		if (canMoveThere(getX(), getY() + 1) && (getY() + 1) < VIEW_HEIGHT)
		{
			moveTo(getX(), getY() + 1);
			if (wasStunnedByWater() == true)//because if you were stunned by water, if you move off, you can not be restunned by that pool of water
				setWasStunnedByWater(false);
			return true;
		}
	}
	else if (getDirection() == GraphObject::Direction::right)
	{
		if (canMoveThere(getX() + 1, getY()) && (getX() + 1) < VIEW_WIDTH)
		{
			moveTo(getX() + 1, getY());
			if (wasStunnedByWater() == true)
				setWasStunnedByWater(false);
			return true;
		}
	}
	else if (getDirection() == GraphObject::Direction::down)
	{
		if (canMoveThere(getX(), getY() - 1) && (getY() - 1) > 0)
		{
			moveTo(getX(), getY() - 1);
			if (wasStunnedByWater() == true)
				setWasStunnedByWater(false);
			return true;
		}
	}
	else if (getDirection() == GraphObject::Direction::left)
	{
		if (canMoveThere(getX() - 1, getY()) && (getX() - 1) > 0)
		{
			moveTo(getX() - 1, getY());
			if (wasStunnedByWater() == true)
				setWasStunnedByWater(false);
			return true;
		}
	}
	return false;
}

void EnergyHolder::eatFood(int amount)
{
	int x=getWorld()->removeAmountFood(getX(), getY(), amount);
	increaseHealthBy(x);
}
void GrassHopper::doSomething()
{//simply follows the layout of the spec in order of instructions
	decrementHealth();
	if (numHealth() <= 0 && isDead() ==false)//checks if dead
	{
		setDead();
		getWorld()->allocateNewFood(getX(), getY(),100);//if yes, replace with 100 units of food
		return;
	}
	if (getStun() == 0)//checks if stunned/sleeping or not
	{
		if (getWorld()->percentageGenerator(3))//1/3 chance of trying to bite something on the square
		{
			if (gBite(this,0))//pass in this so it knows not to bite itself, and pass in 0 because a grasshopper will bite anything (other grasshoppers or ants)
			{
				setStun(2);
				return;
			}
		}
		if (getWorld()->percentageGenerator(10))//one in ten chance of jumping to a new square
		{
			if (canHop())
			{
				hop();
				setStun(2);
				return;
			}
			
		}
		if (getWorld()->isOn(getX(), getY(),foo))//checks if it is on food (foo)
		{
			eatFood(200);
			if (getWorld()->percentageGenerator(2))//2 for 50 percent (1/2) of sleeping immediately
			{
				setStun(2);
				return;
			}
		}
		if (getSteps() == 0)
		{
			setNewDirection();
			setSteps(randInt(2, 10));
		}
		if (moveNow())
			decrementSteps();
		else
			setSteps(0);
		setStun(2);
	}
	else decrementStun();
	return;
	
}

bool GrassHopper::canHop()
{
	if (getWorld()->canJump(getX(), getY()))
		return true;
	return false;
}
void GrassHopper::hop()
{
	int xp;
	int yp;
	for (;;)
	{
		xp = randInt(-10, 10);
		yp = randInt(-10, 10);
		if ((xp*xp) + (yp*yp) <= 100)
			if (canMoveThere(getX() + xp, getY() + yp))
			{
				moveTo(getX() + xp, getY() + yp);
				break;
			}
	}
	
}
bool GrassHopper::gBite(Actor* a,int ignore)
{
	return getWorld()->Bite(getX(), getY(), a,50,ignore);
}

bool Ant::aBite(Actor*a,int ignore)
{
	return getWorld()->Bite(getX(), getY(), a, 15, ignore);
}
void BabyGrasshopper::doSomething()
{
	decrementHealth();
	if (numHealth() <= 0)
	{
		setVisible(false);
		setDead();
		getWorld()->allocateNewFood(getX(), getY(),100);
		return;
	}
	if (numHealth() >= 1600)
	{
		setVisible(false);
		setDead();
		getWorld()->allocateNewAdultGrassHopper(getX(), getY());
		getWorld()->allocateNewFood(getX(), getY(),100);
		return;
	}
	if (getStun() == 0)
	{
		if (getWorld()->isOn(getX(), getY(),foo))
		{
			eatFood(200);
			if (getWorld()->percentageGenerator(2))//2 for 50 percent (1/2)
			{
				setStun(2);
				return;
			}
		}
		if (getSteps() == 0)
		{
			setNewDirection();
			setSteps(randInt(2, 10));
		}
		if (moveNow())
			decrementSteps();
		else
			setSteps(0);
		setStun(2);
	}
	else decrementStun();
	return;

}

void Poison::doSomething()
{
	getWorld()->giveMeAllToPoison(getX(), getY());//poisons everything on the square
}
void WaterPool::doSomething()
{
	getWorld()->giveMeAllToStun(getX(), getY());//stuns everthing on that spot
}

void AntHill::doSomething()
{
	decrementHealth();
	if(numHealth()<=0)
	{
		setDead();
		return;
	}
	if (getWorld()->isOn(getX(), getY(),foo))
	{
		eatFood(10000);
	}
	if (numHealth() >= 2000)
	{
		generateAnt();
		decreaseHealthBy(1500);
		getWorld()->incrementTotalNumberAnts(m_colonyNumber);//increments the student world member that keeps track of how many ants each colony made
	}
}
void AntHill::generateAnt()
{
	getWorld()->allocateNewAnt(getX(), getY(), getColony(), getCompiler());
}
void Ant::doSomething() {
	decrementHealth();
	if (numHealth() <= 0)
	{
		setDead();
		getWorld()->allocateNewFood(getX(), getY(), 100);
		return;
	}
	if (getStun() != 0)
	{
		decrementStun();
		return;
	}
	else
	{
		int count = 0;
		while (alteringworld == false&&count<=10)//can do up to 10 steps or until it changes the world state
		{
			interpret();
			count++;
		}
		count = 0;
		alteringworld = false;
	}
}
bool Ant::interpret()
{//uses the code example from the spec almost exactly as it is described
	Compiler::Command cmd;

		if (!m_compiler->getCommand(m_ic, cmd))
			return false;
		switch (cmd.opcode)
		{
		case Compiler::Opcode::moveForward:
			if (moveNow())
			{
				setWasBitten(false);//moves so it hasnt been bitten
				setWasBlocked(false);//hasnt been blocked
			}
			else setWasBlocked(true);
			alteringworld = true;
			m_ic++;
			break;
		case Compiler::Opcode::faceRandomDirection:
			setNewDirection();
			m_ic++;
			alteringworld = true;
			break;
		case Compiler::Opcode::goto_command:
			m_ic = stoi(cmd.operand1);
			break;
		case Compiler::Opcode::pickupFood:
			if (amountFood > 1400)//can only pick up upto 1800 in 400 food chunks
			{
				amountFood += getWorld()->removeAmountFood(getX(), getY(), (1800 - amountFood));
			}
			else
				amountFood += getWorld()->removeAmountFood(getX(), getY(), 400);
			m_ic++;
			alteringworld = true;
			break;
		case Compiler::Opcode::eatFood:
			eatFood(100);
			m_ic++;
			alteringworld = true;
			break;
		case Compiler::Opcode::dropFood:
			if (amountFood > 0)
			{
				getWorld()->allocateNewFood(getX(), getY(), amountFood);//this function takes care of whether to increase health of current food or to make new food
				amountFood = 0;
			}
			m_ic++;
			alteringworld = true;
			break;
		case Compiler::Opcode::bite:
			aBite(this, (m_colony + 6));//pass in (m_colony+6) because the data structure holds the corresponding colony's ants in the list at arr[m_colony+6]
			m_ic++;
			alteringworld = true;
			break;
		case Compiler::Opcode::if_command:
			if (evaluateIfCommand(cmd.operand1))
			{
				m_ic = stoi(cmd.operand2);
			}
			else m_ic++;
			break;
		case Compiler::Opcode::emitPheromone:
			getWorld()->allocateNewPheromone(getX(), getY(), m_colony);
			m_ic++;
			alteringworld = true;
			break;
		case Compiler::Opcode::rotateClockwise:
			rotateClockwise();
			m_ic++;
			alteringworld = true;
			break;
		case Compiler::Opcode::rotateCounterClockwise:
			rotateCounterClockwise();
			m_ic++;
			alteringworld = true;
			break;
		case Compiler::Opcode::generateRandomNumber:
			m_ic++;
			generateRandomNumber(stoi(cmd.operand1));
			break;
		default:
			m_ic++;
	}
		return true;
}
void Ant::generateRandomNumber(int x)
{
	if (x < 0)
		return;
	if (x == 0)
		lastRand = 0;
	else
	{
		lastRand = randInt(0, x - 1);
	}
}
bool Ant::evaluateIfCommand(std::string c)
{
	if (c == "0")
		return (whatIsInFront(getX(), getY(), m_colony + 6));//checks for danger(ignoring ants of the colony passed in)
	else if (c == "1")
		return(whatIsInFront(getX(), getY(), m_colony + 10));//checking for pheromone of the specific colony
	else if (c == "2")//if prev bitten
	{
		return prevBitten;
	}
	else if (c == "3")//if carrying food
		return(amountFood > 0);
	else if (c == "4")//if hungry
		return(numHealth() <= 25);
	else if (c == "5")//if on anthill
		return(getWorld()->isOn(getX(), getY(), m_colony + 6, true));//checks for your own anthill
	else if (c == "6")//if on food
		return getWorld()->isOn(getX(), getY(), foo);
	else if (c == "7")//if on same square as enemy
		return(getWorld()->isOn(getX(), getY(), m_colony + 6,false));//checks for enemies ignoring ants of your own colony
	else if (c == "8")//if was blocked
		return (wasBlocked);
	else if (c == "9")//if last rand int was 0
		return (lastRand == 0);
	else return false;
}
bool Ant::whatIsInFront(int x, int y,int what)const
{
	GraphObject::Direction dir = getDirection();
	if (dir == GraphObject::Direction::up)
		y++;
	else if (dir == GraphObject::Direction::right)
		x++;
	else if (dir == GraphObject::Direction::down)
		y--;
	else if (dir == GraphObject::Direction::left)
		x--;

	if (what == pher0 || what == pher1 || what == pher2 || what == pher3)//if were looking for pheremone 
		return getWorld()->whereIsPheromone(x,y,what);//call the looking for pheremone function
	else
	return (getWorld()->whereIsPoison(x,y)||(getWorld()->isOn(x, y, what, false)));//otherwise were looking for danger so call this function
}

void Ant::antEat()
{//trivial
	if (amountFood <= 0)
		return;
	else if (amountFood < 100)
	{
		increaseHealthBy(amountFood);
		amountFood = 0;
	}
	else
	{
		increaseHealthBy(100);
		amountFood -= 100;
	}
}
void Ant::rotateClockwise()
{//trivial
	GraphObject::Direction dir = getDirection();
	if (dir == GraphObject::Direction::up)
		setDirection(GraphObject::Direction::right);
	else if (dir == GraphObject::Direction::right)
		setDirection(GraphObject::Direction::down);
	else if (dir == GraphObject::Direction::down)
		setDirection(GraphObject::Direction::left);
	else if (dir == GraphObject::Direction::left)
		setDirection(GraphObject::Direction::up);
	return;
}
void Ant::rotateCounterClockwise()
{//trivial
	GraphObject::Direction dir = getDirection();
	if (dir == GraphObject::Direction::up)
		setDirection(GraphObject::Direction::left);

	else if (dir == GraphObject::Direction::right)
		setDirection(GraphObject::Direction::up);

	else if (dir == GraphObject::Direction::down)
		setDirection(GraphObject::Direction::right);

	else if (dir == GraphObject::Direction::left)
		setDirection(GraphObject::Direction::down);
	return;
}
void Pheromone::doSomething()
{
	decreaseHealthBy(1);
	if (numHealth() <= 0)
		setDead();	
}

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
