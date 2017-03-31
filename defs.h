//--------------------------------------------------------------- -*- c++ -*- --
/// @addtogroup osi

///
/// @file   defs.h
/// @since  Dec, 20, 2014
/// @brief  General services: data structures.
///
/// @author claudio.acsilva@gmail.com
///

#ifndef OSI_DEFS_H_INCLUDED
#define OSI_DEFS_H_INCLUDED

// Std includes
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <ctime>

namespace osi {

   ///
   /// @class eTypes
   /// @brief Tipos de parametros.
   ///
   enum eTypes {VOID = -1, INT, DOUBLE, STRING, TIME};
  
  
   ///
   /// @class Behavior
   /// @brief Todas as informacoes uteis para a manipulacao de parametros.
   ///
   class Behavior
   {
   public:
      eTypes type;       ///< Tipo 
      bool   var;        ///< Variavel/constante -> true/false
      int    idx;        ///< Posicao no array de seu tipo
      double dmin, dmax; ///< limites; testar em real apenas

      Behavior();
      Behavior( Behavior const& );
      ~Behavior();

      Behavior& operator=( Behavior const& );

   protected:
   private:
    
   };

   typedef std::pair<size_t,size_t> pairsz_t;

   ///
   /// @class Definitions
   /// @brief Definicao de parametros e metricas. Nao armazena os valores
   ///
   class Definitions
   {
   public:
      /// Definicao de parametros: mapa entre o nome usado nos
      /// arquivos de configuracao das simulacoes e a descricao
      /// interna.
      std::map<std::string, Behavior> params;

      /// Definicao das metricas: mapa entre o nome usado nos arquivos
      /// de configuracao e a posicao no array de metricas.
      std::map<std::string, size_t>   metrics;

      /// Denominacao do tipo de simulacao tal como aparece nos
      /// resultados.
      std::string profile;

      /// Estagio corrente de walkforwad e proximo estagio para o qual
      /// serao gerados dados de simulacao. Podem ser iguais.
      unsigned int current_step, next_step;

      /// Comandos de sistema que aparecem em linha de dados
      /// dod-engine e que podem ser necessarios para impressao de
      /// resultados
      std::vector<std::string> extras;
    
      Definitions();
      ~Definitions();

      /// Define parametro
      void Set( std::string & name, eTypes type, bool var,
		double dmin = 0.0, double dmax = 0.0 );

      /// Define parametro
      void Set( char const* name, eTypes type, bool var, 
		double dmin = 0.0, double dmax = 0.0 );

      /// Testa a existencia do parametro
      bool Check( std::string& key ) const;

      /// Testa a existencia do parametro
      bool Check( char const* key ) const;

      void PrintParameter( char const* name ); 

      void PrintParameters();

      void PrintMetric( const std::string& name );

      /// Define metricas.
      void Set( char const* name );

      /// Retorna o par <numero total de parametros, numero de variaveis>.
      pairsz_t sizeofInt();

      /// Retorna o par <numero total de parametros, numero de variaveis>.
      pairsz_t sizeofDouble();

      /// Retorna o par <numero total de parametros, numero de variaveis>.
      pairsz_t sizeofString();

      /// Retorna o par <numero total de parametros, numero de variaveis>.
      pairsz_t sizeofTime();

      /// Retorna numero de medias moveis.
      size_t   sizeofma();
    
   protected:
   private:
      size_t  isz,  dsz,  ssz,  tsz; ///< Numero de cada tipo de parametro.
      size_t visz, vdsz, vssz, vtsz; ///< Numero de parametros variaveis.
      size_t nma;                    ///< Numero de medias moveis.
      size_t msz;                    ///< Numero de metricas.
   };

   ///
   /// @class Line
   /// @brief Analogia com a linha de arquivo de simulacao.
   /// Aqui sao armazenados os valores dos parametros.
   class Line
   {
   public:
      std::vector<int>         pi; ///< Parametros inteiros
      std::vector<double>      pd; ///< Parametros reais
      std::vector<std::string> ps; ///< Parametros de texto
      std::vector<std::tm>     pt; ///< Parametros de tempo

      std::vector<size_t>      vi, vd, vs, vt; ///< Indice das variaveis.

      Line(): pi(), pd(), ps(), pt(),
	      vi(), vd(), vs(), vt() {}

      Line( Line const& );

      ~Line();

      Line& operator=( Line const& );

      /// Alocacao de memoria para uma linha de parametros
      void SetSize( pairsz_t isz, pairsz_t dsz, pairsz_t  ssz, pairsz_t tsz );

   protected:      
   private:

   };

   /// Definido apenas para simplificar a inicializacao de parametros
   /// das linhas.
   struct Value
   {
      int i; double d; std::string s; std::tm t;
   };

   /// Define o valor de um parametro da linha. Depende de
   /// inicializacao previa do conjunto de definicoes.
   void SetLineValue(
      Definitions& defs, Line& ln, std::string & nm, Value &v);

   void PrintLine(Definitions& defs, Line& ln);

   ///
   /// @class Performance
   /// @brief Lista de metricas de performance calculadas externamente.
   ///
   class Performance
   {
   public:
      std::vector<double> values;
      size_t line;
      
      Performance(): values(), line(-1) {}
      Performance(Performance const&);
      ~Performance() {}

      Performance& operator=( Performance const& );
      
   protected:
   private:
   };

   void PrintLineMetrics(Definitions& defs, Performance& pf);

}

#endif // OSI_DEFS_H_INCLUDED
//
//-- defs.h ----------------------------------------------------------------
