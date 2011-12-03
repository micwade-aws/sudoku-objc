#pragma once

class Puzzle
{
public:
	Puzzle(void);
	Puzzle(char* puzzle, int length);
	~Puzzle(void);

	int Get(int i, int j);
	int Set(int i, int j, int value);
	
	void operator<<(Puzzle &p);

private:
	int m_BoxSetWidth;
	int m_PuzzleWidth;
	int* m_Puzzle;  // The actual puzzle represented by integers

};

