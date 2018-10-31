#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include "Field.h"
#include<list>
class Compiler;
class Actor;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
//CONSTANTS USED TO KEEP TRACK OF ACTORS AND PUT THEM IN THE RIGHT LIST IN THE ARRAY (ALL PEBBLES GO IN THE LIST AT SLOT 0 OF THE ARRAY ETC...//
const int peb = 0;
const int foo = 1;
const int babyG = 2;
const int adultG = 3;
const int pois = 4;
const int wat = 5;
const int ant0 = 6; //ants and anthill of colony 0 both go in the list at array slot 6 with the anthill always ocupying the beginning of the list
const int ant1 = 7;
const int ant2 = 8;
const int ant3 = 9;
const int pher0 = 10;
const int pher1 = 11;
const int pher2 = 12;
const int pher3 = 13;
const int NUM_ELEMENTS = 14;

//ARBITRARY CONSTANTS TO DISTINGUISH ANTHILLS FROM ANTS SINCE ANTS AND ANTHILLS OF THE SAME COLONY ARE STORED IN THE SAME LIST
const int anth0 = -1;
const int anth1 = -2;
const int anth2 = -3;
const int anth3 = -4;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
		m_time = 0;
		m_colonyWinner = -1;
		m_totalCol1 = 0; m_totalcol2 = 0; m_totalcol3 = 0; m_totalcol0 = 0;
		
	}

	virtual int init();
	virtual int move();
	virtual void cleanUp();
	~StudentWorld();


	bool canMoveThere(int x, int y)const;
	bool isOn(int x, int y,int what,bool hill = false) const;
	bool whereIsPoison(int x, int y)const;
	bool whereIsPheromone(int x, int y,int which) const;

	void allocateNewFood(int x, int y,int amount);
	void allocateNewAdultGrassHopper(int x, int y);
	void allocateNewAnt(int x, int y,int col, Compiler* a);
	void allocateNewPheromone(int x, int y,int whos);
	int removeAmountFood(int x, int y,int amount);

	void giveMeAllToPoison(int x, int y);
	void giveMeAllToStun(int x, int y);
	bool Bite(int x, int y, Actor* a, int dmg,int ignore);
	bool canJump(int x, int y)const;

	bool percentageGenerator(int b);
	void setDisplayText();
	std::string someFunctionToFormatThingsNicely(int ticks,int antsAnt0,int antsAnt1,int antsAnt2,int antsAnt3,int winningAntNumber);
	void incrementTotalNumberAnts(int colnum);
	bool determineWinner();
private:
	int m_time;
	std::list<Actor*>arr[NUM_ELEMENTS];
	int m_colonyWinner;
	Compiler *compilerForEntrant0, *compilerForEntrant1,
		*compilerForEntrant2, *compilerForEntrant3;
	int m_totalCol1, m_totalcol2, m_totalcol3, m_totalcol0;
};

#endif // STUDENTWORLD_H_
