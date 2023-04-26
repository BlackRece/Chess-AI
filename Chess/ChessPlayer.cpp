#include "ChessPlayer.h"
#include "Chess\GameStatus.h"
#include "Chess\Gameplay.h"
#include "Chess\Board.h"
#include "Chess\Piece.h"

using namespace std;

void ChessPlayer::setupPlayers(
	ChessPlayer** playerWhite, 
	ChessPlayer** playerBlack,
	Board* pBoard, 
	GameStatus* pGameStatus, 
	Gameplay* pGamePlay,
	AIType aiType
)
{
	*playerBlack = new ChessPlayer(pBoard, pGameStatus, pGamePlay, PieceColor::BLACK);
	*playerWhite = new ChessPlayer(pBoard, pGameStatus, pGamePlay, PieceColor::WHITE);
	
	//int coinToss = rand() * 10;
	int coinToss = 1;	// set white as AI player
	if (coinToss % 2 == 0)
	{
		(*playerBlack)->setAI(aiType);
	}
	else
	{
		(*playerWhite)->setAI(aiType);
	}
}

ChessPlayer::ChessPlayer(
	Board* pBoard, 
	GameStatus* pGameStatus, 
	Gameplay* pGamePlay, 
	PieceColor colour
)
{
	m_colour = colour;
	m_pBoard = pBoard;
	m_pGameStatus = pGameStatus;
	m_pGamePlay = pGamePlay;
	//m_bAI = false;
	m_aiType = AIType::None;
}

unsigned int ChessPlayer::getAllLivePieces(vecPieces& vpieces)
{
	vpieces.clear();

	PieceInPostion pip;

	unsigned int count = 0;
	for (int i = m_pBoard->MIN_ROW_INDEX; i < m_pBoard->MAX_ROW_INDEX; i++)
	{
		for (int j = m_pBoard->MIN_COL_INDEX; j < m_pBoard->MAX_COL_INDEX; j++)
		{
			std::shared_ptr<Piece> pPiece = m_pBoard->getSquare(i, j)->getOccupyingPiece();

			if (pPiece == nullptr)
				continue;
			if (pPiece->getColor() != m_colour)
				continue;

			count++;
			pip.piece = pPiece;
			pip.row = i;
			pip.col = j;
			vpieces.emplace_back(pip);
		}
	}

	return count;
}

unsigned int ChessPlayer::getLivePiecesFor(PieceColor pcColour, vecPieces& vPieces)
{
	vPieces.clear();

	PieceInPostion pip;

	unsigned int count = 0;
	for (int i = m_pBoard->MIN_ROW_INDEX; i < m_pBoard->MAX_ROW_INDEX; i++)
	{
		for (int j = m_pBoard->MIN_COL_INDEX; j < m_pBoard->MAX_COL_INDEX; j++)
		{
			std::shared_ptr<Piece> pPiece = m_pBoard->getSquare(i, j)->getOccupyingPiece();

			if (pPiece == nullptr)
				continue;
			if (pPiece->getColor() != pcColour)
				continue;

			count++;
			pip.piece = pPiece;
			pip.row = i;
			pip.col = j;
			vPieces.emplace_back(pip);
		}
	}

	return count;
}

vector<std::shared_ptr<Move>> ChessPlayer::getValidMovesForPiece(PieceInPostion pip)
{
	return Gameplay::getValidMoves(m_pGameStatus, m_pBoard, pip.piece, pip.row, pip.col);
}

// chooseAIMove
// in this method - for an AI chess player - choose a move to make. This is called once per play. 
bool ChessPlayer::chooseAIMove(std::shared_ptr<Move>* moveToMake)
{
	switch (m_aiType)
	{
	case Random:
		return chooseRandomMove(moveToMake);
	case First:
		//return chooseFirstMove(moveToMake);
		break;
	case Smart:
		return chooseSmartMove(moveToMake);
		break;
	default:
		return false;
	}
}

bool ChessPlayer::chooseRandomMove(std::shared_ptr<Move>* moveToMake)
{
	vecPieces vPieces;
	unsigned int piecesAvailable = getAllLivePieces(vPieces);

	// BAD AI !! - for the first piece which can move, choose the first available move
	bool moveAvailable = false;
	int randomPiece;
	while (!moveAvailable)
	{
		randomPiece = rand() % vPieces.size(); // choose a random chess piece
		vector<std::shared_ptr<Move>> moves = getValidMovesForPiece(vPieces[randomPiece]); // get all the valid moves for this piece if any)
		if (moves.size() > 0) // if there is a valid move exit this loop - we have a candidate 
			moveAvailable = true;
	}

	// get all moves for the random piece chosen (yes there is some duplication here...)
	vector<std::shared_ptr<Move>> moves = getValidMovesForPiece(vPieces[randomPiece]);
	if (moves.size() > 0)
	{
		int field = moves.size();
		int randomMove = rand() % field; // for all the possible moves for that piece, choose a random one
		*moveToMake = moves[randomMove]; // store it in 'moveToMake' and return
		return true;
	}

	return false; // if there are no moves to make return false
}

