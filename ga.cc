/// @addtogroup osi

///
/// @file   ga.cc
/// @since  Jan, 26, 2015
/// @brief  Genetic algorithm
///
/// @author claudio.acsilva@gmail.com
///

#include <ga.h>
#include <defs.h>
#include <services.h>
#include <io-dodd.h>
#include <string>
#include <map>
#include <set>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <pugixml.hpp>

namespace osi {
   using namespace std;
   const size_t numParams_c = 40; // Reserva de numero de parametros

   /// Geracao de numero aleatorio em ambiente compativel com
   /// POSIX. Verificar na referencia abaixo para codigo especifico
   /// para WIndows.
   ///
   /// www.securecoding.cert.org/confluence/secode/MSC30-C.+Do+not+use+the+rand()+function+for+generating+pseudorandom+numbers
   /// man7.otg/linux/amn-pages/man3/random.3.html
   // int randomGen()
   // {
   //    struct timespec ts;
   //    timespec_get( &ts, TIME_UTC);
   //    srandom(ts.tv_nsec ^ ts.tv_sec);
   //    return random();
   // }

   /// Geracao de real aleatório em [0,1]
   /// forums.codeguru.com/showthread.php?351834-how-do-i-generate-a-random-float-between-0-and-1
   //RAND_MAX + 1 resultou em integer overflow pois na glibc RAND_MAX
   //e' o maior inteiro representavel em 32 bits
   //const unsigned long RAND_MAX_PLUS1 = RAND_MAX+1;
   const unsigned long RAND_MAX_PLUS1 = RAND_MAX;
   const unsigned long BIG_RAND_MAX   = RAND_MAX_PLUS1 * RAND_MAX_PLUS1 -1;
   unsigned long bigRand() { return(random() * (RAND_MAX_PLUS1) + random()); }
   float rand0to1(){return static_cast<float>(bigRand())/BIG_RAND_MAX; }
   
   GeneticAlgorithm::GeneticAlgorithm(): 
      defs(), lns(), pfs(), iteration( 0 ), chdef(), population(), fitness(), 
      pool(), offspring(), offspring_sz( 50 ), pool_sz( 20 ), elite_sz( 10 ), 
      random_sz( 0 ), mut_rate(0.01), mut_sz(0.05), fitfunc(), 
      robot_minops( 20.0) {}

   
   GeneticAlgorithm::~GeneticAlgorithm() {}

   void GeneticAlgorithm::InitPopulation()
   {
      LogFile& l = LogFile::instance();
      l.logfile() << endl << " INIT POPULATION..." << endl << endl;

      // Determicacao do tamanho do cromossomo: soma de todos os
      // parametros variaveis
      size_t chsz = 0;  // numero de genes
      pairsz_t vsz;
      vsz = defs.sizeofInt();    chsz += vsz.second;
      vsz = defs.sizeofDouble(); chsz += vsz.second;
      vsz = defs.sizeofString(); chsz += vsz.second;
      vsz = defs.sizeofTime();   chsz += vsz.second;
      chdef.resize ( chsz );

      // Determinacao das definicoes do cromossomo
      map<string, Behavior>::const_iterator it; size_t i = 0;
      size_t  isz = 0,  dsz = 0,  ssz = 0,  tsz = 0;

      for( it = defs.params.begin(); it != defs.params.end(); it++ )
      {
	 if( it->second.var ) // Copia e corrige dados dos parametros variaveis
	 {
	    chdef[ i ].name = it->first;
	    chdef[ i ].bh   = it->second;

	    // Correcao do indice para representar a posicao nos vetores
	    // de dados do cromossomo.
	    switch( chdef[ i ].bh.type )
	    {
	       case INT:    chdef[ i ].bh.idx = isz; isz++; break;
	       case DOUBLE: chdef[ i ].bh.idx = dsz; dsz++; break;
	       case STRING: chdef[ i ].bh.idx = ssz; ssz++; break;
	       case TIME:   chdef[ i ].bh.idx = tsz; tsz++; break;
	       default: ;
	    }
	    i++;
	 }
      }

      // A populacao e' igual ao numero de linhas de dados fornecidas
      size_t  psz = lns.size();
      population.resize( psz );      

      // Inicializa dados da populacao
      for(size_t j = 0; j < psz; j++)
      {
	 // Inicializa dimensoes do individuo
	 population[ j ].vi.resize( lns[ j ].vi.size() );
	 population[ j ].vd.resize( lns[ j ].vd.size() );
	 population[ j ].vs.resize( lns[ j ].vs.size() );
	 population[ j ].vt.resize( lns[ j ].vt.size() );

	 // Copia valores dos genes do individuo
	 for(size_t k = 0; k < chsz; k++)
	 {
	    it = defs.params.find( chdef[ k ].name );

	    switch( chdef[ k ].bh.type )
	    {
	       case INT: 
		  population[ j ].vi[ chdef[ k ].bh.idx ] = 
		     lns[ j ].pi[ it->second.idx ]; break;
	       case DOUBLE: 
		  population[ j ].vd[ chdef[ k ].bh.idx ] = 
		     lns[ j ].pd[ it->second.idx ]; break;
	       case STRING: 
		  population[ j ].vs[ chdef[ k ].bh.idx ] = 
		     lns[ j ].ps[ it->second.idx ]; break;
	       case TIME: 
		  population[ j ].vt[ chdef[ k ].bh.idx ] = 
		     lns[ j ].pt[ it->second.idx ]; break;
	    }
	 }
      }

      Print( defs, pfs, fitness, psz );
   }
   
