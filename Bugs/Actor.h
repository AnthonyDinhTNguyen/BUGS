#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <string>
#include "Compiler.h"
class StudentWorld;
struct time
{
	time(int t = 2000) :m_time(t) {}
	void tick() { if (m_time<2000) m_time++; }
	bool gameOver() { return(m_time <= 0); }
	int m_time;
};
class Actor : public GraphObject
{
public:
	Actor(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size)
		:GraphObject(IID, xp, yp, dir, depth, size), m_worldP(swp) {m_dead = false;}
	virtual ~Actor() { setVisible(false); }
	virtual void doSomething() = 0;
	virtual std::string whatAmI() { return""; }
	bool isDead()const { return m_dead; }
	void setDead() { m_dead = true; }
	StudentWorld* getWorld()const { return m_worldP; }
private:
	bool m_dead;
	StudentWorld* m_worldP;
	

};
class EnergyHolder :public Actor
{
public:
	EnergyHolder(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size, int health) 
		:Actor(IID, xp, yp, swp, dir, depth, size),m_hitPoints(health) {}
	void decrementHealth() { m_hitPoints--; }
	void incrementHealth() { m_hitPoints++; }
	void decreaseHealthBy(int n) { m_hitPoints -= n; }
	void increaseHealthBy(int n) { m_hitPoints += n; }
	int numHealth()const { return m_hitPoints; }
	virtual void eatFood(int amount);
	//void doSomething() { return; }
private:
	int m_hitPoints;
};

class Food :public EnergyHolder
{
public:
	Food(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size, int health) 
		:EnergyHolder(IID, xp, yp, swp, dir, depth, size, health) { setVisible(true); }
	void doSomething() { return; }
};

class Pheromone :public EnergyHolder
{
public:
	Pheromone(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size, int health) 
		:EnergyHolder(IID, xp, yp, swp, dir, depth, size, health) { setVisible(true); }
	void doSomething();
};

class AntHill :public EnergyHolder
{
public: 
	AntHill(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size, int health, Compiler*a,int col) 
		:EnergyHolder(IID, xp, yp, swp, dir, depth, size, health),m_Compiler(a),m_colonyNumber(col) { setVisible(true); }
	virtual std::string whatAmI() { return"AntHill"; }
	void doSomething();
private:
	void generateAnt();//allocates a new ant
	Compiler* getCompiler() { return m_Compiler; }
	int getColony() { return m_colonyNumber; }
	Compiler*m_Compiler;
	int m_colonyNumber;

};

class Mover :public EnergyHolder
{
public:
	Mover(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size, int health)
		:EnergyHolder(IID, xp, yp, swp, dir, depth, size, health) {
		m_stepsLeft = randInt(2, 10); m_stunLeft = 0; m_stunnedByWater = false;}
	int getSteps()const { return m_stepsLeft; }
	int getStun()const { return m_stunLeft; }
	void setSteps(int n) { m_stepsLeft = n; }
	void setStun(int n) { m_stunLeft = n; }
	void decrementSteps() { m_stepsLeft--; }
	void decrementStun() { m_stunLeft--; }
	void incrementStunBy(int x) { m_stunLeft += x; }
	bool wasStunnedByWater()const { return m_stunnedByWater; }
	void setWasStunnedByWater(bool a) { m_stunnedByWater = a; }
	bool moveNow();
	bool canMoveThere(int x, int y)const;
	void setNewDirection();//changes the direction to a random direction
private:
	bool m_stunnedByWater;
	int m_stunLeft;
	int m_stepsLeft;
};
class Ant :public Mover
{
public:
	Ant(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size, int health, Compiler* com,int col)
		:Mover(IID, xp, yp, swp, dir, depth, size, health),m_compiler(com),m_colony(col)
	{
		setVisible(true); setNewDirection(); amountFood =0; prevBitten = false;
		wasBlocked = false; lastRand = 0; m_ic = 0; alteringworld = false;
	}
	void doSomething();
	std::string whatAmI() { return "Ant"; }
	void setWasBitten(bool a) { prevBitten = a; }
	virtual void eatFood(int amount) { antEat(); }
	
private:
	Compiler* m_compiler;
	int m_colony;
	int amountFood;
	bool prevBitten;
	bool wasBlocked;
	int lastRand;
	int m_ic;
	bool alteringworld;

	bool interpret();
	bool evaluateIfCommand(std::string c);
	void setWasBlocked(bool a) { wasBlocked = a; }
	bool whatIsInFront(int x, int y, int what)const;//returns true for danger in front
	bool aBite(Actor* a, int ignore);
	void rotateClockwise();
	void rotateCounterClockwise();
	void generateRandomNumber(int x);
	void antEat();
};
class GrassHopper :public Mover
{
public:

	GrassHopper(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size, int health) 
		:Mover(IID, xp, yp, swp, dir, depth, size, health) 
		{ setVisible(true); setNewDirection();}
	virtual std::string whatAmI() { return"Adult GrassHopper"; }
	virtual void doSomething();
	virtual bool gBite(Actor* a,int ignore);
private:
	void hop();
	bool canHop();
};

class BabyGrasshopper :public GrassHopper
{
public:
	BabyGrasshopper(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size, int health) 
		:GrassHopper(IID, xp, yp, swp, dir, depth, size, health) { setVisible(true);}
	void doSomething();
};

class Terrain :public Actor
{
public:
	Terrain(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size)
		: Actor(IID, xp, yp, swp, dir, depth, size) {}

};

class Pebble : public Terrain
{
public:
	Pebble(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size) 
		: Terrain(IID, xp, yp, swp, dir, depth, size) { setVisible(true); }
	virtual void doSomething() { return; }
};

class Poison :public Terrain
{
public:
	Poison(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size)
		:Terrain(IID, xp, yp, swp, dir, depth, size) {}
	void doSomething();

};

class WaterPool : public Terrain
{
public :
	WaterPool(int IID, int xp, int yp, StudentWorld* swp, GraphObject::Direction dir, int depth, double size) 
		:Terrain(IID, xp, yp, swp, dir, depth, size) {}
	void doSomething();
};


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
