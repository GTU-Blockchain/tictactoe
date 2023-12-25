// SPDX-License-Identifier: GPL-3.0

pragma solidity >=0.8.12;

contract TicTacToe {
    enum Players {
        None,
        PlayerOne,
        PlayerTwo
    }

    enum Winners {
        None,
        PlayerOne,
        PlayerTwo,
        Draw
    }

    enum GameType {
        PlayerVsPlayer,
        PlayerVsComputer,
        ComputerVsComputer
    }

    struct Game {
        address playerOne;
        address playerTwo;
        Winners winner;
        Players playerTurn;
        GameType gameType;
        Players[3][3] board;
    }

    mapping(uint256 => Game) private games;

    uint256 nrOfGames;
    uint256 randomCounter = 0;

    // Events
    event GameCreated(uint256 gameId, address creator);
    event PlayerJoinedGame(uint256 gameId, address player, uint8 playerNumber);
    event PlayerMadeMove(
        uint256 gameId,
        address player,
        uint8 xCoordinate,
        uint8 yCoordinate
    );
    event GameOver(uint256 gameId, Winners winner);
    event AIMoveMade(uint8 xCoordinate, uint8 yCoordinate);
    event firstMoveAIvsAI(uint8 firstX, uint8 firstY);
    event isMoveValid(bool isValid);

    function newGame(GameType _gameType) public returns (uint256 gameID) {
        // this function returns gameID
        Game memory game;
        game.playerTurn = Players.PlayerOne;
        game.gameType = _gameType;

        if (game.gameType == GameType.ComputerVsComputer) {
            // if gametype is compVScomp, it randomly does the first move. First move got to be random since compVScomp uses the same algorithm for every player. It prevents the game from being tied.
            uint8 firstX = uint8(random(3));
            uint8 firstY = uint8(random(3));
            game.board[firstX][firstY] = Players.PlayerOne;
            game.playerTurn = Players.PlayerTwo;
            emit firstMoveAIvsAI(firstX, firstY);
        }

        games[nrOfGames] = game;
        nrOfGames++;

        emit GameCreated(nrOfGames, msg.sender);

        return nrOfGames; // returns the game ID
    }

    function printBoard(
        // it is for testing, it allows us to see the board
        uint256 _gameID
    ) public view returns (string[3][3] memory) {
        Game memory game = games[_gameID - 1];
        string[3][3] memory str;
        for (uint256 i = 0; i < 3; i++) {
            for (uint256 j = 0; j < 3; j++) {
                if (game.board[i][j] == Players.PlayerOne) str[i][j] = "X";
                if (game.board[i][j] == Players.PlayerTwo) str[i][j] = "O";
                if (game.board[i][j] == Players.None) str[i][j] = "_";
            }
        }

        return str;
    }

    function joinGame(
        uint256 _gameID
    ) public returns (bool success, string memory reason) {
        if (_gameID > nrOfGames) {
            // ID control
            return (false, "No such game exists.");
        }

        address player = msg.sender;
        Game storage game = games[_gameID - 1];

        if (game.gameType == GameType.ComputerVsComputer)
            return (false, "this game is compVScomp mode, you can't join");
        else if (game.gameType == GameType.PlayerVsComputer)
            game.playerTwo = address(1); // in player vs computer mode, people can't join to slot 2.

        // Assign the new player to slot 1 if it is still available.
        if (game.playerOne == address(0)) {
            game.playerOne = player;
            emit PlayerJoinedGame(_gameID, player, uint8(Players.PlayerOne));

            return (true, "Joined as player one.");
        }

        // If slot 1 is taken, assign the new player to slot 2 if it is still available.
        if (game.playerTwo == address(0)) {
            game.playerTwo = player;
            emit PlayerJoinedGame(_gameID, player, uint8(Players.PlayerTwo));

            return (
                true,
                "Joined as player two. Player one can make the first move."
            );
        }

        return (false, "All seats taken.");
    }

    function makeMove(
        uint256 _gameID,
        uint8 _xCoordinate,
        uint8 _yCoordinate
    ) public returns (bool success, string memory reason) {
        if (_gameID > nrOfGames) {
            emit isMoveValid(false);
            return (false, "No such game exists.");
        }

        Game storage game = games[_gameID - 1];

        if (game.winner != Winners.None) {
            emit isMoveValid(false);
            return (false, "The game has already ended.");
        }

        if (msg.sender != getCurrentPlayer(game)) {
            emit isMoveValid(false);
            return (false, "It is not your turn.");
        }

        if (game.board[_xCoordinate][_yCoordinate] != Players.None) {
            emit isMoveValid(false);
            return (false, "There is already a mark at the given coordinates.");
        }

        game.board[_xCoordinate][_yCoordinate] = game.playerTurn;

        emit PlayerMadeMove(_gameID, msg.sender, _xCoordinate, _yCoordinate);
        emit isMoveValid(true);

        Winners winner = calculateWinner(game.board);
        if (winner != Winners.None) {
            game.winner = winner;
            emit GameOver(_gameID, winner);
            return (true, "The game is over.");
        }

        nextPlayer(game);

        return (true, "");
    }

    function getCurrentPlayer(
        Game memory _game
    ) private pure returns (address player) {
        if (_game.playerTurn == Players.PlayerOne) return _game.playerOne;

        if (_game.playerTurn == Players.PlayerTwo) return _game.playerTwo;

        return address(0);
    }

    function calculateWinner(
        Players[3][3] memory _board
    ) private pure returns (Winners winner) {
        Players player = winnerInRow(_board);
        if (player == Players.PlayerOne) return Winners.PlayerOne;
        if (player == Players.PlayerTwo) return Winners.PlayerTwo;

        player = winnerInColumn(_board);
        if (player == Players.PlayerOne) return Winners.PlayerOne;
        if (player == Players.PlayerTwo) return Winners.PlayerTwo;

        player = winnerInDiagonal(_board);
        if (player == Players.PlayerOne) return Winners.PlayerOne;
        if (player == Players.PlayerTwo) return Winners.PlayerTwo;

        if (isBoardFull(_board)) return Winners.Draw;

        return Winners.None;
    }

    function winnerInRow(
        Players[3][3] memory _board
    ) private pure returns (Players winner) {
        for (uint8 x = 0; x < 3; x++) {
            if (
                _board[x][0] == _board[x][1] &&
                _board[x][1] == _board[x][2] &&
                _board[x][0] != Players.None
            ) {
                return _board[x][0];
            }
        }

        return Players.None;
    }

    function winnerInColumn(
        Players[3][3] memory _board
    ) private pure returns (Players winner) {
        for (uint8 y = 0; y < 3; y++) {
            if (
                _board[0][y] == _board[1][y] &&
                _board[1][y] == _board[2][y] &&
                _board[0][y] != Players.None
            ) {
                return _board[0][y];
            }
        }

        return Players.None;
    }

    function winnerInDiagonal(
        Players[3][3] memory _board
    ) private pure returns (Players winner) {
        if (
            _board[0][0] == _board[1][1] &&
            _board[1][1] == _board[2][2] &&
            _board[0][0] != Players.None
        ) {
            return _board[0][0];
        }

        if (
            _board[0][2] == _board[1][1] &&
            _board[1][1] == _board[2][0] &&
            _board[0][2] != Players.None
        ) {
            return _board[0][2];
        }

        return Players.None;
    }

    function isBoardFull(
        Players[3][3] memory _board
    ) private pure returns (bool isFull) {
        for (uint8 x = 0; x < 3; x++) {
            for (uint8 y = 0; y < 3; y++) {
                if (_board[x][y] == Players.None) {
                    return false;
                }
            }
        }

        return true;
    }

    function nextPlayer(Game storage _game) private {
        if (_game.playerTurn == Players.PlayerOne) {
            _game.playerTurn = Players.PlayerTwo;
        } else {
            _game.playerTurn = Players.PlayerOne;
        }
    }

    function findBestMove(
        Game memory _game,
        Players[3][3] storage _board
    ) private returns (uint8[2] memory) {
        bool isPlayerOne;
        if (_game.playerTurn == Players.PlayerOne) isPlayerOne = true;
        else isPlayerOne = false;

        uint8[2] memory bestMove;
        int256 bestScore;
        if (isPlayerOne) bestScore = int256(type(int).min);
        else bestScore = int256(type(int).max);

        int256 alpha = int256(type(int).min);
        int256 beta = int256(type(int).max);

        for (uint8 i = 0; i < 3; i++) {
            for (uint8 j = 0; j < 3; j++) {
                if (_board[i][j] == Players.None) {
                    _board[i][j] = _game.playerTurn;
                    int256 score = alphabeta(
                        _board,
                        9,
                        alpha,
                        beta,
                        !isPlayerOne
                    ); // false for player one, true for player two
                    _board[i][j] = Players.None;

                    if (
                        (isPlayerOne && (score > bestScore)) ||
                        (!isPlayerOne && (score < bestScore))
                    ) {
                        bestScore = score;
                        bestMove[0] = i;
                        bestMove[1] = j;
                    }

                    alpha = max(int(bestScore), int(alpha));
                }
            }
        }
        return bestMove;
    }

    // alpha beta pruning
    function alphabeta(
        Players[3][3] memory state,
        uint8 depth,
        int256 alpha,
        int256 beta,
        bool isMaximizing
    ) internal view returns (int256) {
        if (
            depth == 0 ||
            calculateWinner(state) != Winners.None ||
            isBoardFull(state)
        ) {
            return evaluate(state);
        }

        if (isMaximizing) {
            int256 maxScore = int256(type(int).min);
            for (uint8 i = 0; i < 3; i++) {
                for (uint8 j = 0; j < 3; j++) {
                    if (state[i][j] == Players.None) {
                        Players[3][3] memory newState = state;
                        newState[i][j] = Players.PlayerOne;
                        int256 score = alphabeta(
                            newState,
                            depth - 1,
                            alpha,
                            beta,
                            !isMaximizing
                        );
                        maxScore = int256(max(int(score), int(maxScore)));
                        alpha = int256(max(int(maxScore), int(alpha)));
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }
            }
            return maxScore;
        } else {
            int256 minScore = int256(type(int).max);
            for (uint8 i = 0; i < 3; i++) {
                for (uint8 j = 0; j < 3; j++) {
                    if (state[i][j] == Players.None) {
                        Players[3][3] memory newState = state;
                        newState[i][j] = Players.PlayerTwo;
                        int256 score = alphabeta(
                            newState,
                            depth - 1,
                            alpha,
                            beta,
                            !isMaximizing
                        );
                        minScore = int256(min(int(score), int(minScore)));
                        beta = int256(min(int(minScore), int(beta)));
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }
            }
            return minScore;
        }
    }

    function evaluate(
        Players[3][3] memory state
    ) internal pure returns (int256) {
        Winners winner = calculateWinner(state);
        if (winner == Winners.PlayerOne) return 10;
        else if (winner == Winners.PlayerTwo) return -10;
        else return 0;
    }

    function makeMoveAI(
        uint8 _gameID
    ) public returns (bool success, string memory reason) {
        Game storage _game = games[_gameID - 1];

        if (_game.winner != Winners.None)
            return (false, "The game has already ended.");

        if (_game.gameType == GameType.PlayerVsPlayer)
            return (false, "computer can't play in PvP mode");

        if (
            _game.gameType == GameType.PlayerVsComputer &&
            _game.playerTurn == Players.PlayerOne
        ) return (false, "this is not AI's turn");

        uint8[2] memory move = findBestMove(_game, _game.board);
        _game.board[move[0]][move[1]] = _game.playerTurn;
        nextPlayer(_game);
        emit AIMoveMade(move[0], move[1]);

        Winners winner = calculateWinner(_game.board);
        if (winner != Winners.None) {
            _game.winner = winner;
            emit GameOver(_gameID, winner);

            return (true, "The game is over.");
        }
        return (true, "");
    }

    function random(uint256 number) private returns (uint256) {
        return
            // this function creates a random number within the range of given input via keccak256.
            uint256(
                keccak256(
                    abi.encodePacked(
                        block.timestamp,
                        block.difficulty,
                        msg.sender,
                        randomCounter++
                    )
                )
            ) % number;
    }

    function max(int256 _a, int256 _b) private pure returns (int256 _c) {
        // max function for integers
        if (_a > _b) return _a;
        else return _b;
    }

    function min(int256 _a, int256 _b) private pure returns (int256 _c) {
        // min function for integers
        if (_a < _b) return _a;
        else return _b;
    }

    function getGameState(uint8 _gameID) public view returns (uint8) {
        return games[_gameID - 1].winner == Winners.None ? 0 : 1;
    }
}
