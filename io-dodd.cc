//--------------------------------------------------------------- -*- c++ -*- --
/// @addtogroup osi

///
/// @file   io-dodd.h
/// @since  Jan, 09, 2015
/// @brief  File input/output in dodd engine format.
///
/// @author claudio.acsilva@gmail.com
///

#include <io-dodd.h>
#include <defs.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <services.h>

namespace osi {
   using namespace std;
   const size_t numLines_c = 2000;// Reserva de numero para linhas do arquivo
   const size_t sz_extras  = 24;  // Reserva de numero de termos extras na linha

   void ReadDoddLines(string & fname, Definitions& defs, vector<Line> &lns)
   {
      LogFile& l = LogFile::instance();

      l.logfile() << endl << "READING LINES..." << endl;

      unsigned int count = 0;   // Contador de kinhas
      string dummy, p;
      vector<string> c;         // Lista de nomes de parametros

      // Numero de parametros a serem lidos de cada linha do
      // arquivo. Obs: alguns parametros da linha sao fornecidos como
      // "vetores" com uma unica palavra-chave: ma e sma-p
      size_t nparams = defs.params.size() - defs.sizeofma() + 1;      
      if(defs.Check("sma-p-A") && defs.Check("sma-p-B")) nparams--;
      if(defs.Check("sma-p-B") && defs.Check("sma-p-C")) nparams--;
      
      // Aloca lista de nomes de parametros
      c.resize( nparams );

      // Aloca espaco de armazenamento para dados das linhas
      lns.reserve( numLines_c );

      // Log de funcionamento
      l.logfile() << "\t" 
		  << nparams << " parameters will be read from each line..." 
		  << endl;

      map<string, Behavior>::const_iterator it;

      string line;
      ifstream myfile ( fname.c_str() );
      istringstream iss;

      if ( myfile.is_open() )
      {
	 // Log de funcionamento
	 l.logfile() << "\t" << fname << " is open..." << endl;

	 // Le todas as linha do arquivo
	 while ( getline (myfile,line) ) // Le linha
	 {
	    // Log de funcionamento
	    l.logfile() << setw(4) << count+1 << ".. "; 
	    if( !((count+1) % 10) ) l.logfile() << endl;

	    // Aumenta alocacao de dados de linhas se necessario
	    if( lns.size() < count+1 ) lns.resize( count+1 );

	    // Aloca espaco de armazenamento da linha corrente de
	    // acordo com as definicoes do problema
	    lns[count].SetSize(
	       defs.sizeofInt(), defs.sizeofDouble(), 
	       defs.sizeofString(), defs.sizeofTime());

	    
	    iss.str(line); // Joga a linha no stream para extracao de dados

	    iss >> dummy;  // dodd (comando dodd engine, na e' usado
			   // pelo otimizacor)

	    // Separa todos os comandos da linha em strings independentes
	    for(unsigned int j = 0; j < nparams; j++)
	       iss >> c[ j ];

	    // Processa cada comando separadamente
	    for(unsigned int j = 0; j < nparams; j++)
	    {
	       // Localiza o nome do comando (esta entre '--' e '=')
	       // Nao deveria ser long int?
	       unsigned int first = c[j].find("--") + 2;
	       unsigned int last  = c[j].find('=');
	       
	       if( last != string::npos ) // se existe '='
		  p = c[j].substr( first, last-first );
	       else                       // se nao existe '='
		  p = c[j].substr( first, c[j].length()-2 );

	       // Acessa definicao do parametro para ajustar o procedimento
	       // de leitura correto
	       it = defs.params.find( p );

	       // Leitura de parametros com nomes identicos ao
	       // armazenado nas definicoes
	       if( it != defs.params.end() )
	       {
		  istringstream iss;

		  // Elimina o '=' para usar os recursos o istream
		  replace(c[j].begin(), c[j].end(), '=', ' ');

		  switch(it->second.type)
		  {
		     case INT:
		     {			
			int i; iss.str(c[j]); iss >> dummy >> i;
			lns[count].pi[ it->second.idx ] = i;
			if(it->second.var) 
			   lns[count].vi.push_back( it->second.idx );
		     }
		     break;
		     case DOUBLE: 
		     {
			double d; iss.str(c[j]); iss >> dummy >> d;
			lns[count].pd[ it->second.idx ] = d;
			if(it->second.var) 
			   lns[count].vd.push_back( it->second.idx );
		     }
		     break;
		     case STRING: 
		     {
			string s; iss.str(c[j]); iss >> dummy >> s;
			lns[count].ps[ it->second.idx ] = s;
			if(it->second.var) 
			   lns[count].vs.push_back( it->second.idx );
		     }
		     break;
		     case TIME:
		     {
			unsigned int pos = c[j].find(' ');
			c[j].insert(pos+5, " ");
			c[j].insert(pos+3, " ");
			struct tm tm;
			iss.str(c[j]); 
			iss >> dummy >> tm.tm_hour >> tm.tm_min >> tm.tm_sec;
			lns[count].pt[ it->second.idx ] = tm;
			// lns[count].pt[ it->second.idx ].tm_hour = tm.tm_hour;
			// lns[count].pt[ it->second.idx ].tm_min  = tm.tm_min;
			// lns[count].pt[ it->second.idx ].tm_sec  = tm.tm_sec;
			if(it->second.var) 
			   lns[count].vt.push_back( it->second.idx );
		     }
		     break;
		     default: ;
		  }
	       }
	       else // Casos especiais: parametros "vetoriais" no arquivo
	       {
		  string ma("ma"); string smap("sma-p");
		  // 1. modifica c[j] colocando os espacos convenientes
		  // 2. faz a leitura
		  replace(c[j].begin(), c[j].end(), '=', ' ');
		  replace(c[j].begin(), c[j].end(), ',', ' ');
		  istringstream iss; iss.str(c[j]); iss >> dummy;

		  if( !p.compare(ma) )
		  {
		     for( int j = 0; j < defs.sizeofma(); j++ )
		     {
			int i; iss >> i;
			ostringstream pnm; pnm << ma << "__" << j;
			it = defs.params.find( pnm.str() );
			if( it != defs.params.end() ){
			   lns[count].pi[ it->second.idx ] = i;
			   if(it->second.var) 
			      lns[count].vi.push_back( it->second.idx );
			}
		     }
		  }
		  else if( !p.compare(smap) )
		  {
		     double d[3]; iss >> d[0] >> d[1] >> d[2];
		     char tl[4] = "ABC";
		     
		     for( int j = 0; j < 3; j++ )
		     {
			ostringstream pnm; pnm << smap << "-" << tl[j];
			it = defs.params.find( pnm.str() );
			if( it != defs.params.end() ){
			   lns[count].pd[ it->second.idx ] = d[j];
			   if(it->second.var) 
			      lns[count].vd.push_back( it->second.idx );
			}
		     }
		  }
	       }
	    }	    
	    count++;
	 }
	 myfile.close();
	 l.logfile() << "\n\t" << fname << " is closed..." << endl << endl;

	 // Determina informacoes auxiliares para interpretacao dos dados
	 if( lns.size() )
	 {
	    // Determina o profile
	    string prof = lns[ 0 ].ps[defs.params.find("profile")->second.idx];
	    ostringstream n; n << defs.current_step << "1";
	    string aux = "PROFILENAME";
	    
	    size_t p1 = prof.rfind( aux );
	    size_t p2 = prof.rfind( n.str() );

	    if( p2 == string::npos )
	    {
	      cout << endl << endl 
		   << " -- > Erro na identificacao do profile das linhas. "
		   << "Verificar os dados de entrada e os paramatros da "
		   << "linha de comando!" << endl << endl;

	      exit(EXIT_FAILURE);
	    }
	    else
	    {
	      size_t init = p1+aux.length();
	      size_t end  = p2-init;
	    
	      defs.profile = prof.substr(init, end);
	    
	      // Le o restante dos comandos da linha
	      defs.extras.reserve(24);
	      while(iss >> dummy) defs.extras.push_back( dummy );
	    }
	 }
      }
      else cout << "Unable to open file" << fname << "!"<< endl;
   }

