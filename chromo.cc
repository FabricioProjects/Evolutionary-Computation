//--------------------------------------------------------------- -*- c++ -*- --
/// @addtogroup osi

///
/// @file   chromo.cc
/// @since  Jan, 26, 2015
/// @brief  Gene and chromossome
///
/// @author claudio.acsilva@gmail.com
///

#include <chromo.h>

namespace osi {
   using namespace std;

   Gene& Gene::operator=( Gene const& inst)
   {
      if( &inst != this )
      {
	 name = inst.name;
	 bh   = inst.bh;
      }
      
      return *this;
   }

   Chromossome& Chromossome::operator=( Chromossome const& inst )
   {
      if( &inst != this )
      {
	 vi = inst.vi;
	 vd = inst.vd;
	 vs = inst.vs;
	 vt = inst.vt;
      }
      
      return *this;
   }

}

//
//-- chromo.cc ----------------------------------------------------------------
