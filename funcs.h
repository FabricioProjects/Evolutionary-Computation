//--------------------------------------------------------------- -*- c++ -*- --
/// @addtogroup osi

///
/// @file   funcs.h
/// @since  Dec, 24, 2014
/// @brief  Functionals.
///
/// @author claudio.acsilva@gmail.com
///

#ifndef OSI_FUNCS_H_INCLUDED
#define OSI_FUNCS_H_INCLUDED

#include <defs.h>
#include <string>

namespace osi {

   typedef std::pair<size_t, double> szdouble_t;
   typedef std::pair<szdouble_t, size_t> szdb_sz_t;

   class MinDrawDown
   {
   public:
      MinDrawDown();
      ~MinDrawDown() {}
      
      void Eval( Definitions& defs, std::vector<Performance>& pfs, 
		 std::vector<szdb_sz_t>& res );
      
   protected:
   private:
      std::string drawdown;
   };

   class MaxResultMinDrawDown
   {
   public:
      MaxResultMinDrawDown();
      ~MaxResultMinDrawDown() {}

      void Eval( Definitions& defs, std::vector<Performance>& pfs, 
		 std::vector<szdb_sz_t>& res );
      
   protected:
   private:
      std::string drawdown;
      std::string result;
      double dw, rw;
   };

   class MaxResult
   {
   public:
      MaxResult();
      ~MaxResult() {}

      void Eval( Definitions& defs, std::vector<Performance>& pfs, 
		 std::vector<szdb_sz_t>& res );
      
   protected:
   private:
      std::string result;
   };

   ///
   /// @class Composite01
   /// @brief Funcional composto: MaxResult - MinDrawdown - Penalizacao.
   ///
   /// A funca de penalizacao pune a realizacao de poucas operacoes
   /// (menos que um numero minimo especificado). A funcao de
   /// penalizacao e' linear, sendo maxima para uma (01) operacao e
   /// nula (00) para todo numero de operacoes maior que um numero
   /// minimo de operacoes especificado.
   class Composite01
   {
   public:
      Composite01();
      ~Composite01() {}

      void Eval( Definitions& defs, std::vector<Performance>& pfs, 
		 std::vector<szdb_sz_t>& res );

      double minops;
      
   protected:
   private:
      std::string drawdown;
      std::string result;
      std::string numops;
      double dw, rw, ow;
   };  

   /// Imprime resultados de avaliacao de funcionais.
   void Print(std::vector<szdb_sz_t>& res, size_t sz);

   /// Imprime resultados de avaliacao de funcionais.
   void Print( Definitions& defs, std::vector<Performance>& pfs, 
	       std::vector<szdb_sz_t>& res, size_t sz );
}

#endif // OSI_FUNCS_H_INCLUDED
//
//-- funcs.h -------------------------------------------------------------------