  void PrintDoddLine(
     ofstream &outfile, Definitions& defs, vector<string> &nm, Line& ln,
     size_t ln_idx, unsigned int iter)
   {
      vector<string>::const_iterator itn;
      map<std::string, Behavior>::const_iterator it;
      bool prt_ma   = true;
      unsigned int pres = 2;

      ostringstream prof; 
      // Reescreve o profile para ter conformidade com o formato de
      // arquivo de saida e inclui um denominacao exter para indicar
      // que e' resultado de otimizador
      prof << "PROFILENAME" 
	//<< "_step" << defs.next_step << "_ga" << iter << "_"
	   << defs.profile << defs.next_step << ln_idx+1;
      it = defs.params.find( "profile" );
      ln.ps[ it->second.idx ] = prof.str();
      
      outfile << "dodd ";

      for( itn = nm.begin(); itn != nm.end(); itn++ )
      {
	 it = defs.params.find( *itn );

	 if( it != defs.params.end() )
	 {
	    if( !(*itn).compare("sma-p-A") )
	    {
	       outfile << "--sma-p=" << fixed << setprecision(pres)
		       << ln.pd[ it->second.idx ];
	       it = defs.params.find( "sma-p-B" );
	       outfile << "," <<fixed << setprecision(pres)
		       << ln.pd[ it->second.idx ];
	       it = defs.params.find( "sma-p-C" );
	       outfile << "," <<fixed << setprecision(pres)
		       << ln.pd[ it->second.idx ];
	    }
	    else if( (*itn).compare("sma-p-B") && (*itn).compare("sma-p-C") )
	    {
	       outfile << "--" << *itn;

	       switch(it->second.type)
	       {
		  case INT:
		     outfile << "=" << ln.pi[ it->second.idx ];
		     break;
		  case DOUBLE: 
		     outfile << "=" << fixed << setprecision(pres) 
			     << ln.pd[ it->second.idx ];
		     break;
		  case STRING: 
		     outfile << "=" << ln.ps[ it->second.idx ];
		     break;
		  case TIME:
		  {
		     char tt[10];
		     strftime(tt, 8, "%H%M%S", &(ln.pt[ it->second.idx ]));
		     outfile << "=" <<  tt;
		  }
		  break;
		  default: ;
	       }
	    }
	    outfile << " ";
	 }
	 else if( !(*itn).compare("ma") && prt_ma )
	 {
	    outfile << "--ma=";
	    size_t i;
	    for(i = 0; i < defs.sizeofma()-1; i++)
	    {
	       ostringstream pnm;
	       pnm << "ma__" << i; it = defs.params.find( pnm.str() );
	       outfile << ln.pi[ it->second.idx ] << ",";
	    }	    
	    
	    ostringstream pnm;
	    pnm << "ma__" << i; it = defs.params.find( pnm.str() );
	    outfile << ln.pi[ it->second.idx ];

	    prt_ma = false;
	    outfile << " ";
	 }	 
      }

      // Comando final
      ostringstream dir;

      dir << ln.ps[defs.params.find("data-dir")->second.idx ]
	  << "/output/";

      if( defs.params.find( "quarks" ) != defs.params.end() )
	dir << "quarks/";
      else if( defs.params.find( "rocket" ) != defs.params.end() )
	dir << "rocket/";

      dir << ln.ps[defs.params.find("stock")->second.idx ] << "-";

      if( ln.pi[defs.params.find("dir")->second.idx ] )
	dir << ln.ps[defs.params.find("profile")->second.idx ] << "-short";
      else
	dir << ln.ps[defs.params.find("profile")->second.idx ] << "-long";

      outfile << "&& if ls " << dir.str() 
	      << " &> /dev/null ; then echo \"\" ; else echo \"\" >  "
	      << dir.str() << " ; fi &&  scp -i dod_opt_key " << dir.str()
	      << " " << defs.extras[defs.extras.size()-1];
      

      outfile << endl;
   }

