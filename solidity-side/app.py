from solcx import compile_standard, install_solc
import json
from web3 import Web3
import os
from dotenv import load_dotenv
import time
import serial

arduino = serial.Serial(port="COM7", baudrate=115200, timeout=0.1)

load_dotenv()

## variables
chain_id = 11155111  ## sepolia test network chain id
public_key = os.getenv("PUBLIC_KEY")
private_key = os.getenv("PRIVATE_KEY")
alchemy_api_key = os.getenv("ALCHEMY_API_KEY")
sol_file_path = "XOXAlphaBeta.sol"
json_file_path = "compiled_code.json"
isContractUpdated = True  ## change to True if you want to recompile the contract

install_solc("0.8.12")

if not isContractUpdated:
    with open(json_file_path, "r") as file:
        sol_file = json.load(file)
else:
    with open(sol_file_path, "r") as file:
        simple_storage_file = file.read()

    sol_file = compile_standard(
        {
            "language": "Solidity",
            "sources": {"XOXAlphaBeta.sol": {"content": simple_storage_file}},
            "settings": {
                "outputSelection": {
                    "": {"": ["abi", "metadata", "evm.bytecode", "evm.sourceMap"]}
                }
            },
        },
        solc_version="0.8.12",
    )

    with open("compiled_code.json", "w") as file:
        json.dump(sol_file, file)


sol_abi = sol_file["contracts"]["XOXAlphaBeta.sol"]["TicTacToe"]["abi"]
sol_bytecode = sol_file["contracts"]["XOXAlphaBeta.sol"]["TicTacToe"]["evm"][
    "bytecode"
]["object"]

web3_connection = Web3(
    Web3.HTTPProvider("https://eth-sepolia.g.alchemy.com/v2/" + alchemy_api_key)
)

compiled_contract = web3_connection.eth.contract(abi=sol_abi, bytecode=sol_bytecode)

nonce_value = web3_connection.eth.get_transaction_count(public_key)

## first, send the constructor transaction
constructor_txn = compiled_contract.constructor().build_transaction(
    {"from": public_key, "nonce": nonce_value}
)
signed_txn = web3_connection.eth.account.sign_transaction(
    constructor_txn, private_key=private_key
)
send_txn = web3_connection.eth.send_raw_transaction(signed_txn.rawTransaction)

txn_receipt = web3_connection.eth.wait_for_transaction_receipt(send_txn)

compiled_contract = web3_connection.eth.contract(
    address=txn_receipt.contractAddress, abi=sol_abi
)


## send raw transaction function
def send_raw_transaction(function):
    signed_txn = web3_connection.eth.account.sign_transaction(
        function, private_key=private_key
    )
    send_txn = web3_connection.eth.send_raw_transaction(signed_txn.rawTransaction)
    txn_receipt = web3_connection.eth.wait_for_transaction_receipt(send_txn)
    return txn_receipt


## create new game function
def create_new_game(GameMode):
    newGameFunction = compiled_contract.functions.newGame(GameMode).build_transaction(
        {
            "from": public_key,
            "chainId": chain_id,
            "nonce": nonce_value,
        }
    )
    receipt = send_raw_transaction(newGameFunction)
    print(receipt)  ## printing the receipt
    log = compiled_contract.events.GameCreated().process_receipt(receipt)
    gameID = log[0]["args"]["gameId"]
    if GameMode == 2:
        AIlog = compiled_contract.events.firstMoveAIvsAI().process_receipt(receipt)
        coordX, coordY = AIlog[0]["args"]["firstX"], AIlog[0]["args"]["firstY"]
        return int(gameID), coordX, coordY
    return int(gameID)


## join game function
def join_game(gameId):
    joinGameFunction = compiled_contract.functions.joinGame(gameId).build_transaction(
        {
            "from": public_key,
            "chainId": chain_id,
            "nonce": nonce_value,
        }
    )
    receipt = send_raw_transaction(joinGameFunction)
    print(receipt)


## make move function
def make_move(gameId, moveX, moveY):
    makeMoveFunction = compiled_contract.functions.makeMove(
        gameId, moveX, moveY
    ).build_transaction(
        {
            "from": public_key,
            "chainId": chain_id,
            "nonce": nonce_value,
        }
    )
    receipt = send_raw_transaction(makeMoveFunction)
    is_valid = compiled_contract.events.isMoveValid().process_receipt(receipt)[0][
        "args"
    ]["isValid"]
    return is_valid


