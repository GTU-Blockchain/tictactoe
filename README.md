
# XOX Robot driven by Blockchain

This repository is for a robot playing XOX using blockchain technology. It can allow us to store all data and run all algorithm into blockchain. 


## How did it go?

First of all, we started to create a simple tic-tac-toe game in Solidity as a framework. It contained only Player versus Player gamemode. Then we dived into searching for an algorithm that suits our project since EVM is a very limited work environment. The algorithm had to be efficient and as light as possible. We found a minimax algorithm for the tic-tac-toe AI. Minimax algorithm is a game theory algorithm that checks all possible moves and picks the optimum move. We tested the algorithm. However, we encountered a problem. The Minimax algorithm has a lot of recursion and it is not suitable for EVM. Then we searched again and found alpha-beta pruning for the project. Alpha-beta pruning is a developed algorithm that reduces workload. It is basically minimax but it breaks the loop when the current move is worse than other moves.

Once we settled on an algorithm, we looked into game modes. Project has three game modes: AI vs AI, AI vs Player and Player vs Player. AI vs Player is basic, we created MakeMoveAI function and used it as a second player. We added control mechanisms for AI vs Player to prevent wrong moves. For instance, AI could play when it was the player's turn, we prevented it. Then we created AI vs AI game mode but we had a problem. Since both AIs use the same algorithm, they are almost always tied. To solve that problem, we created a random function and randomly made the first move of the game. This prevents the huge possibility of the game being tied.

Finally, all things were working. We started to optimize the code. The improvements made can be seen in our latest update below.

- Unnecessary big numbers have been reduced.
- Comment lines were added.
- AI could play when the player's turn, AI can no longer play if it is not it's turn.

We are still optimizing and debugging the code to develop the project. Improvements will be added.


## Authors
**Mentor**
- [@selcuktopal80](https://www.github.com/selcuktopal80)

**Software Team**
- [@umutsatir](https://www.github.com/umutsatir)

**Hardware Team**

## Deployment

To deploy this project run:

You need to upload the Arduino code to ESP32. Then edit the python code (ESP32 port) and .env file (wallet addresses). Lastly, run the Python code.

## Roadmap

- C++ has no Web3 library now. When it is created, C++ Web3 library will be used instead of Python
