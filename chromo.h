//--------------------------------------------------------------- -*- c++ -*- --
/// @addtogroup osi

///
/// @file   chromo.h
/// @since  Jan, 26, 2015
/// @brief  Gene and chromossome.
///
/// @author claudio.acsilva@gmail.com
///

#ifndef OSI_CHROMO_H_INCLUDED
#define OSI_CHROMO_H_INCLUDED

#include <defs.h>

namespace osi {

   ///
   /// Definicao de Gene: nome do parametro e seu "significado" para
   /// permitir a localizacao de seu valor numerico no cromossomo.
   ///
   struct Gene
   {
      std::string name; ///< Nome externo do parametro
      Behavior bh;      ///< Significado/descricao do parametro

      Gene(): name(), bh() {}

      ~Gene() {}

      Gene& operator=( Gene const& inst );
   };

   ///
   /// @class Chromossome
   /// @brief Parametros otimizaveis por algoritmo genetico.
   ///
   class Chromossome
   {
   public:
      std::vector<int>         vi; ///< Parametros inteiros
      std::vector<double>      vd; ///< Parametros reais
      std::vector<std::string> vs; ///< Parametros de texto
      std::vector<std::tm>     vt; ///< Parametros de tempo

      Chromossome(): vi(), vd(), vs(), vt() {}

      ~Chromossome() {}

      Chromossome& operator=( Chromossome const& inst ); 

   protected:
   private:

   };

}


#endif // OSI_CHROMO_H_INCLUDED
//
//-- chromo.h ----------------------------------------------------------------
