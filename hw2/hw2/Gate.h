

#ifndef GATE_H
#define GATE_H

#include <string>
#include <vector>
using namespace std ;

    class GATE{
	private :
	    string Name ;
      string function_name ; 
      bool color      ; // to check the net is visited or not
      int logic_value ; 
	    int function_type ;   // 0 : inv  , 1 : NAND_2  , 2 : NOR_2 , 3 : PI  ; 
      double total_delay ; 
      double propogation_delay  ;
	    double transition_time ; 
      double output_C      ;
      //bool is_sensitizable ; 
      GATE* predecessor ; 
      vector<GATE*> Fan_in_list ;  
      vector<GATE*> Fan_out_list ; 
	    
	public :
      GATE( string G_name , int function , double capacitance ) : Name(G_name) , function_name("") , color(0),  function_type(function) , total_delay(0.0) ,propogation_delay(0.0) , transition_time(0.0) , output_C (capacitance)  {} ; 
      
      string get_name()            { return Name ; } 
      string get_function_name()   { return function_name ; } 
      int    get_function      () { return function_type  ; } 
      int    get_logic_value   () { return logic_value ; }
      double    get_total_delay  () { return total_delay   ; } 
      double    get_propogation_delay  () { return propogation_delay   ; } 
      double    get_transition_time () { return transition_time  ; } 
      double    get_output_c()       { return output_C       ; } 
      GATE*     get_Fan_out_list( int i )   { return Fan_out_list[i] ; }
      GATE*     get_Fan_in_list ( int i )   { return Fan_in_list[i]  ; }
      int       get_Fan_in_size () {return Fan_in_list.size() ;  }
      int       get_Fan_out_size () {return Fan_out_list.size() ;  }
      bool      get_color        () {return color; }
      //bool      get_sensitizable () {return is_sensitizable ; }
      GATE*     get_predecessor  () {return predecessor ; }
      
      void    set_function      (int function) { function_type =  function  ; } 
      void    set_total_delay  ( double delay ) { total_delay  = delay   ; } 
      void    set_propogation_delay  ( double delay ) { propogation_delay  = delay   ; } 
      void    set_transition_time ( double delay ) { transition_time = delay   ; } 
      void    set_output_c(double c )       {  output_C  = c     ; } 
      void    set_Fan_in_list ( GATE* G )  {Fan_in_list.push_back(G)  ; }
      void    set_Fan_out_list( GATE* G )  {Fan_out_list.push_back(G) ; }
      void    set_logic_value (int value)  { logic_value = value ; }
      void    set_function_name(string str) { function_name = str ; }
      void    set_color        (bool new_color) {color =  new_color ; }
     // void    set_sensitizable (bool is_sen)    {is_sensitizable = is_sen ; }
      void    set_predecessor  (GATE* pred ) { predecessor = pred ; }
    } ; 









#endif