   void GeneticAlgorithm::GenerateOffspring()
   {
      LogFile& l = LogFile::instance();
      l.logfile() << endl << " GENERATE OFFSPRING..." << endl << endl;

      // Inicializa aramzenamento da nova populacao
      offspring.resize( offspring_sz );
      pool.resize( pool_sz );

      // Inicializa dimensoes do cromossomo da prole
      for(size_t j = 0; j < offspring_sz; j++)
      {
	 offspring[ j ].vi.resize( population[ 0 ].vi.size() );
	 offspring[ j ].vd.resize( population[ 0 ].vd.size() );
	 offspring[ j ].vs.resize( population[ 0 ].vs.size() );
	 offspring[ j ].vt.resize( population[ 0 ].vt.size() );
      }

      //--
      l.logfile() << endl << " \tREPRODUCTION POOL..." << endl;
      Print( defs, pfs, fitness, pool_sz );
      //--

      // Seed do random
      struct timespec ts;
      timespec_get( &ts, TIME_UTC);
      srandom(ts.tv_nsec ^ ts.tv_sec);

      // Gera nova populacao descendente da populacao corrente
      Elitism();
      Selection();
      Crossover();
      Mutation();
   }

   void GeneticAlgorithm::Elitism()
   {
      LogFile& l = LogFile::instance();
      l.logfile() << endl << " \tELISTISM..." << endl;

      // Os individuos gerados por elitismo ficam no topo a
      // lista. Apenas por questao de organizacao. Nao ha nenhum
      // motivo especial para isso.
      for( size_t i = 0; i < elite_sz; i++ )
	 offspring[ i ] = population[ fitness[i].first.first ];
   }

   void GeneticAlgorithm::Selection()
   {
      LogFile& l = LogFile::instance();
      l.logfile() << endl << " \tSELECTION..." << endl;

      for( size_t i = 0; i < pool_sz; i++ )
	 pool[ i ] = fitness[i].first.first;
   }

   void GeneticAlgorithm::Crossover()
   {
      LogFile& l = LogFile::instance();
      l.logfile() << endl << " \tCROSSOVER..." << endl;

      // Numero de cruzamentos
      int xover_sz = offspring_sz - elite_sz - random_sz;

      // Numero de genes do pai e da mae no cruzamento
      size_t ngen  = chdef.size();
      size_t ngen1 = ngen / 2;
      size_t ngen2 = ngen - ngen1;

      //--
      l.logfile() << " \t\tNumber of crossovers: " << xover_sz << endl;
      l.logfile() << " \t\tNumber of genes     : " << ngen
		  << " : [F: " << ngen1 << " / M: " << ngen2 << "]" << endl;
      //--

      for( size_t i = 0; i < xover_sz; i++ )
      {
	 // Sorteia pares
	 size_t id1 = random() % pool_sz; //numero em [0, pool_sz)       <-- PAI
	 size_t id2; do{id2 = random() % pool_sz;} while( id2 == id1);// <-- MAE

	 //--
	 l.logfile() << endl 
		     << "\t\tCrossover: " << i+1 << "..." << endl;
	 l.logfile() << "\t\tF: pool[" << id1 << "] --> Indiv: " << pool[id1]+1;
	 l.logfile() << " / M: pool[" << id2 << "] --> Indiv: " << pool[id2]+1 
		     << endl;
	 //--

	 set<size_t> gen1, gen2;
	 set<size_t>::iterator it;
	 gen1.clear(); gen2.clear();
	 
	 // Sorteia genes do pai: set elimina indices repetidos
	 while( gen1.size() < ngen1 ) gen1.insert( random() % ngen );

	 // O complemento vem da mae
	 for( size_t j = 0; j < ngen; j++ ) gen2.insert( j );
	 for( it = gen1.begin(); it != gen1.end(); it++ ) gen2.erase( *it );

	 //--
	 l.logfile() << "\t\tGenes F:[";
	 for( it = gen1.begin(); it != gen1.end(); it++ ) 
	    l.logfile() << *it << ", ";
	 l.logfile() <<  "]" << endl;
	 l.logfile() << "\t\tGenes M:[";
	 for( it = gen2.begin(); it != gen2.end(); it++ ) 
	    l.logfile() << *it << ", ";
	 l.logfile() <<  "]" <<  endl;
	 l.logfile() <<  endl;
	 //--

	 // Gera novo individuo
	 for( it = gen1.begin(); it != gen1.end(); it++ ) // Genes do pai
	    CopyGene( 
	       chdef[ *it ], population[pool[id1]], offspring[elite_sz + i] );

	 for( it = gen2.begin(); it != gen2.end(); it++ ) // Genes da mae
	    CopyGene( 
	       chdef[ *it ], population[pool[id2]], offspring[elite_sz + i] );
      }
   }

