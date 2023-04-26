#pragma once
#include "Chess\PieceColor.h"
#include "Chess\PieceType.h"
#include <vector>
#include <memory>
#include "Chess/Move.h"

using namespace std;

class Piece;
class Board;
class GameStatus;
class Gameplay;
class Move;

enum AIType
{
	None	= 0,
	Random	= 1,
	First	= 2,
	Smart	= 3
};

struct PieceInPostion
{
	std::shared_ptr<Piece> piece;
	int col;
	int row;
};

typedef vector<PieceInPostion> vecPieces;

class ChessPlayer
{
public:
	static void		setupPlayers(ChessPlayer** playerWhite, ChessPlayer** playerBlack, Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, AIType aiType);
	ChessPlayer(Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, PieceColor colour);

	//void			setAI() { m_bAI = true; }
	void			setAI(AIType aiType) { /* m_bAI = true; */ m_aiType = aiType; }
	bool			isAI() { /* return m_bAI; */ return m_aiType != AIType::None; }
	unsigned int	getAllLivePieces(vecPieces& vpieces);
	unsigned int	getLivePiecesFor(PieceColor colour, vecPieces& vpieces);
	vector<std::shared_ptr<Move>>	getValidMovesForPiece(PieceInPostion pip);
	bool			chooseAIMove(std::shared_ptr<Move>* moveToMake);
	bool			chooseRandomMove(std::shared_ptr<Move>* moveToMake);
	bool			chooseSmartMove(std::shared_ptr<Move>* moveToMake);

protected:
	PieceColor		getColour() { return m_colour; }

private:
	PieceColor	m_colour;
	AIType		m_aiType;
	Board*		m_pBoard;
	GameStatus* m_pGameStatus;
	Gameplay*	m_pGamePlay;
	//bool		m_bAI;

	std::shared_ptr<Move> highestValueCapture(vector<std::shared_ptr<Move>> moves);
	int calcCaptureScore(vector<std::shared_ptr<Move>> moves);
	int calcMovementScore();
};

