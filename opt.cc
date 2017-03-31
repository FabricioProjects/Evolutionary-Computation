#include <services.h>
#include <defs.h>
#include <io-dodd.h>
#include <funcs.h>
#include <chromo.h>
#include <ga.h>
#include <cstdio>
#include <getopt.h>
#include <sstream>
#include <fstream>
#include <stdlib.h>

void print_usage(){
   using namespace std;

   cout << "Usage: opt -c <file> -l <file>" 
	<< " -m <file> -o <file>" 
	<< " -s <int> -n <int>" 
	<< " -i <int> -h"
	<< endl << endl;

   cout << "   -c, --config  Config file for the optimization."         << endl;
   cout << "   -l, --lines   File with dodd engine lines."              << endl;
   cout << "   -m, --metrics Results of the dodd engine simulation."    << endl;
   cout << "   -o, --output  File for the optimization results."        << endl;
   cout << "   -s, --step    Current walk foward step."                 << endl;
   cout << "   -n, --next    Walk foward step results will be used in." << endl;
   cout << "   -i, --iter    Iteration of the optimization process."    << endl;
   cout << "   -h, --help    Usage." << endl;
   cout << endl << endl;
}

int main(int argc, char * argv[])
{
   using namespace osi;
   using namespace std;

   // Algoritmo genetico
   GeneticAlgorithm ga;

   const struct option long_options[] =
     {
       {"config",   required_argument,  0, 'c'},
       {"lines",    required_argument,  0, 'l'},
       {"metrics",  required_argument,  0, 'm'},
       {"output",   required_argument,  0, 'o'},
       {"step",     required_argument,  0, 's'},
       {"next",     required_argument,  0, 'n'},
       {"iter",     required_argument,  0, 'i'},
       {"help",     no_argument,        0, 'h'},
       {0,0,0,0}
     };
   
   int opt = 0;
   int long_index = 0;
   string fconfig, flines, fmetrics, foutput;
   while( (opt = getopt_long(
	     argc, argv,"c:l:m:o:s:n:i:h", long_options, &long_index )) != -1 ) 
   {
     switch(opt) {
       case 'c' : fconfig  = optarg;
	 break;
       case 'l' : flines   = optarg;
	 break;
       case 'm' : fmetrics = optarg;
	 break;
       case 'o' : foutput  = optarg;
	 break;
       case 's' : ga.defs.current_step = atoi(optarg);
	 break;
       case 'n' : ga.defs.next_step = atoi(optarg);
	 break;
       case 'i' : ga.iteration = atoi(optarg);
	 break;
       case 'h' : print_usage(); return 0;
	 break;	 
       default  : print_usage(); return 1;
     }
   }

   // Verificar se os nomes de arquivos foram lidos
   if(fconfig.empty() || flines.empty() || fmetrics.empty() || foutput.empty())
   {
     print_usage(); return 1;
   }

   // Nome do arquivo de log
   ostringstream oss;
   oss << "opt-step-" << ga.defs.current_step << "-iter-" << ga.iteration
       << ".log";   
   string logname( oss.str() );

   // Abre arquivo de log para uso em toda aplicacao
   LogFile& log = LogFile::instance();
   log.open( logname );
   
   if ( log.is_open() )
   {
     // --> APENAS USADO EM TESTES
     //cout << logname << " is open..." << endl << endl;

     log.logfile() << endl << "OPTIMIZATION OF DODD ROBOT PARAMETERS..." 
		   << endl << endl;

     // 1 -- Setup
     vector<string>  nm;
     
     // Le arquivo de configuracao do otimizador
     ga.ReadConfig( fconfig, nm ); 
     ga.defs.PrintParameters();
     
     // Le dados da populacao corrente de arquivo no formato dodd-engine
     ReadDoddLines( flines, ga.defs, ga.lns );

     log.logfile() << endl << "profile: " << ga.defs.profile
		   << endl << endl;

     // --> APENAS USADO EM TESTES
     //PrintLine( ga.defs, ga.lns[0]);
     
     // Le metricas da populacao corrente de arquivo no formato dodd-engine
     ReadDoddMetrics(fmetrics, ga.defs, ga.pfs);
          
     // 2 -- Otimizacao
     
     // Define funcao de aptidao (funcao objetivo)
     // Calcula aptidao
     if( ga.fitfunc == "composite01" )
     {
       Composite01 func;
       func.minops = ga.robot_minops;
       ga.ComputeFitness( func );
     }
     else if( ga.fitfunc == "maxresultmindrawdown" )
     {
       MaxResultMinDrawDown func;
       ga.ComputeFitness( func );
     }
     else
     {
       cout << "ERROR: Fitness function was not defined!"<< endl;
     
       return 1;
     }
     
     // Incializa populacao a partir dos dados lidos
     ga.InitPopulation();
     
     // Gera nova populacao
     ga.GenerateOffspring();
     
     // Escreve os resultados em arquivo
     ga.WriteOffspring( foutput, nm );

     log.logfile() << endl << "Soft landing..." << endl << endl;

     // Fecha arquivo de log
     log.close();

     // --> APENAS USADO EM TESTES
     //cout << logname << " is closed..." << endl << endl;     
   }
   else cout << "ERROR: Unable to open file" << logname << "!"<< endl;
     
   return 0;
}
