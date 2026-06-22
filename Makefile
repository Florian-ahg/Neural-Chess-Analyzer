NAME_ANALYZER  = my_torch_analyzer
NAME_GENERATOR = my_torch_generator
LIB_NAME       = lib_my_torch.a

CC       = g++
CXXFLAGS = -I./include -W -Wall -Wextra -std=c++17 -O3
AR       = ar rcs

# Sources de la bibliothèque my_torch (tous les .cpp sauf les main)
SRC_LIB = src/lib/Network.cpp \
          src/lib/Layer.cpp \
          src/analyser/reseau.cpp \
          src/analyser/serialisation_reseau.cpp \
          src/analyser/parseur_fen.cpp \
          src/analyser/encodage_sorties.cpp

OBJ_LIB = $(SRC_LIB:.cpp=.o)

# Sources des binaires (seulement les main)
SRC_ANALYZER  = src/outils/main_analyzer.cpp
OBJ_ANALYZER  = $(SRC_ANALYZER:.cpp=.o)

SRC_GENERATOR = src/outils/main_generator.cpp
OBJ_GENERATOR = $(SRC_GENERATOR:.cpp=.o)

all: $(LIB_NAME) $(NAME_ANALYZER) $(NAME_GENERATOR)

# Création de la bibliothèque statique
$(LIB_NAME): $(OBJ_LIB)
	$(AR) $(LIB_NAME) $(OBJ_LIB)

# Compilation de l'Analyzer (linké avec la lib)
$(NAME_ANALYZER): $(LIB_NAME) $(OBJ_ANALYZER)
	$(CC) -o $(NAME_ANALYZER) $(OBJ_ANALYZER) -L. -l_my_torch

# Compilation du Generator (linké avec la lib)
$(NAME_GENERATOR): $(LIB_NAME) $(OBJ_GENERATOR)
	$(CC) -o $(NAME_GENERATOR) $(OBJ_GENERATOR) -L. -l_my_torch

# Règles de compilation
%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_LIB) $(OBJ_ANALYZER) $(OBJ_GENERATOR)

fclean: clean
	rm -f $(NAME_ANALYZER) $(NAME_GENERATOR) $(LIB_NAME)

re: fclean all

.PHONY: all clean fclean re