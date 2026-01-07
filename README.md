# Baby Tetris
This is an university project (in collaboration with [Seçkin Yağmur Ergin](https://github.com/xaelxx14)) that aims to create a MDP model for the Tetris game.
The goal of such a model is to compute in advance the optimal strategy (maximizing the game score)

## Usage
0. Requirements:
    - gcc
    - make

You can either install them individually or use our `shell.nix` script that install automatically all necessary dependencies by running:
```bash
nix-shell shell.nix
```
1. Clone this git repository :
```bash
git clone git@github.com:diaarca/baby-tetris.git
```
2. Navigate to the project directory :
```bash
cd baby-tetris
```
3. Build the project :
```bash
make
```
4. Run the baby-tetris :
```bash
make run
```
or :
```bash
./bin/tetris
```
