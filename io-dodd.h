//--------------------------------------------------------------- -*- c++ -*- --
/// @addtogroup osi

///
/// @file   io-dodd.h
/// @since  Jan, 09, 2015
/// @brief  File input/output in dodd engine format.
///
/// @author claudio.acsilva@gmail.com
///

#ifndef IO_DODD_H_INCLUDED
#define IO_DODD_H_INCLUDED

// Std includes
#include <defs.h>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

namespace osi {

   /// Leitura de linhas de arquivo do ambiente dodd-engine.
   /// Adicionalmente le o profile (elimina a necessidade de fornecer
   /// dado redundante na entrada de dados).
   void ReadDoddLines(
      std::string& fname, Definitions& defs, std::vector<Line> &lns);

   /// Imprime linha de comando para ambiente dodd-engine
   void PrintDoddLine(std::ofstream &outfile, Definitions& defs, 
		      std::vector<std::string> &nm, Line& ln, size_t ln_idx, 
		      unsigned int iter);

   /// Leitura de linhas de arquivo do ambiente dodd-engine
   void ReadDoddMetrics(
      std::string& fname, Definitions& defs, std::vector<Performance> &pfs);
}

#endif // IO_DODD_H_INCLUDED
//
//-- io-dodd.h ----------------------------------------------------------------
