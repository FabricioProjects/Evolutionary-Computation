//--------------------------------------------------------------- -*- c++ -*- --
/// @addtogroup osi

///
/// @file   ga.h
/// @since  Jan, 26, 2015
/// @brief  Genetic algorithm.
///
/// @author claudio.acsilva@gmail.com
///

#ifndef OSI_GA_H_INCLUDED
#define OSI_GA_H_INCLUDED

#include <defs.h>
#include <funcs.h>
#include <chromo.h>
#include <vector>
#include <string>

namespace osi {

   class GeneticAlgorithm
   {
   public:
      Definitions             defs;///< Definicoes *externas* do problema.
      std::vector<Line>       lns; ///< Caracteristicas *externas* da populacao.
      std::vector<Performance>pfs; ///< Performances calculadas *externamente*.

      unsigned int iteration; ///< Numero da iteracao
      int offspring_sz; ///< Tamanho da prole (nova populacao).
      int      pool_sz; ///< Tamanho do pool de reproducao.
      int     elite_sz; ///< Tamanho da prole gerada por elitismo.
      int    random_sz; ///< Numero de individuos gerados aleatoriamente.
      float   mut_rate; ///< Probabilidade de mutacao.
      float     mut_sz; ///< Tamanho da mutacao.
      std::string fitfunc; ///< Nome da funcao de aptidao
      float  robot_minops; ///< Numero minimo de operacoes

      // offspring_sz > pool_sz >= elite_sz

      /// Construtor padrao
      GeneticAlgorithm();

      /// Destrutor
      ~GeneticAlgorithm();

      /// Inicializa populacao com dados de definicoes e
      /// linhas. Definicoes e Linhas ja devem estar inicializadas.
      void InitPopulation();

      /// Calcula aptidao da populacao corrente usando informacoes de
      /// performance ja inicializadas.
      template<class FitnessFunction>
      void ComputeFitness( FitnessFunction ff );

      /// Gera nova geracao descendente da populacao corrente.
      void GenerateOffspring();

      /// Le configuracao de problema de arquivo
      void ReadConfig( std::string  &fname, std::vector<std::string> &nm );

      /// Retorna nova geracao em arquivo
      void WriteOffspring( std::string  &fname, std::vector<std::string> &nm );

   protected:
   private:   

      /// Seleciona conjunto de individuos para cruzamento. Metodo
      /// steady state sem vies (selecao simples por ranqueamento).
      void Selection();

      /// Os melhores indiviuos passam diretamente para a proxima
      /// geracao.
      void Elitism();

      /// Cruzamento uniforma com metade dos genes vindo de cada
      /// par. Os genes a serem trocados sao selecionados por sorteio.
      void Crossover();

      /// Sorteia e aplica mutacao.
      void Mutation();

      /// Copia a gene g do cromossomo in ara o cromossomo out
      void CopyGene(Gene &g, Chromossome &in, Chromossome &out);

      void MutateGene(Gene &g, Chromossome &c);

      std::vector<Gene>             chdef;///< Lista de genes de um cromossomo
      std::vector<Chromossome> population;///< Lista de cromossomos da populacao
      std::vector<szdb_sz_t>      fitness;///< Lista de aptidoes ja ordenadas
      std::vector<size_t>            pool;///< Pool de reproducao      

      std::vector<Chromossome>  offspring;///< Prole: resultado
   };

   template<class FitnessFunction>
   void GeneticAlgorithm::ComputeFitness( FitnessFunction ff )
   {
      // Calcula funcao de aptidao e ordena do melhor para o pior.
      ff.Eval( defs, pfs, fitness );
   }
   
}


#endif // OSI_GA_H_INCLUDED
//
//-- ga.h ----------------------------------------------------------------
