# Python3 program to find the next optimal move for a player
player, opponent = "x", "o"

boardSize = 3


# This function returns true if there are moves
# remaining on the board. It returns false if
# there are no moves left to play.
def isMovesLeft(board):
    for i in range(boardSize):
        for j in range(boardSize):
            if board[i][j] == "_":
                return True
    return False


# This is the evaluation function as discussed
# in the previous article ( http://goo.gl/sJgv68 )
def evaluate(b):
    # Checking for Rows for X or O victory.
    for row in range(boardSize):
        if b[row][0] == b[row][1] and b[row][1] == b[row][2]:
            if b[row][0] == player:
                return 10
            elif b[row][0] == opponent:
                return -10
        if boardSize == 4:
            if b[row][1] == b[row][2] and b[row][2] == b[row][3]:
                if b[row][1] == player:
                    return 10
                elif b[row][1] == opponent:
                    return -10

    # Checking for Columns for X or O victory.
    for col in range(boardSize):
        if b[0][col] == b[1][col] and b[1][col] == b[2][col]:
            if b[0][col] == player:
                return 10
            elif b[0][col] == opponent:
                return -10
        if boardSize == 4:
            if b[1][col] == b[2][col] and b[2][col] == b[3][col]:
                if b[1][col] == player:
                    return 10
                elif b[1][col] == opponent:
                    return -10

    # Checking for Diagonals for X or O victory.
    if b[0][0] == b[1][1] and b[1][1] == b[2][2]:
        if b[0][0] == player:
            return 10
        elif b[0][0] == opponent:
            return -10
    if b[0][2] == b[1][1] and b[1][1] == b[2][0]:
        if b[0][2] == player:
            return 10
        elif b[0][2] == opponent:
            return -10

    if boardSize == 4:
        if b[1][1] == b[2][2] and b[2][2] == b[3][3]:
            if b[1][1] == player:
                return 10
            elif b[1][1] == opponent:
                return -10
        if b[1][2] == b[2][1] and b[2][1] == b[3][0]:
            if b[1][2] == player:
                return 10
            elif b[1][2] == opponent:
                return -10
        if b[0][1] == b[1][2] and b[1][2] == b[2][3]:
            if b[0][1] == player:
                return 10
            elif b[0][1] == opponent:
                return -10
        if b[1][0] == b[2][1] and b[2][1] == b[3][2]:
            if b[1][0] == player:
                return 10
            elif b[1][0] == opponent:
                return -10
        if b[0][3] == b[1][2] and b[1][2] == b[2][1]:
            if b[0][3] == player:
                return 10
            elif b[0][3] == opponent:
                return -10
        if b[1][3] == b[2][2] and b[2][2] == b[3][1]:
            if b[1][3] == player:
                return 10
            elif b[1][3] == opponent:
                return -10

    # Else if none of them have won then return 0
    return 0


# This is the minimax function. It considers all
# the possible ways the game can go and returns
# the value of the board
def minimax(board, depth, isMax):
    score = evaluate(board)

    # If Maximizer has won the game return his/her
    # evaluated score
    if score == 10:
        return score

    # If Minimizer has won the game return his/her
    # evaluated score
    if score == -10:
        return score

    # If there are no more moves and no winner then
    # it is a tie
    if isMovesLeft(board) == False:
        return 0

    # If this maximizer's move
    if isMax:
        best = -1000

        # Traverse all cells
        for i in range(boardSize):
            for j in range(boardSize):
                # Check if cell is empty
                if board[i][j] == "_":
                    # Make the move
                    board[i][j] = player

                    # Call minimax recursively and choose
                    # the maximum value
                    best = max(best, minimax(board, depth + 1, not isMax))

                    # Undo the move
                    board[i][j] = "_"
        return best

    # If this minimizer's move
    else:
        best = 1000

        # Traverse all cells
        for i in range(boardSize):
            for j in range(boardSize):
                # Check if cell is empty
                if board[i][j] == "_":
                    # Make the move
                    board[i][j] = opponent

                    # Call minimax recursively and choose
                    # the minimum value
                    best = min(best, minimax(board, depth + 1, not isMax))

                    # Undo the move
                    board[i][j] = "_"
        return best


# This will return the best possible move for the player
def findBestMove(board):
    bestVal = -1000
    bestMove = (-1, -1)

    # Traverse all cells, evaluate minimax function for
    # all empty cells. And return the cell with optimal
    # value.
    for i in range(boardSize):
        for j in range(boardSize):
            # Check if cell is empty
            if board[i][j] == "_":
                # Make the move
                board[i][j] = player

                # compute evaluation function for this
                # move.
                moveVal = minimax(board, 0, False)

                # Undo the move
                board[i][j] = "_"

                # If the value of the current move is
                # more than the best value, then update
                # best/
                if moveVal > bestVal:
                    bestMove = (i, j)
                    bestVal = moveVal

    return bestMove


def findBestMove2(board):
    bestVal = 1000
    bestMove = (-1, -1)

    # Traverse all cells, evaluate minimax function for
    # all empty cells. And return the cell with optimal
    # value.
    for i in range(boardSize):
        for j in range(boardSize):
            # Check if cell is empty
            if board[i][j] == "_":
                # Make the move
                board[i][j] = opponent

                # compute evaluation function for this
                # move.
                moveVal = minimax(board, 0, True)

                # Undo the move
                board[i][j] = "_"

                # If the value of the current move is
                # more than the best value, then update
                # best/
                if moveVal < bestVal:
                    bestMove = (i, j)
                    bestVal = moveVal

    return bestMove


def print_board(board):
    print("------------")
    for x in range(boardSize):
        print(board[x])
    print("------------")


def play():
    while isMovesLeft(board):
        bestMoveX = findBestMove(board)
        if isMovesLeft(board):
            board[bestMoveX[0]][bestMoveX[1]] = player
        print_board(board)
        bestMoveY = findBestMove2(board)
        if isMovesLeft(board):
            board[bestMoveY[0]][bestMoveY[1]] = opponent
        print_board(board)


# Driver code
board = [["x", "o", "_"], ["_", "_", "_"], ["_", "_", "_"]]

play()