   double getMetricsValue( const string &name, string & ln )
   {
      size_t p1 = ln.find( name );
      double d;

      if(p1 != string::npos)
      {
	 size_t p2 = ln.find( ':', p1 );
	 size_t p3 = ln.find( ' ', p2+2 );

	 istringstream iss;
	 iss.str( ln.substr(p2+1, p3) ); iss >> d;
      }

      return d;
   }

   size_t getLineNumber( string & profile, string & ln )
   {
      size_t p1 = ln.rfind( profile );
      size_t n;

      if(p1 != string::npos)
      {
	 size_t p2 = ln.rfind( '-' );
	 istringstream iss;
	 iss.str( ln.substr(p1+profile.length(), p2-1) ); iss >> n;
      }

      return n;
   }

   void ReadDoddMetrics(
      std::string& fname, Definitions& defs, std::vector<Performance> &pfs)
   {
      unsigned int count = 0;
      size_t nmetrics = defs.metrics.size();

      LogFile& l = LogFile::instance();

      pfs.reserve( numLines_c );

      l.logfile() << endl << "READING METRICS..." << endl;
      l.logfile() << "\t" 
		  << nmetrics << " metrics will be read from each line..." 
		  << endl;

      string line;
      ifstream myfile ( fname.c_str() );

      if ( myfile.is_open() )
      {
	 l.logfile() << "\t" << fname << " is open..." << endl;

	 while ( getline (myfile,line) )
	 {
	    l.logfile() << setw(4) << count+1 << ".. "; 
	    if( !((count+1) % 10) ) l.logfile() << endl;

	    if( pfs.size() < count+1 ) pfs.resize( count+1 );
	    pfs[count].values.resize( nmetrics );

	    map<std::string, size_t>::const_iterator it;
	    
	    // Armazena valores das metricas
	    for(it = defs.metrics.begin(); it !=defs.metrics.end(); it++)
	    {
	       pfs[count].values[it->second] = getMetricsValue(it->first, line);
	    }
	    
	    ostringstream prof; prof << defs.profile << defs.current_step; 
	    string profile = prof.str();

	    pfs[count].line = getLineNumber( profile, line ) - 1;
	    
	    count++;
	 }
	 myfile.close();
	 l.logfile() << "\n\t" << fname << " is closed..." << endl << endl;
      }
      else cout << "Unable to open file" << fname << "!"<< endl;
   }


}

//
//-- io-dodd.cc ----------------------------------------------------------------