   void GeneticAlgorithm::Mutation()
   {
      LogFile& l = LogFile::instance();
      l.logfile() << endl << " \tMUTATION..." << endl;

      int xover_sz = offspring_sz - random_sz; // para usar em
					       // contador que comeca
					       // em elite_sz

      size_t ngen  = chdef.size();

      for( size_t i = elite_sz; i < xover_sz; i++ )
      {
	 //--
	 l.logfile() << endl << "\t\tMutation of indiv: " << i+1 << "..." 
		     << endl;
	 l.logfile() << "\t\t";
	 //--

	 for( size_t j = 0; j < ngen; j++ )
	 {
	    float dice = rand0to1();
	    if(dice <= mut_rate) 
	    {
	       l.logfile() << "G[" << j << "] ";
	       MutateGene(chdef[j], offspring[i]);
	    }
	 }
	 
	 l.logfile() << endl;
      }
   }

   void GeneticAlgorithm::CopyGene(Gene &g, Chromossome &in, Chromossome &out)
   {
      size_t idx = g.bh.idx;

      switch( g.bh.type )
      {
	 case INT:    out.vi[ idx ] = in.vi[ idx ]; break;
	 case DOUBLE: out.vd[ idx ] = in.vd[ idx ]; break;
	 case STRING: out.vs[ idx ] = in.vs[ idx ]; break;
	 case TIME:   out.vt[ idx ] = in.vt[ idx ]; break;
	 default: ;
      }
   }

   void GeneticAlgorithm::MutateGene(Gene &g, Chromossome &c)
   {
      size_t idx = g.bh.idx;
      int s      = random() % 2; if(!s) s = -1; // sinal do delta
      int m_sz   = random() % 3 + 1;            // tres niveis de mutacao

      // O valor base da mutacao e' dado por mut_sz * (valor do
      // gene). Esse valor é multiplicado pelo nivel de mutacao [1,3]
      // e pelo sinal (pode-se somar o subtrair o valor da
      // mutacao). Em seguida, verifica-se se o valor extrapolou os
      // limites de variacao e faz-se a correcao necessaria para o
      // limite admissel.
      switch( g.bh.type )
      {
	 case INT:    
	 {
	    c.vi[ idx ] += static_cast<int>(s*m_sz*mut_sz*c.vi[ idx ]); 
	    if( c.vi[ idx ] < g.bh.dmin )
	       c.vi[ idx ] = static_cast<int>(g.bh.dmin);
	    else if ( c.vi[ idx ] > g.bh.dmax )
	       c.vi[ idx ] = static_cast<int>(g.bh.dmax);
	 }
	 break;
	 case DOUBLE:    
	 {
	    c.vd[ idx ] += s*m_sz*mut_sz*c.vd[ idx ]; 
	    if( c.vd[ idx ] < g.bh.dmin )
	       c.vd[ idx ] = g.bh.dmin;
	    else if ( c.vi[ idx ] > g.bh.dmax )
	       c.vd[ idx ] = g.bh.dmax;
	 }
	 break;

	 default: ;
      }
   }