## make move AI function
def make_move_ai(gameId):
    winner = 0  ## 0 means no winner yet
    makeMoveAI = compiled_contract.functions.makeMoveAI(gameId).build_transaction(
        {
            "from": public_key,
            "chainId": chain_id,
            "nonce": nonce_value,
        }
    )
    receipt = send_raw_transaction(makeMoveAI)
    move_made_log = compiled_contract.events.AIMoveMade().process_receipt(receipt)
    print(move_made_log)
    xCoordinate, yCoordinate = (
        move_made_log[0]["args"]["xCoordinate"],
        move_made_log[0]["args"]["yCoordinate"],
    )

    if getGameState(gameId) == 1:
        game_over_log = compiled_contract.events.GameOver().process_receipt(receipt)
        winner = game_over_log[0]["args"]["winner"]

    return (int(xCoordinate), int(yCoordinate), int(winner))


def getGameState(gameId):
    getGameStateFunction = compiled_contract.functions.getGameState(gameId).call()
    return getGameStateFunction


def getBoard(gameId):  ## control function
    getBoardFunction = compiled_contract.functions.printBoard(gameId).call()
    for i in range(3):
        print(getBoardFunction[i])
    print("\n")


def coordinate_edit(xCoord, yCoord):
    return (3 * xCoord) + yCoord + 1


def coordinate_seperate(coord):
    if coord == 1:
        return (0, 0)
    elif coord == 2:
        return (0, 1)
    elif coord == 3:
        return (0, 2)
    elif coord == 4:
        return (1, 0)
    elif coord == 5:
        return (1, 1)
    elif coord == 6:
        return (1, 2)
    elif coord == 7:
        return (2, 0)
    elif coord == 8:
        return (2, 1)
    elif coord == 9:
        return (2, 2)


def playAIvsAI(gameID, firstX, firstY):
    global nonce_value
    time.sleep(20)
    firstCoord = coordinate_edit(firstX, firstY)
    arduino.write(bytes(str(firstCoord), "utf-8"))
    time.sleep(5)
    while getGameState(gameID) != 1:
        nonce_value += 1
        xCoordAI, yCoordAI, winner = make_move_ai(gameID)
        coordEdit = coordinate_edit(xCoordAI, yCoordAI)
        arduino.write(bytes(str(coordEdit), "utf-8"))
        time.sleep(0.05)
        getBoard(gameID)
    time.sleep(5)
    return winner


def playAIvsPlayer(gameID):
    global nonce_value
    nonce_value += 1
    join_game(gameID)
    while getGameState(gameID) != 1:
        nonce_value += 1
        moveCoord = str(input("Type your move: "))
        xCoord, yCoord = coordinate_seperate(int(moveCoord))
        if not make_move(gameID, xCoord, yCoord):
            print("Wrong input!")
            continue
        arduino.write(bytes(moveCoord, "utf-8"))
        time.sleep(0.05)
        nonce_value += 1
        time.sleep(2)
        xCoordAI, yCoordAI, winner = make_move_ai(gameID)
        coordEdit = coordinate_edit(xCoordAI, yCoordAI)
        arduino.write(bytes(str(coordEdit), "utf-8"))
        time.sleep(0.05)
        getBoard(gameID)
    time.sleep(5)
    return winner


def printWinner(winner):
    if winner == 1 or winner == 2:
        print(f"Winner is player {winner}!\n")
    else:
        print("Game is tie!\n")


## if we have to wait for the transaction to be mined or increase nonce manually
nonce_value += 1

gameType = ""  ## initialize the variable
while gameType != "1" and gameType != "2":
    print("Welcome! Please select your choice: \n1) PvE\n2) AI vs AI\n")
    gameType = str(input("Type: "))

arduino.write(bytes(gameType, "utf-8"))
time.sleep(0.05)

time.sleep(20)

if gameType == "2":
    gameID, firstX, firstY = create_new_game(int(gameType))
else:
    gameID = create_new_game(int(gameType))

if gameType == "1":
    winner = playAIvsPlayer(gameID)
elif gameType == "2":
    winner = playAIvsAI(gameID, firstX, firstY)

printWinner(winner)
