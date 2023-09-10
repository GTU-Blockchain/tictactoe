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

    // Events
    event GameCreated(uint256 gameId, address creator);
    event PlayerJoinedGame(uint256 gameId, address player, uint8 playerNumber);
    event PlayerMadeMove(
        uint256 gameId,
        address player,
        uint256 xCoordinate,
        uint256 yCoordinate
    );
    event GameOver(uint256 gameId, Winners winner);

    function newGame(GameType _gameType) public returns (uint256 gameID) {
        Game memory game;
        game.playerTurn = Players.PlayerOne;
        game.gameType = _gameType;

        if (game.gameType == GameType.ComputerVsComputer) {
            game.board[random(3)][random(3)] = Players.PlayerOne;
            game.playerTurn = Players.PlayerTwo;
            game.playerTwo = address(1);
        }

        games[nrOfGames] = game;
        nrOfGames++;

        emit GameCreated(nrOfGames, msg.sender);

        return nrOfGames;
    }

    function printBoard(
        uint256 _gameID
    ) public view returns (string[3][3] memory) {
        // temporary function for checking the board
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
            return (false, "No such game exists.");
        }

        address player = msg.sender;
        Game storage game = games[_gameID - 1];

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
        uint256 _xCoordinate,
        uint256 _yCoordinate
    ) public returns (bool success, string memory reason) {
        if (_gameID > nrOfGames) return (false, "No such game exists.");

        Game storage game = games[_gameID - 1];

        if (game.winner != Winners.None)
            return (false, "The game has already ended.");

        if (msg.sender != getCurrentPlayer(game))
            return (false, "It is not your turn.");

        if (game.board[_xCoordinate][_yCoordinate] != Players.None)
            return (false, "There is already a mark at the given coordinates.");

        game.board[_xCoordinate][_yCoordinate] = game.playerTurn;

        emit PlayerMadeMove(_gameID, msg.sender, _xCoordinate, _yCoordinate);

        Winners winner = calculateWinner(game.board);
        if (winner != Winners.None) {
            game.winner = winner;
            emit GameOver(_gameID, winner);

            return (true, "The game is over.");
        }

        nextPlayer(game);

        if (game.gameType == GameType.PlayerVsComputer) {
            makeMoveY(game, game.board);
            return (true, "computer played, play again");
        }

        return (true, "");
    }

    function getCurrentPlayer(
        Game memory _game
    ) public pure returns (address player) {
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

    // Minimax algorithm functions
    function MiniMax(
        Players[3][3] storage _board,
        uint256 _depth,
        bool _isMax
    ) private returns (int256) {
        Winners winner = calculateWinner(_board);
        if (winner == Winners.PlayerOne) return 10;
        if (winner == Winners.PlayerTwo) return -10;
        if (winner == Winners.Draw) return 0;

        int256 best;

        if (_isMax) {
            best = -1000;

            for (uint8 i = 0; i < 3; i++) {
                for (uint8 j = 0; j < 3; j++) {
                    if (_board[i][j] == Players.None) {
                        _board[i][j] == Players.PlayerOne;
                        best = max(best, MiniMax(_board, _depth + 1, !_isMax));
                        _board[i][j] = Players.None;
                    }
                }
            }
            return best;
        } else {
            best = 1000;

            for (uint8 i = 0; i < 3; i++) {
                for (uint8 j = 0; j < 3; j++) {
                    if (_board[i][j] == Players.None) {
                        _board[i][j] == Players.PlayerTwo;
                        best = min(best, MiniMax(_board, _depth + 1, !_isMax));
                        _board[i][j] = Players.None;
                    }
                }
            }
            return best;
        }
    }

    function findBestMoveX(
        Players[3][3] storage _board
    ) private returns (uint8[2] memory _bestMove) {
        int256 bestVal = -1000;
        int256 moveVal;
        uint8[2] memory bestMove;

        for (uint8 i = 0; i < 3; i++) {
            for (uint8 j = 0; j < 3; j++) {
                if (_board[i][j] == Players.None) {
                    _board[i][j] = Players.PlayerOne;

                    moveVal = MiniMax(_board, 0, false);

                    _board[i][j] = Players.None;

                    if (moveVal > bestVal) {
                        bestMove[0] = i;
                        bestMove[1] = j;
                        bestVal = moveVal;
                    }
                }
            }
        }
        return bestMove;
    }

    function findBestMoveY(
        Players[3][3] storage _board
    ) private returns (uint8[2] memory) {
        int256 bestVal = 1000;
        int256 moveVal;
        uint8[2] memory bestMove;

        for (uint8 i = 0; i < 3; i++) {
            for (uint8 j = 0; j < 3; j++) {
                if (_board[i][j] == Players.None) {
                    _board[i][j] = Players.PlayerTwo;

                    moveVal = MiniMax(_board, 0, true);

                    _board[i][j] = Players.None;

                    if (moveVal < bestVal) {
                        bestMove[0] = i;
                        bestMove[1] = j;
                        bestVal = moveVal;
                    }
                }
            }
        }
        return bestMove;
    }

    function makeMoveX(
        // TODO duplicate functions will be deleted!
        Game storage _game,
        Players[3][3] storage _board
    ) private returns (bool success, string memory reason) {
        if (_game.playerTurn != Players.PlayerOne)
            return (false, "Not first players turn");
        uint8[2] memory move = findBestMoveX(_board);
        _board[move[0]][move[1]] = Players.PlayerOne;
        nextPlayer(_game);
        return (true, "");
    }

    function makeMoveY(
        Game storage _game,
        Players[3][3] storage _board
    ) private returns (bool success, string memory reason) {
        if (_game.playerTurn != Players.PlayerTwo)
            return (false, "Not second players turn");
        uint8[2] memory move = findBestMoveY(_board);
        _board[move[0]][move[1]] = Players.PlayerTwo;
        nextPlayer(_game);
        return (true, "");
    }

    function random(uint256 number) private view returns (uint256) {
        return
            uint256(
                keccak256(
                    abi.encodePacked(
                        block.timestamp,
                        block.difficulty,
                        msg.sender
                    )
                )
            ) % number;
    }

    function max(int256 _a, int256 _b) private pure returns (int256 _c) {
        if (_a > _b) return _a;
        else return _b;
    }

    function min(int256 _a, int256 _b) private pure returns (int256 _c) {
        if (_a < _b) return _a;
        else return _b;
    }
}
