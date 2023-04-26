#include "Board.h"

Board::Board()
{
}

Board::~Board()
{
}

Board::Board(const Board& board)
{
	int posXScore = 0;
	int posYScore = 0;
	int midRow = MAX_ROW_INDEX / 2;
	int midCol = MAX_COL_INDEX / 2;
	
	for(int row= MIN_ROW_INDEX; row< MAX_ROW_INDEX; row++)
	{
		int iRowScore = 0;
		
		int rowDelta = row - midRow;
		if (rowDelta == 0)
			iRowScore = 5;
		else 
			iRowScore = rowDelta < 0 ? abs(rowDelta - midRow) : rowDelta;
		
		for(int col= MIN_COL_INDEX; col< MAX_COL_INDEX; col++)
		{
			int iColScore = 0;
			
			int iColDelta = col - midCol;
			if (iColDelta == 0)
				iColScore = 5;
			else
				iColScore = iColDelta < 0 ? abs(iColDelta - midCol) : iColDelta;
			
			piecePositionScoreMap.insert(
				std::pair<std::pair<int, int>, int>(
					std::pair<int, int>(row, col), iRowScore + iColScore
				)
			);
				
			this->squares[row][col] = board.squares[row][col];
		}
	}
}

Square* Board::getSquare(int row, int col)
{
	return &squares[row][col];
}

Board* Board::hardCopy()
{
	Board* pBoard = new Board();

	for (int row = MIN_ROW_INDEX; row < MAX_ROW_INDEX; row++)
	{
		for (int col = MIN_COL_INDEX; col < MAX_COL_INDEX; col++)
		{
			Square temp = this->squares[row][col].hardCopy();
			if (temp.getOccupyingPiece())
			{
				pBoard->squares[row][col] = temp;
			}
		}
	}

	return pBoard;
}

int Board::getPiecePositionScore(std::pair<int, int> pos)
{
	int row = pos.first;
	int col = pos.second;
	
	
	return 0;
}