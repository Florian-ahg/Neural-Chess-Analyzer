#include "encodage_sorties.hpp"
#include <cctype>

std::vector<double> label_vers_sortie(const std::string &label)
{
  std::vector<double> out(5, 0.0);

  bool has_checkmate = (label.find("Checkmate") != std::string::npos);
  bool has_check = (label.find("Check") != std::string::npos);
  bool has_white = (label.find("White") != std::string::npos);
  bool has_black = (label.find("Black") != std::string::npos);

  if (has_checkmate)
  {
    if (has_white)
      out[0] = 1.0; 
    else if (has_black)
      out[1] = 1.0; 
    else
      out[0] = 1.0;
  }
  else if (has_check)
  {
    
    if (has_white)
      out[2] = 1.0;
    else if (has_black)
      out[3] = 1.0;
    else
      out[2] = 1.0;
  }
  else
  {
    out[4] = 1.0;
  }

  return out;
}

std::string sortie_vers_label(const std::vector<double> &sortie)
{
  if (sortie.empty())
  {
    return "Nothing";
  }

  std::size_t idx_max = 0;
  double val_max = sortie[0];

  for (std::size_t i = 1; i < sortie.size(); ++i)
  {
    if (sortie[i] > val_max)
    {
      val_max = sortie[i];
      idx_max = i;
    }
  }

  switch (idx_max)
  {
    case 0: return "Checkmate White";
    case 1: return "Checkmate Black";
    case 2: return "Check White";
    case 3: return "Check Black";
    case 4: return "Nothing";
    default: return "Nothing";
  }
}