   void GeneticAlgorithm::ReadConfig( string  &fname, vector<string> &nm )
   {
      LogFile& l = LogFile::instance();
      l.logfile() << endl << "READING OPTIMIZATION SETUP..." << endl << endl;

      istringstream iss; 

      pugi::xml_document doc;

      pugi::xml_parse_result myfile = doc.load_file(fname.c_str());

      // Leitura de parametros de otimizacao
      pugi::xml_node ga = doc.child("DOD_OPT").child("GA");
      iss.str(ga.child_value("OFFSPRING_SZ")); iss >> offspring_sz;
      iss.str(ga.child_value("POOL_SZ"));      iss >> pool_sz;
      iss.str(ga.child_value("ELITE_SZ"));     iss >> elite_sz;
      iss.str(ga.child_value("RANDOM_SZ"));    iss >> random_sz;
      iss.str(ga.child_value("MUT_RATE"));     iss >> mut_rate;
      iss.str(ga.child_value("MUT_SZ"));       iss >> mut_sz;
      fitfunc = ga.child("FITNESS").attribute("NAME").value();
      if( fitfunc == "composite01")
	robot_minops = ga.child("FITNESS").child("MINOPS").text().as_double();

      // Leitura de parametros de simulacao
      pugi::xml_node params = 
	 doc.child("DOD_OPT").child("SIMULATION").child("PARAMETERS");

      nm.reserve( numParams_c );
      unsigned int i = 0; eTypes type;  bool var; double min, max;
      string s, si("INT"), sd("DOUBLE"), ss("STRING"), st("TIME");

      for(pugi::xml_node_iterator it = params.begin(); it != params.end(); ++it)
      {
	 nm.push_back( it->attribute("NAME").value() );
	 s   = it->attribute("TYPE").value();
	 var = it->attribute("VAR").as_bool();

	 if( s == si ) type = INT;
	 else if ( s == sd ) type = DOUBLE;
	 else if ( s == ss ) type = STRING;
	 else if ( s == st ) type = TIME;
	 else type = VOID;

	 if( var )
	 {
	    iss.str( it->child_value("MIN") ); iss >> min;
	    iss.str( it->child_value("MAX") ); iss >> max;
	    defs.Set(nm[i++], type, var, min, max);
	    
	 }
	 else defs.Set(nm[i++], type, var);
      }

      // Leitura de nomes de metricas
      pugi::xml_node mets = 
	 doc.child("DOD_OPT").child("SIMULATION").child("METRICS");

      for(pugi::xml_node_iterator it = mets.begin(); it != mets.end(); ++it)
	 //defs.Set( it->text().get() );
	 defs.Set( it->attribute("NAME").value() );

      l.logfile() << endl << "OPTIMIZATION SETUP OK..." << endl << endl;
   }

   void GeneticAlgorithm::WriteOffspring( string  &fname, vector<string> &nm )
   {
      LogFile& l = LogFile::instance();
      l.logfile() << endl << "WRITING OFFSPRING..." << endl;

      ofstream resfile( fname.c_str() );

      if ( resfile.is_open() )
      {
	 l.logfile() << "\n\t" << fname << " is open..." << endl;
	 // cout << "\n\t" << fname << " is open..." << endl;

	 //Cria uma linha
	 Line ln;
	 ln.SetSize( 
	    defs.sizeofInt(), defs.sizeofDouble(), 
	    defs.sizeofString(), defs.sizeofTime() );

	 //Copia todos os parametros de uma linha (depois altero
	 //apenas os variaveis)
	 ln = lns[ 0 ];

	 size_t ngen = chdef.size();

	 map<std::string, Behavior>::const_iterator it;

	 for( size_t i = 0; i < offspring_sz; i++ )
	 {
	    for( size_t j = 0; j < ngen; j++ )
	    {
	       // Substitui cada gene na linha: A partir do nome do
	       // gene, localiza dados no defs. Copia os dados do gene
	       // na linha.
	       it = defs.params.find( chdef[ j ].name );

	       size_t ln_idx = it->second.idx;
	       size_t gn_idx = chdef[ j ].bh.idx;

	       switch( it->second.type )
	       {
		  case INT:
		     ln.pi[ ln_idx ] = offspring[ i ].vi[ gn_idx ];
		     break;
		  case DOUBLE: 
		     ln.pd[ ln_idx ] = offspring[ i ].vd[ gn_idx ];
		     break;
		  case STRING: 
		     ln.ps[ ln_idx ] = offspring[ i ].vs[ gn_idx ];
		     break;
		  case TIME:
		     ln.pt[ ln_idx ] = offspring[ i ].vt[ gn_idx ];
		     break;
		  default: ;
	       }	       
	    }

	    // Imprime a linha
	    PrintDoddLine(resfile, defs, nm, ln, i, iteration);
	 }	 

	 resfile.close();

	 l.logfile() << "\n\t" << fname << " is closed..." << endl << endl;
	 //cout << "\n\t" << fname << " is closed..." << endl << endl;
      }
      else cout << "Unable to open file" << fname << "!"<< endl;
   }
}

//
//-- ga.cc ----------------------------------------------------------------
