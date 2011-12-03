#include <math.h>
#include <stdio.h>
#include "Puzzle.h"

Puzzle::Puzzle(void)
{
}

Puzzle::Puzzle(char* puzzle, int length)
{
	//The idea here is to convert a string into a puzzle.
	//The string must be made up of ASCI characters
	m_BoxSetWidth = (int)sqrt(sqrt((float)length));
	m_PuzzleWidth = m_BoxSetWidth * m_BoxSetWidth;

	//create the boxes in linear order
	m_BoxSets = new BoxSet[m_PuzzleWidth];
	for(int i = 0; i < m_PuzzleWidth; i++) 
		m_BoxSets[i] = BoxSet(puzzle + i*m_PuzzleWidth, m_PuzzleWidth );		
}

Puzzle::~Puzzle(void)
{
	delete m_BoxSets;
}

void Puzzle::operator<<(Puzzle &p){
	int s = 0;

	//print the puzzle "line by line" 
	for(int boxRow = 0; boxRow < m_BoxSetWidth; boxRow++) {
		s=0;
		// Print each box row "line by line"
		for(int box = 0; box < m_BoxSetWidth; box++) {
			for(int n = 0; n < m_BoxSetWidth; n++) {
				printf("%d ", m_BoxSets[box+boxRow].m_box[n+s].m_value);
			}
		}
		s+=m_BoxSetWidth;
	}
}

int Puzzle::Get(int i, int j){
	return m_Puzzle[
}
