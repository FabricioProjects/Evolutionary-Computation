//--------------------------------------------------------------- -*- c++ -*- --
/// @addtogroup osi

///
/// @file   defs.cc
/// @since  Dec, 20, 2014
/// @brief  General services: data structures.
///
/// @author claudio.acsilva@gmail.com
///

#include <services.h>
#include <defs.h>
#include <sstream>
#include <algorithm>

namespace osi {
   using namespace std;

   Behavior::Behavior(): 
      type(VOID), var(false), idx( -1 ), dmin( 0.0 ), dmax ( 0.0 ) { }

   Behavior::Behavior( Behavior const& inst)
   {
      type = inst.type; 
      var  = inst.var;
      idx  = inst.idx; 
      dmin = inst.dmin;
      dmax = inst.dmax;
   }

   Behavior::~Behavior() {}

   Behavior& Behavior::operator=( Behavior const& inst )
   {
      if( &inst != this )
      {
	 type = inst.type; 
	 var  = inst.var;
	 idx  = inst.idx; 
	 dmin = inst.dmin;
	 dmax = inst.dmax;
      }

      return *this;
   }

   Definitions::Definitions(): params(), metrics(), profile(),
			       current_step(0), next_step(0),
			       isz( 0 ), dsz( 0 ), ssz( 0 ), tsz( 0 ),
			       visz( 0 ), vdsz( 0 ), vssz( 0 ), vtsz( 0 ),
			       nma(0), msz(0), extras() {}
   Definitions::~Definitions() {}

   void Definitions::Set( 
      string & name, eTypes type, bool var, 
      double dmin, double dmax )
   {
      Set(name.c_str(), type, var, dmin, dmax);
   }

   void Definitions::Set( 
      char const* name, eTypes type, bool var, 
      double dmin, double dmax )
   {
      Behavior bh;

      bh.type = type; bh.var = var;
      bh.dmin = dmin; bh.dmax = dmax;

      switch( type )
      {
	 case INT:
	    bh.idx = isz; isz++; if( var ) visz++;
	    break;
	 case DOUBLE: 
	    bh.idx = dsz; dsz++; if( var ) vdsz++;
	    break;
	 case STRING: 
	    bh.idx = ssz; ssz++; if( var ) vssz++;
	    break;
	 case TIME:
	    bh.idx = tsz; tsz++; if( var ) vtsz++;
	    break;
	 default:
	    bh.idx = -1;
      }

      string nm = name;

      // Caso especial para o parametro 'ma': o nome do parametro e'
      // sempre o mesmo mas a chave interna e' modificada para se
      // adequar 'a estrutura necessaria 'a otimizacao.
      if( !nm.compare("ma") )
      {
	 ostringstream pnm;
	 pnm << nm << "__" << nma++;
	 nm = pnm.str();
      }
    
      params[ nm ] = bh;
   }
   void Definitions::PrintParameter( char const* name )
   {
      LogFile& l = LogFile::instance();
 
      string nm = name;

      if( !(params.find( nm ) == params.end()) )
      {
	 Behavior bh = params[ nm ];

	 l.logfile() << setw( 20 ) << nm << " : ";

	 switch( bh.type )
	 {
	    case INT:    l.logfile() << "INT   " << " : "; break;
	    case DOUBLE: l.logfile() << "DOUBLE" << " : "; break;
	    case STRING: l.logfile() << "STRING" << " : "; break;
	    case TIME:   l.logfile() << "TIME  " << " : "; break;
	    default:     l.logfile() << "VOID  " << " : "; break;
	 }
      
	 if( bh.var ) l.logfile() << "VARIABLE" << " : ";
	 else         l.logfile() << "CONSTANT" << " : ";

	 l.logfile() << "[ " << setw( 2 ) << bh.idx << " ]";

      }
      else
      {
	 l.logfile() << "Undefined parameter!";
      }
   }

   bool Definitions::Check( string& key ) const
   {
      return !(params.find( key ) == params.end());
   }

   bool Definitions::Check( char const* key ) const
   {
      string k(key);
      return Check( k );
   }

   void Definitions::PrintParameters()
   {
      LogFile& l = LogFile::instance();

      l.logfile() << endl << "ROBOT PARAMETERS..." << endl << endl;
 
      map<std::string, Behavior>::const_iterator it;

      for( it = params.begin(); it != params.end(); it++ )
      {
	 PrintParameter( it->first.c_str() );
	 l.logfile() << endl;
      }

      l.logfile() << endl;
      l.logfile() << setw( 20 ) 
		  << "INT    PARAMETERS" << " : " << setw( 2 ) << isz
		  << " : " << "VARIABLE" << " : " << setw( 2 ) << visz << endl;
      l.logfile() << setw( 20 ) 
		  << "DOUBLE PARAMETERS" << " : " << setw( 2 ) << dsz 
		  << " : " << "VARIABLE" << " : " << setw( 2 ) << vdsz << endl;
      l.logfile() << setw( 20 ) 
		  << "STRING PARAMETERS" << " : " << setw( 2 ) << ssz 
		  << " : " << "VARIABLE" << " : " << setw( 2 ) << vssz << endl;
      l.logfile() << setw( 20 ) 
		  << "TIME   PARAMETERS" << " : " << setw( 2 ) << tsz 
		  << " : " << "VARIABLE" << " : " << setw( 2 ) << vtsz << endl;
      l.logfile() << endl;
      
   }

