// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <defs.h>
#include <fstream>
#include <sstream>

#include <cstdlib>
#include <ctime>

using namespace std;
using namespace osi;

const unsigned long RAND_MAX_PLUS1 = RAND_MAX;
const unsigned long BIG_RAND_MAX   = RAND_MAX_PLUS1 * RAND_MAX_PLUS1 -1;
unsigned long bigRand() { return(random() * (RAND_MAX_PLUS1) + random()); }
float rand0to1(){return static_cast<float>(bigRand())/BIG_RAND_MAX; }

int main () {

  // Definitions def;

  // vector<string> nm; 
  // nm.reserve(40);

  // unsigned int i = 0;
   
  // nm.push_back("t-init");          def.Set(nm[i++], INT,    false);
  // nm.push_back("min-ret");         def.Set(nm[i++], INT,    false);
  // nm.push_back("gmi-avg-days");    def.Set(nm[i++], INT,    false);
  // nm.push_back("simulation");      def.Set(nm[i++], VOID,   false);
  // nm.push_back("data-dir");        def.Set(nm[i++], STRING, false);
  // nm.push_back("block-time");      def.Set(nm[i++], INT,    false);
  // nm.push_back("dir");             def.Set(nm[i++], INT,    false);
  // nm.push_back("sma-max-sl");      def.Set(nm[i++], DOUBLE, false, 5.0,   6.0);
  // nm.push_back("sma-sg");          def.Set(nm[i++], DOUBLE, false, 2.5, 300.0);
  // nm.push_back("sma-wgap");        def.Set(nm[i++], DOUBLE, false, 1.0,   1.5);
  // nm.push_back("quarks");          def.Set(nm[i++], VOID,   false);
  // nm.push_back("stock");           def.Set(nm[i++], STRING, false);
  // nm.push_back("sma-trade-start"); def.Set(nm[i++], TIME,   false);
  // nm.push_back("sma-trade-stop");  def.Set(nm[i++], TIME,   false);
  // nm.push_back("profile");         def.Set(nm[i++], STRING, false);
  // nm.push_back("candle-out-gain"); def.Set(nm[i++], INT,    true,   60, 3600);
  // nm.push_back("candle");          def.Set(nm[i++], INT,    true,  300, 3600);
  // nm.push_back("t-max");           def.Set(nm[i++], INT,    true,  300,  900);
  // nm.push_back("gmi-min");         def.Set(nm[i++], DOUBLE, true, 0.00, 1.00);
  // nm.push_back("gmi-max");         def.Set(nm[i++], DOUBLE, true, 0.70, 2.20);
  // nm.push_back("margin2");         def.Set(nm[i++], DOUBLE, true, 0.05,  3.00);
  // nm.push_back("margin1");         def.Set(nm[i++], DOUBLE, true, 0.05,  3.00);
  // nm.push_back("sma-min-sl");      def.Set(nm[i++], DOUBLE, true, 1.50,  5.00);
  // nm.push_back("gmi-tframe");      def.Set(nm[i++], INT,    true,   60,  900);
  // nm.push_back("amp-factor");      def.Set(nm[i++], DOUBLE, true, 1.10,  7.00);
  // nm.push_back("cut");             def.Set(nm[i++], DOUBLE, true, 0.00,  1.00);
  // nm.push_back("min-factor");      def.Set(nm[i++], DOUBLE, true, 0.10,  3.00);
  // nm.push_back("sup-band-lim");    def.Set(nm[i++], DOUBLE, true, 0.10,  7.50);
  // nm.push_back("inf-band-lim");    def.Set(nm[i++], DOUBLE, true, 0.10,  7.00);
  // nm.push_back("perc-band-lim");   def.Set(nm[i++], DOUBLE, true, 0.10,  7.00);
  // nm.push_back("ma-A");            def.Set(nm[i++], INT,    true,  60, 1200);
  // nm.push_back("ma-B");            def.Set(nm[i++], INT,    true, 450, 1800);
  // nm.push_back("ma-C");            def.Set(nm[i++], INT,    true, 900, 5400);
  // nm.push_back("sma-p-A");         def.Set(nm[i++], DOUBLE, true, 0.05,   1.5);
  // nm.push_back("sma-p-B");         def.Set(nm[i++], DOUBLE, true, 1.50,  15.0);
  // nm.push_back("sma-p-C");         def.Set(nm[i++], DOUBLE, true, 0.30,   4.0);

  // Line ln; 
  // ln.SetSize(
  //   def.sizeofInt(), def.sizeofDouble(), def.sizeofString(), def.sizeofTime());

  // string line;
  // ifstream myfile ("../ref/dados20141212/linesstep4");

  // i = 0;
  // string s, p;
  // vector<string> c; c.resize(def.params.size());
  
  // map<std::string, Behavior>::const_iterator it;

  // if (myfile.is_open())
  // {
  //   // while ( getline (myfile,line) )
  //   // {
  //   //   cout << line << '\n';
  //   // }
  //   // myfile.close();
    
  //   while ( getline (myfile,line) )
  //   {    
  //     istringstream iss;
  //     iss.str(line);

  //     iss >> s; // dodd
      
  //     for(unsigned int j = 0; j < def.params.size(); j++)
  // 	iss >> c[ j ];
      
  //     for(unsigned int j = 0; j < def.params.size(); j++)
  //     {
  // 	unsigned first = c[j].find("--") + 2;
  // 	unsigned last  = c[j].find('=');
  // 	if( last != string::npos )
  // 	  p = c[j].substr( first, last-first);
  // 	else
  // 	  p = c[j].substr( first, c[j].length()-2);
	
  // 	if(def.Check(p)) cout << "Found " << p << endl;
  //     }
  //   }	
  //   myfile.close();
  // }
  // else cout << "Unable to open file"; 

  // string ln("1|Tot Result: 6912	2|Op Avg: 67.7647	3|Daily Avg: 181.895	4|Pos Daily Avg: 313.179	5|Pos Stdev: 245.382	6|Neg Daily Avg: -185.7	7|Neg Stdev: 135.797	8|pos ratio: 0.736842	9|Drawdown: 540	10|Total Ops: 102	11|file: hints/step4/WIN_2013_20141031-*ubuntu@dod-worker-019.cloudapp.net*quarksquarks_01shortstep4831-short");
  
  // string name("Tot Result"); 

  // size_t p1 = ln.find( name );
  // double d;

  // if(p1 != string::npos)
  // {
  //   size_t p2 = ln.find( ':', p1 );
  //   size_t p3 = ln.find( ' ', p2+2 );

  //   istringstream iss;
  //   iss.str( ln.substr(p2+1, p3) ); iss >> d;
  // }

  // cout << d << endl;

  // string profile("quarksquarks_01shortstep4");

  // p1 = ln.rfind( profile );
  // size_t n;

  // if(p1 != string::npos)
  // {
  //   size_t p2 = ln.rfind( '-' );
  //   istringstream iss;
  //   iss.str( ln.substr(p1+profile.length(), p2-1) ); iss >> n;
  // }

  // cout << n << endl;

  struct timespec ts;
  timespec_get( &ts, TIME_UTC);
  srandom(ts.tv_nsec ^ ts.tv_sec);

  cout << RAND_MAX << endl;
  cout << BIG_RAND_MAX << endl;
  cout << rand0to1() << endl;
  cout << rand0to1() << endl;
  cout << rand0to1() << endl;

  return 0;
}
