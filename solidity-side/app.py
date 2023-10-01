from solcx import compile_standard, install_solc
import json
from web3 import Web3
import os
from dotenv import load_dotenv
import serial
import time

arduino = serial.Serial(port="COM4", baudrate=115200, timeout=0.1)

load_dotenv()

## variables
chain_id = 11155111  ## sepolia test network chain id
public_key = os.getenv("PUBLIC_KEY")
private_key = os.getenv("PRIVATE_KEY")
alchemy_api_key = os.getenv("ALCHEMY_API_KEY")
sol_file_path = "solidity-side/XOXAlphaBeta.sol"
json_file_path = "solidity-side/compiled_code.json"
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
                    "*": {"*": ["abi", "metadata", "evm.bytecode", "evm.sourceMap"]}
                }
            },
        },
        solc_version="0.8.12",
    )

    with open("solidity-side/compiled_code.json", "w") as file:
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
    log = compiled_contract.events.GameCreated().process_receipt(receipt)
    gameID = log[0]["args"]["gameId"]
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
    print(receipt)


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


## if we have to wait for the transaction to be mined or increase nonce manually
nonce_value += 1
gameID = create_new_game(2)

## AI vs AI game
while getGameState(gameID) != 1:
    nonce_value += 1
    xCoord, yCoord, winner = make_move_ai(gameID)
    print("AI move:", xCoord, yCoord)  ## !! for testing purposes
    getBoard(gameID)

## print("Winner:", winner) ## for testing purposes