   void Definitions::PrintMetric( const string& name )
   {
      map<std::string, size_t>::const_iterator it = metrics.find( name );

      if( !(it == metrics.end()) )
      {
	 cout << setw( 20 ) << name << " : ";

	 cout << "[ " << setw( 2 ) << it->second << " ]";

      }
      else
      {
	 cout << "Undefined metric!";
      }
   }

   void Definitions::Set( char const* name )
   {
      string nm = name;
      metrics[ nm ] = msz++;
   }

   pair<size_t,size_t> Definitions::sizeofInt()
   {
      pair<size_t,size_t> a;

      a.first = isz; a.second = visz;

      return a;
   }

   pair<size_t,size_t> Definitions::sizeofDouble()
   {
      pair<size_t,size_t> a;

      a.first = dsz; a.second = vdsz;

      return a;
   }

   pair<size_t,size_t> Definitions::sizeofString()
   {
      pair<size_t,size_t> a;

      a.first = ssz; a.second = vssz;

      return a;
   }

   pair<size_t,size_t> Definitions::sizeofTime()
   {
      pair<size_t,size_t> a;

      a.first = tsz; a.second = vtsz;

      return a;
   }

   size_t Definitions::sizeofma()
   {
      return nma;
   }

   Line::Line( Line const& inst)
   {
      pi = inst.pi; 
      pd = inst.pd; 
      ps = inst.ps;
      pt = inst.pt;

      vi = inst.vi;
      vd = inst.vd;
      vs = inst.vs;
      vt = inst.vt;
   }

   Line::~Line() {}

   Line& Line::operator=( Line const& inst)
   {
      if( &inst != this )
      {
	 pi = inst.pi; 
	 pd = inst.pd; 
	 ps = inst.ps;
	 pt = inst.pt;
	 
	 vi = inst.vi;
	 vd = inst.vd;
	 vs = inst.vs;
	 vt = inst.vt;
      }

      return *this;
   }

   void Line::SetSize( pairsz_t isz, pairsz_t dsz, pairsz_t  ssz, pairsz_t tsz )
   {
      pi.resize(isz.first); vi.reserve(isz.second);
      pd.resize(dsz.first); vd.reserve(dsz.second);
      ps.resize(ssz.first); vs.reserve(ssz.second);
      pt.resize(tsz.first); vt.reserve(tsz.second);
   }

   void SetLineValue(
      Definitions& defs, Line& ln, std::string& nm, Value &value)
   {
      map<std::string, Behavior>::const_iterator it( defs.params.find(nm) );

      if( it != defs.params.end() )
      {
	 switch(it->second.type)
	 {
	    case INT:
	       ln.pi[ it->second.idx ] = value.i;
	       if(it->second.var) ln.vi.push_back( it->second.idx );
	       break;
	    case DOUBLE: 
	       ln.pd[ it->second.idx ] = value.d;
	       if(it->second.var) ln.vd.push_back( it->second.idx );
	       break;
	    case STRING: 
	       ln.ps[ it->second.idx ] = value.s;
	       if(it->second.var) ln.vs.push_back( it->second.idx );
	       break;
	    case TIME:
	       ln.pt[ it->second.idx ] = value.t;
	       if(it->second.var) ln.vt.push_back( it->second.idx );
	       break;
	    default: ;
	 }
      }
      
   }

   void PrintLine(Definitions& defs, Line& ln)
   {
      map<std::string, Behavior>::const_iterator it;
      
      for( it = defs.params.begin(); it != defs.params.end(); it++ )
      {
	 defs.PrintParameter( it->first.c_str() );

	 switch(it->second.type)
	 {
	    case INT:
	       cout << " : " << ln.pi[ it->second.idx ];
	       break;
	    case DOUBLE: 
	       cout << " : " << ln.pd[ it->second.idx ];
	       break;
	    case STRING: 
	       cout << " : " << ln.ps[ it->second.idx ];
	       break;
	    case TIME:
	    {
	       char tt[10];
	       strftime(tt, 8, "%H%M%S", &(ln.pt[ it->second.idx ]));
	       cout << " : " << tt;
	       // cout << " : " << ln.pt[ it->second.idx ].tm_hour
	       // 	             << ln.pt[ it->second.idx ].tm_min
	       // 	             << ln.pt[ it->second.idx ].tm_sec;
	    }
	    break;
	    default: ;
	 }
	 cout << endl; 
      }
   }

   Performance::Performance(Performance const& inst)
   {
      values = inst.values;
      line   = inst.line;
   }

   Performance& Performance::operator=( Performance const& inst)
   {
      if( &inst != this )
      {
	 values = inst.values;
	 line   = inst.line;
      }

      return *this;
   }

   void PrintLineMetrics(Definitions& defs, Performance& pf)
   {
      map<std::string, size_t>::const_iterator it = defs.metrics.begin();

      cout << setw( 20 ) << "Line" << " : " << pf.line+1 << endl;
      
      for( it = defs.metrics.begin(); it != defs.metrics.end(); it++ )
      {
	 defs.PrintMetric( it->first );

	 cout << " : " << pf.values[ it->second ] << endl; 
      }
   }

}

//
//-- defs.cc ----------------------------------------------------------------
