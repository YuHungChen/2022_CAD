

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <vector>
#include <string>
#include "Gate.h"
#include <fstream>
#include <map>
#include <string>
using namespace std ; 

    class CIRCUIT{
	private : 
	    vector<GATE* > Netlist  ; 
      vector<GATE* > PIlist  ;
      vector<GATE* > POlist  ; 
      vector<vector<GATE*> > all_path ; 
      vector<double > path_delay ;  
      //char POlist ; 
      vector<string> pattern_str_vec ; 
      //vector<string> pattern_vec ; 
         
      vector<string> NOR2_input_c  ;
      vector<string> NAND2_input_c ;
     // vector<string> one_vec ;   
         
      vector<vector<string> > NOR2_cell_rise ; 
      vector<vector<string> > NOR2_cell_fall ;
      vector<vector<string> > NOR2_rise_transition ;
      vector<vector<string> > NOR2_fall_transition  ;
      
      vector<vector<string> > INV_cell_rise ;
      vector<vector<string> > INV_cell_fall ;
      vector<vector<string> > INV_rise_transition ;
      vector<vector<string> > INV_fall_transition  ;
      
      vector<vector<string> > NAND2_cell_rise ; 
      vector<vector<string> > NAND2_cell_fall ; 
      vector<vector<string> > NAND2_rise_transition  ; 
      vector<vector<string> > NAND2_fall_transition  ;
      
      string INV_input_c ;

      
      
      vector<string> index_1 ;
      vector<string> index_2 ;
      
      map<string , GATE* > Gate_map ; 
      map<string , GATE* >::iterator Gate_it ; 
      
	public  :
	   // CIRCUIT() {} ;  
      CIRCUIT(string str ) : INV_input_c(str)  {
            Netlist.reserve(256);
            PIlist.reserve(128);
            //POlist.reserve(128);  
            //pattern_str_vec.reserve(64) ; 
            NOR2_cell_rise.reserve(8) ; 
            NOR2_cell_fall.reserve(8) ; 
            NOR2_rise_transition.reserve(8) ; 
            NOR2_fall_transition.reserve(8) ; 
            
            INV_cell_rise.reserve(8) ; 
            INV_cell_fall.reserve(8) ; 
            INV_rise_transition.reserve(8) ; 
            INV_fall_transition.reserve(8) ; 
            
            NAND2_cell_rise.reserve(8) ; 
            NAND2_cell_fall.reserve(8) ; 
            NAND2_rise_transition.reserve(8) ; 
            NAND2_fall_transition.reserve(8) ; 
            
           // INV_input_c("") ; 
            index_1.reserve(8) ; 
            index_2.reserve(8) ; 
            
            
        }   
      //string get_inv_input_c() {return INV_input_c ; } 
      void Construct_table( ifstream& test_lib )  ; 
      void set_gate     (ifstream& gate    ) ; 
      bool read_pattern (ifstream& pattern , bool first ) ;
      void write_delay_file(ofstream& delay_file) ; 
      void write_load_file (string load_file_name) ; 
      void write_path_file (ofstream& path_file) ; 
      
      void propogation ();
     // void DFS( vector<GATE*> & start_gate , GATE* end_gate , vector<GATE*> & one_row , double &delay ) ;
      //void Search_path() ; 
       
    } ; 








#endif
