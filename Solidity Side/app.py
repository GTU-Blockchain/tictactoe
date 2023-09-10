from solcx import compile_standard, install_solc
import json
from web3 import Web3
import os
from dotenv import load_dotenv

load_dotenv()

## variables
chain_id = 11155111
public_key = os.getenv("PUBLIC_KEY")
private_key = os.getenv("PRIVATE_KEY")

install_solc("0.8.12")

with open("Solidity Side/XOXAlphaBeta.sol", "r") as file:
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

with open("compiled_code.json", "w") as file:
    json.dump(sol_file, file)

sol_abi = sol_file["contracts"]["XOXAlphaBeta.sol"]["TicTacToe"]["abi"]
sol_bytecode = sol_file["contracts"]["XOXAlphaBeta.sol"]["TicTacToe"]["evm"][
    "bytecode"
]["object"]

web3_connection = Web3(
    Web3.HTTPProvider(
        "https://eth-sepolia.g.alchemy.com/v2/" + os.getenv("ALCHEMY_API_KEY")
    )
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
    print(txn_receipt)


## create new game function
def create_new_game(GameMode):  ## !! return gameId
    newGameFunction = compiled_contract.functions.newGame(GameMode).build_transaction(
        {
            "from": public_key,
            "chainId": chain_id,
            "nonce": nonce_value,
        }
    )
    send_raw_transaction(newGameFunction)


## join game function
def join_game(gameId):
    joinGameFunction = compiled_contract.functions.joinGame(gameId).build_transaction(
        {
            "from": public_key,
            "chainId": chain_id,
            "nonce": nonce_value,
        }
    )
    send_raw_transaction(joinGameFunction)


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
    send_raw_transaction(makeMoveFunction)


## make move AI function
def make_move_ai(gameId):  ## !! return move
    makeMoveAI = compiled_contract.functions.makeMoveAI(gameId).build_transaction(
        {
            "from": public_key,
            "chainId": chain_id,
            "nonce": nonce_value,
        }
    )
    send_raw_transaction(makeMoveAI)


def getGameState(gameId):
    getGameStateFunction = compiled_contract.functions.getGameState(gameId).call()
    return getGameStateFunction


## if we have to wait for the transaction to be mined or increase nonce manually
nonce_value += 1
create_new_game(2)

## AI vs AI game
while getGameState(1) != 1:
    nonce_value += 1
    make_move_ai(1)