bool ChessPlayer::chooseSmartMove(std::shared_ptr<Move>* moveToMake)
{
	/* TODO:
	evaluation methods
	1) Side Score: tally the score of the pieces of both sides, subtract them and provide the result
	2) Position Score: tally the positional score of pieces on the, add to individual peice's score and provide the result
	3) Capture Score: tally the number of attacks/captures that a piece can make, add to individual peice's score and provide the result
	4) Movement Score: tally the number of spaces that a piece can move to, add to individual peice's score and provide the result
	*/
	vecPieces vPieces;
	unsigned int piecesAvailable = getAllLivePieces(vPieces);
	Board* tempBoard = m_pBoard->hardCopy();
	int nBestScore = -INT_MAX;
	
	int bestPieceIndex = 0;
	std::shared_ptr<Move> bestMove = nullptr;
	int bestMoveScore = 0;

	// BAD AI !! - for the first piece which can move, choose the first available move
	bool moveAvailable = false;
	int randomPiece;
	for (int i = 0; i < (int)vPieces.size(); i++)
	{
		vector<std::shared_ptr<Move>> moves = getValidMovesForPiece(vPieces[i]);
		int movementScore = moves.size();
		
		if (moves.size() < 1)
			continue;
		
		std::shared_ptr<Move> hiValCapture = highestValueCapture(moves);
		
		//will move capture another piece?
		for (std::shared_ptr<Move> testMove : moves)
		{
			std::shared_ptr<Piece> capturedPiece = testMove->getCapturedPiece();
			
			if (capturedPiece == nullptr)
				continue;
			
			bool willCapture = false;
			int pieceCost = 0;

			if (capturedPiece != nullptr)
			{
				pieceCost = (int)capturedPiece->getType();
				willCapture = true;
			}
			
			if (pieceCost > bestMoveScore)
			{
				bestMoveScore = pieceCost;
				bestMove = testMove;
				bestPieceIndex = i;
			}
		}

		int iPositionScore = 0;
		
		//will move improve position
		for (std::shared_ptr<Move> testMove : moves)
		{
			auto destPos = testMove->getDestinationPosition();
			auto score = m_pBoard->getPiecePositionScore(destPos);
			
			//destPos.
		}
	}
	
	if (bestMove != nullptr)
	{
		*moveToMake = bestMove;
		return true;
	}
}

int ChessPlayer::calcMovementScore()
{
	int movementScore = 0;
	vecPieces vBlackPeices, vWhitePeices;
	
	int blackPeicesAvailable = getLivePiecesFor(PieceColor::BLACK, vBlackPeices);
	int whitePeicesAvailable = getLivePiecesFor(PieceColor::WHITE, vWhitePeices);


	
	return movementScore;
}

int ChessPlayer::calcCaptureScore(vector<std::shared_ptr<Move>> moves)
{
	int captureScore = 0;

	for (std::shared_ptr<Move> testMove : moves)
	{
		std::shared_ptr<Piece> capturedPiece = testMove->getCapturedPiece();

		if (capturedPiece == nullptr)
			continue;

		captureScore += (int)capturedPiece->getType();
	}
	
	return captureScore;
}

std::shared_ptr<Move> ChessPlayer::highestValueCapture(vector<std::shared_ptr<Move>> moves)
{
	int bestCaptureScore = 0;
	std::shared_ptr<Move> bestCaptureMove = nullptr;
	
	for (std::shared_ptr<Move> testMove : moves)
	{
		std::shared_ptr<Piece> capturedPiece = testMove->getCapturedPiece();

		if (capturedPiece == nullptr)
			continue;

		int pieceCost = (int)capturedPiece->getType();

		if (pieceCost > bestCaptureScore)
		{
			bestCaptureScore = pieceCost;
			bestCaptureMove = testMove;
		}
	}
	
	return bestCaptureMove;
}

struct MiniMaxNode
{
	int score;
	vector<MiniMaxNode> children;
};

struct PruneData 
{
	int alpha;
	int beta;
};

int minimax(MiniMaxNode node, int depth, bool maximizingPlayer, PruneData pruneData)
{
	if (depth == 0 || node.children.size() == 0)
		return node.score;
	
	int bestScore = -INT_MAX;

	Board* pBoard = new Board();
	

	for (MiniMaxNode child : node.children)
	{
		if (maximizingPlayer)
		{
			bestScore;// = INT_MAX;
			for (MiniMaxNode child : node.children)
			{
				//PruneData pruneMaxScores = PruneData();
				//pruneMaxScores.alpha = INT_MAX;
				//pruneMaxScores.beta = pruneData.beta;
				int score = minimax(child, depth - 1, true, pruneData);
				bestScore = min(score, bestScore);
				//pruneData.beta = min(pruneData.beta, bestScore);
				//if (pruneData.beta <= pruneData.alpha)
				//	break;
			}
		}
		else
		{
			bestScore = INT_MIN;
			for (MiniMaxNode child : node.children)
			{
				int score = minimax(child, depth - 1, false, pruneData);
				bestScore = max(score, bestScore);
				//pruneData.alpha = max(pruneData.alpha, bestScore);
				//if (pruneData.beta <= pruneData.alpha)
				//	break;
			}
		}
	}
	
	return bestScore;
}
