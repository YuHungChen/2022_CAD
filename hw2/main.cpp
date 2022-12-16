#include <iostream>
#include <fstream>
#include <sstream>
#include "Gate.h"
#include "Circuit.h"
#include <cstring>
using namespace std ;

int main( int argc , char* argv[] ) {

  ifstream c17_v , c17_pat ,test_lib ;
  ofstream out1 ;

  
  c17_v.open (argv[1] ) ;  //
  c17_pat.open (argv[3] ) ;
  test_lib.open (argv[5] ) ;


  string file_name = "" ;
  

  for (unsigned int i=0 ; i < strlen( argv[1] ) ; i++){
    if (i== strlen( argv[1] )-2)
      break ;
    else
      file_name = file_name + argv[1][i] ;
  }
  
  //cout << file_name << endl ;
  
  CIRCUIT Circuit("0.0" ) ;
  Circuit.Construct_table(test_lib) ;

  string str ;
  string unit ;
  string value ;

  


  string Title;
  string Title_name ;
  //char parenthesis ;
  bool flag = 0 ; 
  c17_v >> Title >> Title_name ;
  Circuit.set_gate(c17_v) ; 
  
  
  string output_path_name , output_load_name , output_delay_name ;

  output_path_name  = "310580023_" + file_name + "_path.txt" ;
  output_load_name  = "310580023_" + file_name + "_load.txt" ;
  output_delay_name = "310580023_" + file_name + "_delay.txt" ;
  
  ofstream delay_file ; 
  ofstream path_file  ; 
  delay_file.open(output_delay_name) ; 
  path_file.open(output_path_name) ; 
  //cout << output_path_name << endl ;
  //cout << output_load_name << endl ;
  //cout << output_delay_name << endl ;
  //cout <<  argv[1] << endl ;
  bool first_pattern = true ; 
  while (true){
  flag = Circuit.read_pattern(c17_pat , first_pattern ) ;
   
  //cout << 5<< endl ; 
  
  if (flag)
     break ; 

  Circuit.write_load_file(output_load_name) ; 
  Circuit.propogation() ; 
  Circuit.write_delay_file(delay_file) ; 
  Circuit.write_path_file(path_file) ; 
  
 // Circuit.Search_path() ; 
  
  first_pattern = false ; 
  
  //break ;  // temp ; 
  }


  delay_file.close() ; 
  path_file.close() ; 

  c17_v.close() ;
  c17_pat.close() ;
  test_lib.close() ;

}
