//--------------------------------------------------------------- -*- c++ -*- --
/// @addtogroup osi

///
/// @file   funcs.h
/// @since  Dec, 24, 2014
/// @brief  Functionals.
///
/// @author claudio.acsilva@gmail.com
///

#include <funcs.h>
#include <services.h>
#include <algorithm>
#include <cmath>

namespace osi {
   using namespace std;

   struct LessThan {
      bool operator()(szdb_sz_t i, szdb_sz_t j)
      {
	 return (i.first.second < j.first.second);
      }
   } lthan;
   
   struct GreaterThan {
      bool operator()(szdb_sz_t i, szdb_sz_t j)
      {
	 return (i.first.second > j.first.second);
      }
   } gthan;

   MinDrawDown::MinDrawDown(): drawdown("Drawdown") {}

   void MinDrawDown::Eval(
      Definitions& defs, vector<Performance> &pfs, vector<szdb_sz_t> &res)
   {
      res.resize( pfs.size() );

      vector<Performance>::const_iterator it;

      size_t pos = (defs.metrics.find(drawdown))->second;      

      it = pfs.begin();

      for(size_t j = 0; j < pfs.size(); j++, it++)
      {
	 res[j].first.first = it->line;
	 res[j].first.second= it->values[pos];
	 res[j].second      = j;
      }

      // Ordenar usando algoritmos do STL. Para isso tenho que definir
      // classe para o par e depois definir um comparador. Vide documentação.
      sort(res.begin(), res.end(), lthan); // ordena em ordem crescente      
   }

   MaxResult::MaxResult(): result("Tot Result") {}

   void MaxResult::Eval(
      Definitions& defs, vector<Performance> &pfs, vector<szdb_sz_t> &res)
   {
      res.resize( pfs.size() );

      vector<Performance>::const_iterator it;

      size_t posr = (defs.metrics.find(result))->second;

      it = pfs.begin();

      for(size_t j = 0; j < pfs.size(); j++, it++)
      {
	 res[j].first.first = it->line;
	 res[j].first.second= it->values[posr];
	 res[j].second      = j;
      }

      sort(res.begin(), res.end(), gthan); // ordena em ordem decrescente      
   }

   MaxResultMinDrawDown::MaxResultMinDrawDown(): 
     drawdown("Drawdown"), result("Tot Result"), dw(1.0), rw(1.0) {}

   void MaxResultMinDrawDown::Eval(
      Definitions& defs, vector<Performance> &pfs, vector<szdb_sz_t> &res)
   {
      res.resize( pfs.size() );

      vector<Performance>::const_iterator it;

      double maxd;
      double maxr;

      size_t posd = (defs.metrics.find(drawdown))->second;
      size_t posr = (defs.metrics.find(result))->second;

      for(it = pfs.begin(), maxd = it->values[posd]; it != pfs.end(); it++)
	 if(maxd < it->values[posd]) maxd = it->values[posd];

      if(maxd == 0.0) maxd = 1.0; // Apenas para prevenir divisao por zero

      for(it = pfs.begin(), maxr = it->values[posr]; it != pfs.end(); it++)
	 if(maxr < it->values[posr]) maxr = it->values[posr];

      if(maxr == 0.0) maxr = 1.0; // Apenas para prevenir divisao por zero
      
      it = pfs.begin();

      // Funcao combinando maximizacao do resultado e minimizacao do
      // drawdown. E' necessario ajustar os pesos.
      for(size_t j = 0; j < pfs.size(); j++, it++)
      {
	 res[j].first.first = it->line;
	 res[j].first.second= 
	   rw*(it->values[posr]/fabs(maxr)) - dw*(it->values[posd]/maxd);
	 res[j].second      = j;
      }

      sort(res.begin(), res.end(), gthan); // ordena em ordem decrescente      
   }

   Composite01::Composite01(): 
     drawdown("Drawdown"), result("Tot Result"), numops("Total Ops"),
     dw(1.0), rw(1.0), ow(0.25), minops(20.0) {}

   void Composite01::Eval(
      Definitions& defs, vector<Performance> &pfs, vector<szdb_sz_t> &res)
   {
      res.resize( pfs.size() );

      vector<Performance>::const_iterator it;

      double maxd;
      double maxr;

      size_t posd = (defs.metrics.find(drawdown))->second;
      size_t posr = (defs.metrics.find(result))->second;
      size_t poso = (defs.metrics.find(numops))->second;

      for(it = pfs.begin(), maxd = it->values[posd]; it != pfs.end(); it++)
	 if(maxd < it->values[posd]) maxd = it->values[posd];

      if(maxd == 0.0) maxd = 1.0; // Apenas para prevenir divisao por zero

      for(it = pfs.begin(), maxr = it->values[posr]; it != pfs.end(); it++)
	 if(maxr < it->values[posr]) maxr = it->values[posr];

      if(maxr == 0.0) maxr = 1.0; // Apenas para prevenir divisao por zero
      
      it = pfs.begin();

      // Funcao combinando maximizacao do resultado, minimizacao do
      // drawdown e penalizacao por numero de operacoes. Pode ser
      // necessario ajustar os pesos.
      //
      // Funcao de penalizacao: f = f0 +((f1-f0)/(x1-x0))(x-x0)
      //
      for(size_t j = 0; j < pfs.size(); j++, it++)
      {
	 res[j].first.first = it->line;

	 double pen;

	 if( it->values[poso] >= minops ) pen = 0.0;
	 else
	   pen = 1.0 + ((-1.0)/(minops-1.0))*(it->values[poso]-1.0);

	 res[j].first.second= 
	   rw*(it->values[posr]/fabs(maxr)) - dw*(it->values[posd]/maxd) - ow*pen;

	 res[j].second      = j;
      }

      sort(res.begin(), res.end(), gthan); // ordena em ordem decrescente      
   }

   void Print(vector<szdb_sz_t>& res, size_t sz)
   {
      if(sz > res.size()) sz = res.size();

      for(size_t j = 0; j < sz; j++)
	 cout << "("  << setw(5) << res[j].first.first+1 << ") -> " 
	      << setw(8) << res[j].first.second << endl;

      cout << endl;
   }

   void Print( Definitions& defs, std::vector<Performance>& pfs, 
	       std::vector<szdb_sz_t>& res, size_t sz )
   {
      LogFile& l = LogFile::instance();
      
      string drawdown("Drawdown"), result("Tot Result"), tops("Total Ops");

      size_t posd = (defs.metrics.find(drawdown))->second;
      size_t posr = (defs.metrics.find(result))->second;
      size_t poso = (defs.metrics.find(tops))->second;

      std::ios_base::fmtflags original_flags = std::cout.flags();   //1

      if(sz > res.size()) sz = res.size();

      l.logfile() << endl;
      l.logfile() << setw(7) << "LINE" << " -> " 
		  << setw(8) << "FUNC" << " "
		  << setw(8) << "RES"  << " "
		  << setw(8) << "DD"   << " "
		  << setw(8) << "OPS"  
		  << endl;
      for(size_t j = 0; j < sz; j++)
      {
	 l.logfile() << "("  << setw(5) << res[j].first.first+1 << ") -> " 
		     << setiosflags(ios::fixed) << setprecision(6)
		     << setw(8) << res[j].first.second << " ";
	 l.logfile().flags(original_flags);
	 l.logfile() << setw(8) << pfs[res[j].second].values[posr] << " "
		     << setw(8) << pfs[res[j].second].values[posd] << " "
		     << setw(8) << pfs[res[j].second].values[poso]
		     << endl;
      }

      l.logfile() << endl;
   }
}
//
//-- funcs.cc ------------------------------------------------------------------
