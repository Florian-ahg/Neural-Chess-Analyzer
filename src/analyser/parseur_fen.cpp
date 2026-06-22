#include "parseur_fen.hpp"
#include <cctype>
#include <algorithm>

static double piece_to_value(char c)
{
  switch (c)
  {
  case 'P':
    return 0.1;
  case 'N':
    return 0.2;
  case 'B':
    return 0.3;
  case 'R':
    return 0.4;
  case 'Q':
    return 0.5;
  case 'K':
    return 0.6;
  case 'p':
    return -0.1;
  case 'n':
    return -0.2;
  case 'b':
    return -0.3;
  case 'r':
    return -0.4;
  case 'q':
    return -0.5;
  case 'k':
    return -0.6;
  default:
    return 0.0;
  }
}

static std::string extraire_plateau(const std::string &fen)
{
  std::string board;
  board.reserve(fen.size());
  for (char ch : fen)
  {
    if (ch == ' ')
      break;
    board.push_back(ch);
  }
  return board;
}

static void traduire_plateau_vers_valeurs(const std::string &board,
                                          std::vector<double> &out)
{
  for (char c : board)
  {
    if (c == '/')
    {
      continue;
    }
    else if (c >= '1' && c <= '8')
    {
      int emptyCount = c - '0';
      for (int i = 0; i < emptyCount && out.size() < 64; ++i)
      {
        out.push_back(0.0);
      }
    }
    else
    {
      out.push_back(piece_to_value(c));
      if (out.size() == 64)
      {
        break;
      }
    }
  }
}

bool convertir_fen_en_entrees(const std::string &fen,
                              const Reseau &reseau,
                              std::vector<double> &entree)
{
  if (reseau.tailles_couches.empty())
  {
    return false;
  }

  int taille_entree = reseau.tailles_couches[0];
  if (taille_entree <= 0)
  {
    return false;
  }

  std::vector<double> cases;
  cases.reserve(64);

  const std::string board = extraire_plateau(fen);
  traduire_plateau_vers_valeurs(board, cases);

  if (cases.size() < 64)
  {
    cases.resize(64, 0.0);
  }
  else if (cases.size() > 64)
  {
    cases.resize(64);
  }

  entree.clear();
  entree.resize(static_cast<std::size_t>(taille_entree), 0.0);

  const std::size_t n = std::min<std::size_t>(entree.size(), cases.size());
  for (std::size_t i = 0; i < n; ++i)
  {
    entree[i] = cases[i];
  }

  return true;
}
