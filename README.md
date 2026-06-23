# my_torch — Neural Network Chessboard Analyzer

A feed-forward **neural network built from scratch in C++17** (no PyTorch, no
TensorFlow — only the standard library) that reads a chess position in **FEN**
notation and classifies its game state:

- **Check** (White / Black)
- **Checkmate** (White / Black)
- **Nothing**

The project implements the full machine-learning pipeline by hand: a generic
neural-network engine, a network generator, a dataset translator, and an
analyzer that can **train** a network or **predict** with one.

> Epitech project `G-CNA-500` — the subject and detailed design documents are in
> [`docs/`](docs/).

---

## The neural network

A fully-connected multilayer perceptron trained with **backpropagation** and
**stochastic gradient descent**.

| Component | Choice | Why |
|-----------|--------|-----|
| Hidden activation | **ReLU** | Cheap, avoids vanishing gradients on deep stacks |
| Output activation | **Sigmoid** | Maps each class to an independent [0,1] score |
| Loss | **Mean Squared Error** | Simple, stable for this classification setup |
| Weight init | **He** (hidden) / **Xavier** (output) | Keeps signal variance stable across layers |
| Optimizer | **SGD** + shuffling + **learning-rate decay** | Escapes shallow local minima, fine-tunes late training |

Default topology (configurable through a `.conf` file):

```
INPUT 64  ->  512 (ReLU)  ->  256 (ReLU)  ->  128 (ReLU)  ->  OUTPUT (Sigmoid)
   board              hidden layers                              game-state classes
```

A board is encoded as **64 floating-point values** (one per square, signed by
piece value and color); the label is **one-hot** encoded.

---

## Build

Requires `g++` with C++17 support.

```bash
make            # builds the static lib + the 3 tools
make clean      # remove object files
make fclean     # remove objects, binaries and the library
make re         # rebuild from scratch
make tests_run  # build and run the functional tests
```

Produced artifacts:

| Binary | Role |
|--------|------|
| `my_torch_analyzer` | Train a network or predict game states |
| `my_torch_generator` | Generate a blank network from a config file |
| `my_torch_dataset_generator` | Translate FEN+label lines into input/output vectors |
| `lib_my_torch.a` | The reusable neural-network engine |

---

## Usage

### Predict (with the provided pre-trained network)

```bash
./my_torch_analyzer --predict my_torch_network.nn chessboards.txt
# Check Black
# Nothing
# Nothing
# Check White
# Nothing
```

The analyzer prints one prediction per input board, **in the same order** as the
input file.

### Train

```bash
# Generate a blank network, then train it and save the result
./my_torch_generator config.conf 1
./my_torch_analyzer --train --save my_torch_network.nn config.nn dataset.txt
```

Each training line is a FEN board followed by its expected label, e.g.:

```
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 Nothing
```

### Generate a network

```bash
./my_torch_generator config.conf 1      # config.conf = topology, one size per line
```

### Translate a dataset

```bash
./my_torch_dataset_generator dataset.txt   # FEN+label -> fen_vector / label_vector
```

> `train.sh` ties these tools together into a full train-and-evaluate run.
> Training datasets are intentionally **not** versioned (they are large); use the
> dataset generator and `train.sh` to reproduce them.

---

## Project structure

```
.
├── include/                 # Public headers (engine + analyzer + generator)
├── src/
│   ├── lib/                 # Neural-network engine
│   │   ├── Network.cpp       #   forward pass, backprop, save/load
│   │   └── Layer.cpp         #   layer, activations, weight init
│   ├── analyser/            # FEN parsing, output encoding, serialization
│   ├── generator/           # Dataset translation helpers (FEN <-> vectors)
│   └── outils/              # main() of the three executables
├── tests/                   # Functional tests (no training required)
├── docs/                    # Subject (PDF) + design docs (engine/analyzer/generator)
├── screenshots/             # Demo script & images
├── config.conf              # Example network topology
├── my_torch_network.nn      # Pre-trained network (required deliverable)
└── Makefile
```

---

## Notes

- Training is computationally heavy (large datasets, many epochs); the repository
  ships a **pre-trained** `my_torch_network.nn` so predictions work out of the box.
- Neural-network libraries (PyTorch, TensorFlow, …) are forbidden by the subject:
  everything here is implemented with the C++ standard library only.

---

## Authors

Florian Ahouangbe · Jonas Chaou · Johann-André — Epitech *Neural Network* module.
