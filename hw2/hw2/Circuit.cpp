#include <iostream>
#include "Circuit.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <list>

#include <vector>
#include <string>
#include <map>
using namespace std ;

bool sort_c_value( GATE* G1 , GATE* G2 ){
    string str1 ; 
    string str2 ; 
    if (  G1 -> get_output_c()  == G2 -> get_output_c() ){
       str1 = G1 -> get_function_name() ; 
       str2 = G2 -> get_function_name() ; 
       if (str1.size() == str2.size()){
         for (unsigned int i=0 ; i<str1.size() ; i++){
             if (str1[i] == str2[i])
               continue ; 
             else 
               return ( int(str1[i]) < int(str2[i])    )  ;  
         }
       }else{
         return (str1.size() < str2.size()) ; 
       }
    }
    
    return ( G1 -> get_output_c()  > G2 -> get_output_c() ) ;
      
      
}

bool sort_delay_value( GATE* G1 , GATE* G2 ){
    string str1 ; 
    string str2 ; 
    if (  G1 -> get_propogation_delay()  == G2 -> get_propogation_delay() ){
       str1 = G1 -> get_function_name() ; 
       str2 = G2 -> get_function_name() ; 
       if (str1.size() == str2.size()){
         for (unsigned int i=0 ; i<str1.size() ; i++){
             if (str1[i] == str2[i])
               continue ; 
             else 
               return ( int(str1[i]) < int(str2[i])    )  ;  
         }
       }else{
         return (str1.size() < str2.size()) ; 
       }
    }
    
    return ( G1 -> get_propogation_delay()  > G2 -> get_propogation_delay() ) ;
      
      
}


void CIRCUIT::write_path_file (ofstream& path_file) {
  //cout << "start to search " << endl ; 


  double Longest_delay ; 
  int    Longest_index = 0 ; 
  double Shortest_delay ; 
  int    Shortest_index = 0 ; 
  
  for (unsigned int i=0 ; i < POlist.size() ; i++){
     if (i==0) {
        Longest_delay  = POlist[0] -> get_total_delay() ; 
        Shortest_delay = POlist[0] -> get_total_delay() ; 
        
        Longest_index  = 0 ; 
        Shortest_index = 0 ; 
     }else {
        if (  POlist[i] -> get_total_delay()  >  Longest_delay  ){
            Longest_delay = POlist[i] -> get_total_delay()  ; 
            Longest_index = i ; 
            
        }else if (POlist[i] -> get_total_delay()  <  Shortest_delay ) {
            Shortest_delay = POlist[i] -> get_total_delay()  ; 
            Shortest_index = i ; 
        }
     }
  }  
  
  // cout << Longest_delay << " " << Shortest_delay << endl ;
  
  vector <GATE* > output_path ; 
  GATE* gptr = POlist[Longest_index] ; 
  //cout << gptr -> get_name() << endl ; 
  while (gptr != NULL) {
    // cout << gptr -> get_name() << endl ; 
     output_path.push_back(gptr) ; 
     gptr = gptr -> get_predecessor() ; 
  }
  
  //cout << output_path.size() ;
  
  path_file << "Longest delay = " << Longest_delay << ", the path is: " ; 
  for (int i= output_path.size() -1 ; i >=0 ; i--){
    path_file << output_path[i] -> get_name() ;
    if (i!=0)
    path_file << " -> " ; 
  } 
  path_file << endl ;
  
  output_path.clear() ; 
  gptr = POlist[Shortest_index] ; 
  //cout << gptr -> get_name() << endl ; 
  while (gptr != NULL) {
    // cout << gptr -> get_name() << endl ; 
     output_path.push_back(gptr) ; 
     gptr = gptr -> get_predecessor() ; 
  }
  
  path_file << "Shortest delay = " << Shortest_delay << ", the path is: " ; 
  for (int i= output_path.size() -1 ; i >=0 ; i--){
    path_file << output_path[i] -> get_name() ;
    if (i!=0)
    path_file << " -> " ; 
  } 
  path_file << endl ;
  path_file << endl ;
  
  
  
  
  
  
}

void CIRCUIT::propogation(){
    //cout << "=========start_propogate============" << endl ;
    const int INV  = 0 ;
    const int NAND = 1 ;
    const int NOR  = 2 ;
    const int PI   = 3 ;  
    
    
    list<GATE* > Queue ; 
    // clear color 
    for (unsigned int i=0 ; i < Netlist.size() ; i++){
       //cout << Netlist[i] -> get_name() << " " << Netlist[i] -> get_function()<< endl ; 
       Netlist[i] -> set_predecessor(0) ; 
       if (  Netlist[i] ->  get_function() == PI ){     // PI set to color 1 
         Netlist[i] -> set_color(1) ; 
         if (Netlist[i] -> get_Fan_out_size() != 0){
          //   cout << Netlist[i] -> get_name() ; 
             for ( int j = 0 ; j<  Netlist[i]-> get_Fan_out_size() ; j++)
               Queue.push_back(  Netlist[i] -> get_Fan_out_list(j) )  ; 
         }
         
       }else
         Netlist[i] -> set_color(0) ; 
    }
    
    GATE* gptr = NULL ; 
    bool flag_to_calculate_delay ; 
    int logic_value ; 
    double times_row  = 1.0 ; 
    double times_col  = 1.0 ;
    int row_number = 0 ; 
    int col_number = 0 ;
    double value_inter_1 = 0.0 ; 
    double value_inter_2 = 0.0 ; 
    double value1 = 0.0 ;
    double value2 = 0.0 ; 
    
    bool has_c_value = false ; 
    bool extra_row_polation = false; 
    bool extra_col_polation = false; 
    
    double actual_transition_time = 0.0 ; 
    double last_level_delay = 0.0 ; 
    
    while (!Queue.empty() ) {
       flag_to_calculate_delay = true; 
       has_c_value = false ; 
       extra_row_polation = false ; 
       extra_col_polation = false ; 
       
       gptr = Queue.front() ; 
       times_row = 1.0 ;
       times_col = 1.0 ;
       if (gptr ->  get_Fan_in_size() != 0){
           for ( int i=0 ; i < gptr ->  get_Fan_in_size() ; i++ ){
              if (  gptr -> get_Fan_in_list(i) -> get_color() == 0){
                 flag_to_calculate_delay = 0 ; 
                 break ; 
              }
           }
       }
       
      // cout << "=============now_gptr========"<<endl ;
       //cout <<  gptr -> get_name()  << endl ;
       
       if (flag_to_calculate_delay && gptr -> get_color() == false ){
          for ( int j = 0 ; j<  gptr-> get_Fan_out_size() ; j++)
               Queue.push_back(  gptr -> get_Fan_out_list(j) )  ; 
               
         // cout <<  gptr -> get_name() << endl ;
          
          if ( gptr -> get_function() == INV ){
             actual_transition_time = gptr -> get_Fan_in_list(0)  -> get_transition_time()  ; 
             gptr -> set_predecessor (gptr -> get_Fan_in_list(0) ) ; 
             last_level_delay = gptr -> get_Fan_in_list(0) -> get_total_delay() ; 
             
          } else if ( gptr -> get_function() == NAND ){
             if ( gptr -> get_Fan_in_list(1) -> get_total_delay()   <  gptr -> get_Fan_in_list(0) -> get_total_delay()  ){
                 if ( gptr -> get_Fan_in_list(1)  -> get_logic_value() == 0) {    // controlling value
                      actual_transition_time = gptr -> get_Fan_in_list(1) -> get_transition_time() ; 
                      gptr -> set_predecessor (gptr -> get_Fan_in_list(1) ) ; 
                      last_level_delay = gptr -> get_Fan_in_list(1) -> get_total_delay() ; 
                 }else {
                      actual_transition_time = gptr -> get_Fan_in_list(0) -> get_transition_time() ; 
                      gptr -> set_predecessor (gptr -> get_Fan_in_list(0) ) ; 
                      last_level_delay = gptr -> get_Fan_in_list(0) -> get_total_delay() ;  
                 }
             }else if ( ( gptr -> get_Fan_in_list(1) -> get_total_delay()   ==  gptr -> get_Fan_in_list(0) -> get_total_delay()  )  && ( gptr -> get_Fan_in_list(1) -> get_transition_time() !=  gptr -> get_Fan_in_list(0) -> get_transition_time()) )   {
                 
                 if (  gptr -> get_Fan_in_list(0)  -> get_logic_value() == 0    &&  gptr -> get_Fan_in_list(1)  -> get_logic_value() == 0  ){   // controlling
                     if (  gptr -> get_Fan_in_list(0) -> get_transition_time()   <   gptr -> get_Fan_in_list(1) -> get_transition_time()  ){
                        actual_transition_time = gptr -> get_Fan_in_list(0) -> get_transition_time() ; 
                        gptr -> set_predecessor (gptr -> get_Fan_in_list(0) ) ; 
                        last_level_delay = gptr -> get_Fan_in_list(0) -> get_total_delay() ; 
                     }else {
                        actual_transition_time = gptr -> get_Fan_in_list(1) -> get_transition_time() ; 
                        gptr -> set_predecessor (gptr -> get_Fan_in_list(1) ) ; 
                        last_level_delay = gptr -> get_Fan_in_list(1) -> get_total_delay() ; 
                     }
                     
                 } else if  (  gptr -> get_Fan_in_list(0)  -> get_logic_value() == 1    &&  gptr -> get_Fan_in_list(1)  -> get_logic_value() == 1  ) {
                     if (  gptr -> get_Fan_in_list(0) -> get_transition_time()   >   gptr -> get_Fan_in_list(1) -> get_transition_time()  ){
                        actual_transition_time = gptr -> get_Fan_in_list(0) -> get_transition_time() ; 
                        gptr -> set_predecessor (gptr -> get_Fan_in_list(0) ) ; 
                        last_level_delay = gptr -> get_Fan_in_list(0) -> get_total_delay() ; 
                     }else {
                        actual_transition_time = gptr -> get_Fan_in_list(1) -> get_transition_time() ; 
                        gptr -> set_predecessor (gptr -> get_Fan_in_list(1) ) ; 
                        last_level_delay = gptr -> get_Fan_in_list(1) -> get_total_delay() ; 
                     }
                 } else if  ( gptr -> get_Fan_in_list(0)  -> get_logic_value() == 0  ){
                    actual_transition_time = gptr -> get_Fan_in_list(0) -> get_transition_time() ; 
                    gptr -> set_predecessor (gptr -> get_Fan_in_list(0) ) ; 
                    last_level_delay = gptr -> get_Fan_in_list(0) -> get_total_delay() ; 
                 } else {
                    actual_transition_time = gptr -> get_Fan_in_list(1) -> get_transition_time() ; 
                    gptr -> set_predecessor (gptr -> get_Fan_in_list(1) ) ; 
                    last_level_delay = gptr -> get_Fan_in_list(1) -> get_total_delay() ; 
                 }
                
             }else{
                 if ( gptr -> get_Fan_in_list(0)  -> get_logic_value() == 0) {    // controlling value
                     
                      actual_transition_time = gptr -> get_Fan_in_list(0) -> get_transition_time() ; 
                      gptr -> set_predecessor (gptr -> get_Fan_in_list(0) ) ; 
                      last_level_delay = gptr -> get_Fan_in_list(0) -> get_total_delay() ; 
                     
                 }else {
                      actual_transition_time = gptr -> get_Fan_in_list(1) -> get_transition_time() ; 
                      gptr -> set_predecessor (gptr -> get_Fan_in_list(1) ) ; 
                      last_level_delay = gptr -> get_Fan_in_list(1) -> get_total_delay() ;  
                 }
             }
            
          } else if ( gptr -> get_function() == NOR ){
             if ( gptr -> get_Fan_in_list(1) -> get_total_delay()   <  gptr -> get_Fan_in_list(0) -> get_total_delay()  ){
                 if ( gptr -> get_Fan_in_list(1)  -> get_logic_value() == 1) {    // controlling value
                      actual_transition_time = gptr -> get_Fan_in_list(1) -> get_transition_time() ; 
                      gptr -> set_predecessor (gptr -> get_Fan_in_list(1) ) ; 
                      last_level_delay = gptr -> get_Fan_in_list(1) -> get_total_delay() ; 
                 }else {
                      actual_transition_time = gptr -> get_Fan_in_list(0) -> get_transition_time() ; 
                      gptr -> set_predecessor (gptr -> get_Fan_in_list(0) ) ; 
                      last_level_delay = gptr -> get_Fan_in_list(0) -> get_total_delay() ;  
                 }
             }else if ( ( gptr -> get_Fan_in_list(1) -> get_total_delay()   ==  gptr -> get_Fan_in_list(0) -> get_total_delay()  )  && ( gptr -> get_Fan_in_list(1) -> get_transition_time() !=  gptr -> get_Fan_in_list(0) -> get_transition_time()) )   {
                 
                 if (  gptr -> get_Fan_in_list(0)  -> get_logic_value() == 1    &&  gptr -> get_Fan_in_list(1)  -> get_logic_value() == 1  ){   // controlling
                     if (  gptr -> get_Fan_in_list(0) -> get_transition_time()   <   gptr -> get_Fan_in_list(1) -> get_transition_time()  ){
                        actual_transition_time = gptr -> get_Fan_in_list(0) -> get_transition_time() ; 
                        gptr -> set_predecessor (gptr -> get_Fan_in_list(0) ) ; 
                        last_level_delay = gptr -> get_Fan_in_list(0) -> get_total_delay() ; 
                     }else {
                        actual_transition_time = gptr -> get_Fan_in_list(1) -> get_transition_time() ; 
                        gptr -> set_predecessor (gptr -> get_Fan_in_list(1) ) ; 
                        last_level_delay = gptr -> get_Fan_in_list(1) -> get_total_delay() ; 
                     }
                     
                 } else if  (  gptr -> get_Fan_in_list(0)  -> get_logic_value() == 0    &&  gptr -> get_Fan_in_list(1)  -> get_logic_value() == 0  ) {
                     if (  gptr -> get_Fan_in_list(0) -> get_transition_time()   >   gptr -> get_Fan_in_list(1) -> get_transition_time()  ){
                        actual_transition_time = gptr -> get_Fan_in_list(0) -> get_transition_time() ; 
                        gptr -> set_predecessor (gptr -> get_Fan_in_list(0) ) ; 
                        last_level_delay = gptr -> get_Fan_in_list(0) -> get_total_delay() ; 
                     }else {
                        actual_transition_time = gptr -> get_Fan_in_list(1) -> get_transition_time() ; 
                        gptr -> set_predecessor (gptr -> get_Fan_in_list(1) ) ; 
                        last_level_delay = gptr -> get_Fan_in_list(1) -> get_total_delay() ; 
                     }
                 } else if  ( gptr -> get_Fan_in_list(0)  -> get_logic_value() == 1  ){
                    actual_transition_time = gptr -> get_Fan_in_list(0) -> get_transition_time() ; 
                    gptr -> set_predecessor (gptr -> get_Fan_in_list(0) ) ; 
                    last_level_delay = gptr -> get_Fan_in_list(0) -> get_total_delay() ; 
                 } else {
                    actual_transition_time = gptr -> get_Fan_in_list(1) -> get_transition_time() ; 
                    gptr -> set_predecessor (gptr -> get_Fan_in_list(1) ) ; 
                    last_level_delay = gptr -> get_Fan_in_list(1) -> get_total_delay() ; 
                 }
                
             }else {
                 if ( gptr -> get_Fan_in_list(0)  -> get_logic_value() == 1) {    // controlling value
                      actual_transition_time = gptr -> get_Fan_in_list(0) -> get_transition_time() ; 
                      gptr -> set_predecessor (gptr -> get_Fan_in_list(0) ) ; 
                      last_level_delay = gptr -> get_Fan_in_list(0) -> get_total_delay() ; 
                 }else {
                      actual_transition_time = gptr -> get_Fan_in_list(1) -> get_transition_time() ; 
                      gptr -> set_predecessor (gptr -> get_Fan_in_list(1) ) ; 
                      last_level_delay = gptr -> get_Fan_in_list(1) -> get_total_delay() ;  
                 }
             }
             
          }
         // cout << actual_transition_time << endl ;
          // polation // 
               for (unsigned int i=0 ; i < index_2.size() ; i++){
                  if (  actual_transition_time  < stod( index_2[i])   ){
                  //  cout <<  gptr -> get_transition_time() << " " << stod( index_2[i]) << endl ; 
                    if (i==0)
                      row_number = 1 ; 
                    else
                      row_number = i ; 
                    break ; 
                  }
                  if (i == index_2.size() - 1 ){
                    row_number = i ;   // need to extrapolation
                    extra_row_polation = true ; 
                   // cout << "extro_polation" << endl ; 
                  }
               }
          if (extra_row_polation)   
            times_row =   ( actual_transition_time - stod( index_2[row_number     ])  )    / ( stod( index_2[row_number]) - stod( index_2[row_number -1]) ) ; 
          else   
            times_row =   ( stod( index_2[row_number])  -  actual_transition_time  )    / ( stod( index_2[row_number]) - stod( index_2[row_number -1]) ) ; 
               
               for (unsigned int i=0 ; i < index_1.size() ; i++){
                  if (  gptr -> get_output_c()  < stod( index_1[i])   ){
                  //  cout <<  gptr -> get_transition_time() << " " << stod( index_2[i]) << endl ; 
                    if (i==0)
                      col_number = 0 ; 
                    else
                      col_number = i ; 
                    break ; 
                  }
                  if (i == index_1.size() - 1 ){
                    col_number = i ;   // need to extrapolation
                    extra_col_polation = true ; 
                 //   cout << "extro_polation" << endl ;
                  }
                  
               }
          if (extra_col_polation)
            times_col =   ( gptr -> get_output_c()      -  stod( index_1[col_number ])   )    / ( stod( index_1[col_number]) - stod( index_1[col_number -1]) ) ; 
          else
            times_col =   ( stod( index_1[col_number])  -  gptr -> get_output_c()       )    / ( stod( index_1[col_number]) - stod( index_1[col_number -1]) ) ; 
               
          // polation // 
          /*
          cout << "================Name  : " << gptr -> get_name()  << "=============" << endl ;  
          cout << "input transition time : " << actual_transition_time << endl ; 
          cout << "output c              : " << gptr -> get_output_c()  << endl ; 
          cout << "row_num  : " << row_number << endl  ; 
          cout << "times_row  : " << times_row << endl  ; 
          
          cout << "col_num  : " << col_number << endl  ; 
          cout << "times_col  : " << times_col << endl  ; 
          */
        //  cout << "****** \n" ; 
          
          if (gptr -> get_function() == INV ){
          //   cout << "INV : " << endl ; 
             logic_value = ! ( gptr -> get_Fan_in_list(0) -> get_logic_value() ) ;
             gptr -> set_logic_value(logic_value) ; 
             gptr -> set_color (1) ; 
             
             
             if (logic_value){
                // cout << "rise" << endl ; 
                 // cell rise delay 
                 value1 = stod ( INV_cell_rise[row_number  ][col_number  ] ); 
                 value2 = stod ( INV_cell_rise[row_number  ][col_number-1] ); 
             //    cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;
             //    cout << "inter cell_delay : " << value_inter_1 << endl ; 
                 
              //   cout << "===================" << endl ; 
                 
                 value1 = stod ( INV_cell_rise[row_number-1][col_number  ] ); 
                 value2 = stod ( INV_cell_rise[row_number-1][col_number-1] ); 
              //   cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;
                   
              //   cout << "inter cell_delay : " << value_inter_2 << endl ; 
                 
                // cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
                 
              //   cout << "final cell_delay : " << value_inter_1 << endl ; 
                 gptr -> set_propogation_delay (  value_inter_1  ) ;
                 gptr -> set_total_delay       (  value_inter_1 + last_level_delay ) ; 
                 
                 
                 // transition rise time 
                 value1 = stod ( INV_rise_transition[row_number  ][col_number  ] ); 
                 value2 = stod ( INV_rise_transition[row_number  ][col_number-1] ); 
                // cout << "original transition : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;  
                // cout << "inter transition : " << value_inter_1 << endl ; 
                 
               //  cout << "===================" << endl ; 
                 
                 value1 = stod ( INV_rise_transition[row_number-1][col_number  ] ); 
                 value2 = stod ( INV_rise_transition[row_number-1][col_number-1] ); 
               //  cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;  
                // cout << "inter transition : " << value_inter_2 << endl ; 
                 
              //   cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ;   
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
              //   cout << "final transition : " << value_inter_1 << endl ; 
                 gptr -> set_transition_time (  value_inter_1  ) ;
                 
             }else{
             //    cout << "fall" << endl ; 
                 // cell fall delay 
                 value1 = stod ( INV_cell_fall[row_number  ][col_number  ] ); 
                 value2 = stod ( INV_cell_fall[row_number  ][col_number-1] ); 
              //   cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;  
              //   cout << "inter cell_delay : " << value_inter_1 << endl ; 
                 
              //   cout << "===================" << endl ; 
                 
                 value1 = stod ( INV_cell_fall[row_number-1][col_number  ] ); 
                 value2 = stod ( INV_cell_fall[row_number-1][col_number-1] ); 
             //    cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;  
             //    cout << "inter cell_delay : " << value_inter_2 << endl ; 
                 
              //   cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ;   
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
             //    cout << "final cell_delay : " << value_inter_1 << endl ; 
                 gptr -> set_propogation_delay (  value_inter_1  ) ;
                 gptr -> set_total_delay       (  value_inter_1 + last_level_delay ) ; 
                 
                 
                 // transition fall time 
                 value1 = stod ( INV_fall_transition[row_number  ][col_number  ] ); 
                 value2 = stod ( INV_fall_transition[row_number  ][col_number-1] ); 
          //       cout << "original transition : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;  
            //     cout << "inter transition : " << value_inter_1 << endl ; 
                 
            //     cout << "===================" << endl ; 
                 
                 value1 = stod ( INV_fall_transition[row_number-1][col_number  ] ); 
                 value2 = stod ( INV_fall_transition[row_number-1][col_number-1] ); 
            //     cout << "original transition : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;  
            //     cout << "inter transition : " << value_inter_2 << endl ; 
                 
            //     cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ;   
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
            //     cout << "final transition : " << value_inter_1 << endl ; 
                 gptr -> set_transition_time (  value_inter_1  ) ;
                 
             }
             
          } else if (gptr -> get_function() == NAND ){
           //  cout << "NAND : " << endl ; 
             gptr -> set_color (1) ; 
             
             for ( int i=0 ; i< gptr -> get_Fan_in_size() ; i++){
                if (  gptr -> get_Fan_in_list(i) -> get_logic_value() == 0 ){
                   has_c_value = true ; 
                   break ; 
                }
             } 
            
             if (has_c_value){
                gptr -> set_logic_value(1) ; 
                
           //     cout << "rise" << endl ; 
                 // cell rise delay 
                 value1 = stod ( NAND2_cell_rise[row_number  ][col_number  ] ); 
                 value2 = stod ( NAND2_cell_rise[row_number  ][col_number-1] ); 
           //      cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;
           //      cout << "inter cell_delay : " << value_inter_1 << endl ; 
                 
           //      cout << "===================" << endl ; 
                 
                 value1 = stod ( NAND2_cell_rise[row_number-1][col_number  ] ); 
                 value2 = stod ( NAND2_cell_rise[row_number-1][col_number-1] ); 
            //     cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;
                   
             //    cout << "inter cell_delay : " << value_inter_2 << endl ; 
                 
            //     cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
                 
            //     cout << "final cell_delay : " << value_inter_1 << endl ; 
                 gptr -> set_propogation_delay (  value_inter_1  ) ;
                 gptr -> set_total_delay       (  value_inter_1 + last_level_delay  ) ; 
                 
                 
                 // transition rise time 
                 value1 = stod ( NAND2_rise_transition[row_number  ][col_number  ] ); 
                 value2 = stod ( NAND2_rise_transition[row_number  ][col_number-1] ); 
            //     cout << "original transition : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;  
           //      cout << "inter transition : " << value_inter_1 << endl ; 
                 
           //      cout << "===================" << endl ; 
                 
                 value1 = stod ( NAND2_rise_transition[row_number-1][col_number  ] ); 
                 value2 = stod ( NAND2_rise_transition[row_number-1][col_number-1] ); 
           //      cout << "original transition : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;  
            //     cout << "inter transition : " << value_inter_2 << endl ; 
                 
            //     cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ;   
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
              //   cout << "final transition : " << value_inter_1 << endl ; 
                 gptr -> set_transition_time (  value_inter_1  ) ;
                 
             }else {
                 gptr -> set_logic_value(0) ; 
                
            //     cout << "fall" << endl ; 
                 // cell fall delay 
                 value1 = stod ( NAND2_cell_fall[row_number  ][col_number  ] ); 
                 value2 = stod ( NAND2_cell_fall[row_number  ][col_number-1] ); 
           //      cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;  
              //   cout << "inter cell_delay : " << value_inter_1 << endl ; 
                 
             //    cout << "===================" << endl ; 
                 
                 value1 = stod ( NAND2_cell_fall[row_number-1][col_number  ] ); 
                 value2 = stod ( NAND2_cell_fall[row_number-1][col_number-1] ); 
            //     cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;  
             //    cout << "inter cell_delay : " << value_inter_2 << endl ; 
                 
              //   cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ;   
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
              //   cout << "final cell_delay : " << value_inter_1 << endl ; 
                 gptr -> set_propogation_delay (  value_inter_1  ) ;
                 gptr -> set_total_delay       (  value_inter_1 + last_level_delay ) ; 
                 
                 
                 // transition fall time 
                 value1 = stod ( NAND2_fall_transition[row_number  ][col_number  ] ); 
                 value2 = stod ( NAND2_fall_transition[row_number  ][col_number-1] ); 
            //     cout << "original transition : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;  
            //     cout << "inter transition : " << value_inter_1 << endl ; 
                 
            //     cout << "===================" << endl ; 
                 
                 value1 = stod ( NAND2_fall_transition[row_number-1][col_number  ] ); 
                 value2 = stod ( NAND2_fall_transition[row_number-1][col_number-1] ); 
           //      cout << "original transition : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;  
            //     cout << "inter transition : " << value_inter_2 << endl ; 
                 
             //    cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ;   
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
             //    cout << "final transition : " << value_inter_1 << endl ; 
                 gptr -> set_transition_time (  value_inter_1  ) ;
                
                
                
             }
          
          
          } else if (gptr -> get_function() == NOR ){
          //   cout << "NOR : " << endl ; 
             gptr -> set_color (1) ; 
             
             for ( int i=0 ; i< gptr -> get_Fan_in_size() ; i++){
                if (  gptr -> get_Fan_in_list(i) -> get_logic_value() == 1 ){
                   has_c_value = true ; 
                   break ; 
                }
             } 
             
             if (!has_c_value){
                gptr -> set_logic_value(1) ; 
                
                
            //    cout << "rise" << endl ; 
                 // cell rise delay 
                 value1 = stod ( NOR2_cell_rise[row_number  ][col_number  ] ); 
                 value2 = stod ( NOR2_cell_rise[row_number  ][col_number-1] ); 
            //     cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;
            //     cout << "inter cell_delay : " << value_inter_1 << endl ; 
                 
            //     cout << "===================" << endl ; 
                 
                 value1 = stod ( NOR2_cell_rise[row_number-1][col_number  ] ); 
                 value2 = stod ( NOR2_cell_rise[row_number-1][col_number-1] ); 
             //    cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;
                   
             //    cout << "inter cell_delay : " << value_inter_2 << endl ; 
                 
             //    cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
                 
             //    cout << "final cell_delay : " << value_inter_1 << endl ; 
                 gptr -> set_propogation_delay (  value_inter_1  ) ;
                 gptr -> set_total_delay       (  value_inter_1 + last_level_delay  ) ; 
                 
                 
                 // transition rise time 
                 value1 = stod ( NOR2_rise_transition[row_number  ][col_number  ] ); 
                 value2 = stod ( NOR2_rise_transition[row_number  ][col_number-1] ); 
              //   cout << "original transition : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;  
             //    cout << "inter transition : " << value_inter_1 << endl ; 
                 
            //     cout << "===================" << endl ; 
                 
                 value1 = stod ( NOR2_rise_transition[row_number-1][col_number  ] ); 
                 value2 = stod ( NOR2_rise_transition[row_number-1][col_number-1] ); 
            //     cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;  
             //    cout << "inter transition : " << value_inter_2 << endl ; 
                 
             //    cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ;   
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
             //    cout << "final transition : " << value_inter_1 << endl ; 
                 gptr -> set_transition_time (  value_inter_1  ) ;
                
             }else {
                gptr -> set_logic_value(0) ; 
                
                
            //    cout << "fall" << endl ; 
                 // cell fall delay 
                 value1 = stod ( NOR2_cell_fall[row_number  ][col_number  ] ); 
                 value2 = stod ( NOR2_cell_fall[row_number  ][col_number-1] ); 
             //    cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;  
              //   cout << "inter cell_delay : " << value_inter_1 << endl ; 
                 
               //  cout << "===================" << endl ; 
                 
                 value1 = stod ( NOR2_cell_fall[row_number-1][col_number  ] ); 
                 value2 = stod ( NOR2_cell_fall[row_number-1][col_number-1] ); 
               //  cout << "original cell_delay : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;  
              //  cout << "inter cell_delay : " << value_inter_2 << endl ; 
                 
              //   cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ;   
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
             //    cout << "final cell_delay : " << value_inter_1 << endl ; 
                 gptr -> set_propogation_delay (  value_inter_1  ) ;
                 gptr -> set_total_delay       (  value_inter_1 + last_level_delay  ) ; 
                 
                 
                 // transition fall time 
                 value1 = stod ( NOR2_fall_transition[row_number  ][col_number  ] ); 
                 value2 = stod ( NOR2_fall_transition[row_number  ][col_number-1] ); 
             //    cout << "original transition : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_1  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_1  = value1 - ( (value1 - value2) * times_col ) ;  
               //  cout << "inter transition : " << value_inter_1 << endl ; 
                 
             //    cout << "===================" << endl ; 
                 
                 value1 = stod ( NOR2_fall_transition[row_number-1][col_number  ] ); 
                 value2 = stod ( NOR2_fall_transition[row_number-1][col_number-1] ); 
              //   cout << "original transition : " << value1 << " " << value2 << endl ; 
                 if (extra_col_polation)
                   value_inter_2  = value1 + ( (value1 - value2) * times_col ) ;  
                 else
                   value_inter_2  = value1 - ( (value1 - value2) * times_col ) ;  
              //   cout << "inter transition : " << value_inter_2 << endl ; 
                 
              //   cout << "===================" << endl ; 
                 
                 if (extra_row_polation)
                   value_inter_1  = value_inter_1 + ( (value_inter_1 - value_inter_2) * times_row ) ;   
                 else
                   value_inter_1  = value_inter_1 - ( (value_inter_1 - value_inter_2) * times_row ) ; 
                 
              //   cout << "final transition : " << value_inter_1 << endl ; 
                 gptr -> set_transition_time (  value_inter_1  ) ;
                
                
                
                
             }
          
          
          } 
          
       }
      
       Queue.pop_front() ; 
    }
    /*
    cout << "===============================" << endl ; 
    
    for (vector<GATE*>::iterator it = Netlist.begin() ; it != Netlist.end() ; it++ ){
       if (  (*it) -> get_function() == PI)
         continue ; 
       else {
          cout  << setw(10) <<  (*it) -> get_name() << setw(20) << (*it) -> get_propogation_delay() << setw(20) << (*it) -> get_transition_time()  << setw(20) << (*it) -> get_output_c() <<  endl ;
       }
    }
    
    */
    
    
    
    /*
    for (vector<GATE*>::iterator it = Netlist.begin() ; it != Netlist.end() ; it++ ){
      cout << "string : "  << setw(5) << (*it) -> get_name() << " A1 :  " << setw(10)  ;  
      //cout << (*it) -> get_Fan_in_size() << endl ;
      if (  (*it) -> get_Fan_in_size() == 0){
         cout << " NULL" <<  " A2 : " << "NULL"  ;
      }else if (   (*it) -> get_Fan_in_size() == 1 ) // inv
         cout <<  (*it) -> get_Fan_in_list(0) -> get_name() << " A2 : " << "NULL " ;
      else
         cout <<  (*it) -> get_Fan_in_list(0) -> get_name() << " A2 : " << (*it) -> get_Fan_in_list(1) -> get_name() ;
         
      
      if (  (*it) -> get_Fan_out_size() == 0 ) {
        cout << setw(10) <<  " ZN : "<< "NULL "<< endl ; 
      } else {
        cout << setw(10) <<  " ZN : " ; 
        for (unsigned int i = 0 ; i<  (*it) -> get_Fan_out_size() ; i++)
          cout << (*it)  -> get_Fan_out_list(i) -> get_name() << " " ; 
        cout << endl ;
      }
        
    } */
    
    //cout << 33 << endl ;
    
}

bool CIRCUIT::read_pattern( ifstream& pattern , bool first){
  //cout << "Read_pattern" << endl ;
  string str ;
  string new_str  ; 
  //vector <string> pattern_str_vec ; 
  stringstream ss ; 
  //int value ; 
  //cout << PIlist.size() << endl ;
  //getline(pattern, str ) ;
  
 // pattern_str_vec.reserve(PIlist.size()) ; 
  
  bool no_pattern = 0 ; 
/*  
  char old_char = ' '; 
  char find_char ; 
  bool slash_astrick_flag = 0 ; 
  bool slash_slash_flag   = 0 ; 
  bool find_input = 0 ; 
  
  while (!find_input){
      getline(pattern,str) ; 
      ss << str ; 
      
      while( ss >> find_char ){
          if (old_char == '\\'  && find_char == '\\'){
            slash_slash_flag = 1 ; 
            break ; 
          }else if (old_char == '\\'  && find_char == '*') {
            slash_astrick_flag = 1 ;
          } else if (old_char == '*'  && find_char == '\\'){
            slash_astrick_flag = 0 ;
          }
          
          old_char = find_char ; 
      }
        
      ss.str("") ;
      ss.clear() ;
      
      if (slash_astrick_flag  || slash_slash_flag){
          slash_slash_flag = 0 ; 
          continue ; 
        }
      ss << str ; 
      
      while (ss >> str ){
       cout << str << endl ;
        if (str == "input") {
          find_input = 1 ;
          break ; 
        }
        
        if (str == ".end"){
          find_input = 1 ; 
          no_pattern = 1 ; 
          break ; 
        }
      }
      
  
      
  } 
  
  */
  //cout << 11<< endl ;
  //ss.str("") ;
  //ss.clear() ;
  
  
  if (first){
        while (pattern >> str){
          if (str == "input"  || (str == "*/input") || (str == "/**/input" ) )
             break ;
        } 
    
      getline(pattern, str ) ;
      ss << str ;
      //cout << str << endl ;
      
      while ( getline(ss , str , ',') ) {
         new_str = "" ; 
         for (unsigned int i=0 ; i< str.size() ; i=i+1){
           if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' ' || str[i] == '\n' || str[i] == '\t' )
             continue ;
           else
             new_str = new_str + str[i] ; 
         }
        // cout << new_str << endl ;
         
         pattern_str_vec.push_back(new_str) ; 
         
         if (  pattern_str_vec.size() == PIlist.size()){
           break ;
         }  
      }
      ss.str("") ;
      ss.clear() ;
      
 //      for (unsigned int i=0 ; i< pattern_str_vec.size() ; i=i+1){
 //         cout << pattern_str_vec[i] << endl ;
  //     }
   
  }
  //cout << "second time" << endl ;
   bool slash_astrick_flag = 0 ;
   unsigned int a = 0 ;
   GATE* gptr = NULL  ; 
  // cout << PIlist.size()<< endl ;
   while ( pattern >> str){
     new_str = "" ; 
       for (unsigned int i=0 ; i< str.size() ; i=i+1){
          if( i>=1 && str[i]== '/'  && str[i-1] == '/' ){
            getline(pattern,str) ; 
            break ; 
          }else if(i>=1 && str[i-1]== '/'  && str[i] == '*' ){
            slash_astrick_flag = 1 ;
            i = i+1 ; 
          }else if (slash_astrick_flag) {
            if (i>=1 && str[i-1] == '*' && str[i] == '/'){
               slash_astrick_flag = 0 ;
            }
          }else {
            new_str = new_str + str[i] ;
          }
       }
       
    if (slash_astrick_flag == 1){
      continue ; 
    }
//    cout << new_str << endl ;
     //cout << str << endl ;
     if (new_str == ".end"){
        no_pattern = 1 ;
        break ;
     }else if (new_str == "1"){
        gptr = Gate_map[pattern_str_vec[a]] ; 
        gptr -> set_logic_value (stoi(new_str)) ; 
    //    cout << gptr -> get_name() << endl ; 
        a++ ; 
     } else if (new_str == "0"){
        gptr = Gate_map[pattern_str_vec[a]] ; 
        gptr -> set_logic_value (stoi(new_str)) ; 
        //cout << gptr -> get_name() << endl ;
        a++ ; 
     }
     if (a == PIlist.size() )
        break ;
        
  }
   
  return no_pattern ; 
}

void CIRCUIT::Construct_table( ifstream& test_lib){

  //cout << 11 << endl ;

  vector<string> one_row ;
  string value , str ;
  stringstream each_line ; 
  int a =0 ;

  while (test_lib >> str){
     if (str == "index_1"){
       getline(test_lib,str , '\"') ;
       getline(test_lib,str , '\"') ;
       each_line.str("") ;
       each_line.clear() ;
       each_line << str  ;
       while ( getline(each_line,value,',') ){
            index_1.push_back (value) ;
       }

       break ;
     }
  }
  
  
  //cout << index_1.size() ;
  //for (unsigned int i=0 ; i< index_1.size() ; i++){
 //   cout << index_1[i] << endl ;
  //}
 //  cout <<  "======" << endl ;

  while (test_lib >> str){
     if (str == "index_2"){
       getline(test_lib,str , '\"') ;
       getline(test_lib,str , '\"') ;
       each_line.str("") ;
       each_line.clear() ;
       each_line << str  ;
       while ( getline(each_line,value,',') ){
            index_2.push_back (value) ;
       }

       break ;
     }
  }
  //cout << index_1.size() ;
 // for (unsigned int i=0 ; i< index_2.size() ; i++){
//    cout << index_2[i] << endl ;
 // }
//======================NOR=====================================
  while (test_lib >> str){
    if(str == "capacitance"){
        test_lib >> str ;
        getline(test_lib , str ,' ') ;
        getline(test_lib , str ,';') ;
        NOR2_input_c.push_back(str) ;
        break ;
    }
  }

  while (test_lib >> str){
    if(str == "capacitance"){
        test_lib >> str ;
        getline(test_lib , str ,' ') ;
        getline(test_lib , str ,';') ;
        NOR2_input_c.push_back(str) ;
        break ;
    }
  }
 // cout <<  "======" << endl ;
 // for (unsigned int i=0 ; i< NOR2_input_c.size() ; i++){
  //  cout << NOR2_input_c[i] << endl ;
  //}

 // cout <<  "======" << endl ;

  while (test_lib >> str){
        if (str == "timing()"){
            break ;
        }
    }


  while (test_lib){
    while (str != "values"){
        test_lib >> str ;
    }
    a = a + 1 ;

    for (int i=1 ; i<=7 ; i=i+1){
        if ( getline(test_lib,str , '\"') ){
            getline(test_lib,str , '\"') ;
            each_line.str("") ;
            each_line.clear() ;
            each_line << str  ;

            while ( getline(each_line,value,',') ){
                one_row.push_back (value) ;
            }
            //getline(test_lib,str , '\"') ;
            //one_row.push_back (str) ;
            if (a == 1)
                NOR2_cell_rise.push_back(one_row) ;
            else if (a==2)
                NOR2_cell_fall.push_back(one_row) ;
            else if (a==3)
                NOR2_rise_transition.push_back(one_row) ;
            else
                NOR2_fall_transition.push_back(one_row) ;

               one_row.clear() ;
            }
    }
    if (a == 4)
        break ;
  }
/*
  for (unsigned int i=0 ; i< NOR2_fall_transition.size() ; i++){
    for (unsigned int j=0 ; j< NOR2_fall_transition[i].size() ; j++){
        cout << NOR2_fall_transition[i][j] << " " ;
    }
    cout << endl  ;
  }
  */
//======================INV========================================
  a = 0 ;
  while (test_lib >> str){
    if(str == "capacitance"){
        test_lib >> str ;
        getline(test_lib , str ,' ') ;
        getline(test_lib , str ,';') ;
        INV_input_c = str ;
        break ;
    }
  }

 // cout << INV_input_c << endl ;

  while (test_lib >> str){
        if (str == "timing()"){
            break ;
        }
    }


  while (test_lib){
    while (str != "values"){
        test_lib >> str ;
    }
    a = a + 1 ;

    for (int i=1 ; i<=7 ; i=i+1){
        if ( getline(test_lib,str , '\"') ){
            getline(test_lib,str , '\"') ;
            each_line.str("") ;
            each_line.clear() ;
            each_line << str  ;
            while ( getline(each_line,value,',') ){
                one_row.push_back (value) ;
            }
            //getline(test_lib,str , '\"') ;
            //one_row.push_back (str) ;
            if (a == 1)
                INV_cell_rise.push_back(one_row) ;
            else if (a==2)
                INV_cell_fall.push_back(one_row) ;
            else if (a==3)
                INV_rise_transition.push_back(one_row) ;
            else
                INV_fall_transition.push_back(one_row) ;

               one_row.clear() ;
            }
    }
    if (a == 4)
        break ;
  }
/*
  for (unsigned int i=0 ; i< INV_fall_transition.size() ; i++){
    for (unsigned int j=0 ; j< INV_fall_transition[i].size() ; j++){
        cout << INV_fall_transition[i][j] << " " ;
    }
    cout << endl  ;
  }
    */

//=====================NAND2==================================
a = 0 ;
 while (test_lib >> str){
    if(str == "capacitance"){
        test_lib >> str ;
        getline(test_lib , str ,' ') ;
        getline(test_lib , str ,';') ;
        NAND2_input_c.push_back(str) ;
        break ;
    }
  }

  while (test_lib >> str){
    if(str == "capacitance"){
        test_lib >> str ;
        getline(test_lib , str ,' ') ;
        getline(test_lib , str ,';') ;
        NAND2_input_c.push_back(str) ;
        break ;
    }
  }
  //cout <<  "======" << endl ;
 // for (unsigned int i=0 ; i< NAND2_input_c.size() ; i++){
 //   cout << NAND2_input_c[i] << endl ;
  //}

 // cout <<  "======" << endl ;

  while (test_lib >> str){
        if (str == "timing()"){
            break ;
        }
    }


  while (test_lib){
    while (str != "values"){
        test_lib >> str ;
    }
    a = a + 1 ;

    for (int i=1 ; i<=7 ; i=i+1){
        if ( getline(test_lib,str , '\"') ){
            getline(test_lib,str , '\"') ;
            each_line.str("") ;
            each_line.clear() ;
            each_line << str  ;
            while ( getline(each_line,value,',') ){
                one_row.push_back (value) ;
            }
            //getline(test_lib,str , '\"') ;
            //one_row.push_back (str) ;
            if (a == 1)
                NAND2_cell_rise.push_back(one_row) ;
            else if (a==2)
                NAND2_cell_fall.push_back(one_row) ;
            else if (a==3)
                NAND2_rise_transition.push_back(one_row) ;
            else
                NAND2_fall_transition.push_back(one_row) ;

               one_row.clear() ;
            }
    }
    if (a == 4)
        break ;
  }
  
/*
  for (unsigned int i=0 ; i< NAND2_fall_transition.size() ; i++){
    for (unsigned int j=0 ; j< NAND2_fall_transition[i].size() ; j++){
        cout << NAND2_fall_transition[i][j] << " " ;
    }
    cout << endl  ;
  }
*/

}


void CIRCUIT::set_gate (ifstream& gate_file ) {

  const int INV  = 0 ;
  const int NAND = 1 ;
  const int NOR  = 2 ;
  const int PI   = 3 ;
  string new_str ; 
  
//  cout << "================for gate=======================" << endl ;
  Gate_map.clear() ;

  string str ;
  GATE* gptr = NULL ;
  bool input_flag = 0  ; 
  bool output_flag = 0 ; 
  bool slash_astrick_flag = 0 ; 

  stringstream each_line ;

 // cout<< " ===================NOR2、INV、NAND2=========================  " << endl ;

 // cout << Gate_map["N1"] -> get_name() << endl ;

  string A1 , A2 , I , ZN;
  string d_str ;
  GATE* gptr_ZN = NULL ;
  GATE* gptr_A1 = NULL ;
  GATE* gptr_A2 = NULL ;
  GATE* gptr_I ;
  string function_name ; 
  
  //each_line.str("") ;
  //each_line.clear() ;
  //each_line
  
  while ( gate_file >> str){
      each_line.str("") ;
      each_line.clear() ;
      
      new_str = "" ; 
       for (unsigned int i=0 ; i< str.size() ; i=i+1){
          if( i>=1 && str[i]== '/'  && str[i-1] == '/' ){
            getline(gate_file,str) ; 
            break ; 
          }else if(i>=1 && str[i-1]== '/'  && str[i] == '*' ){
            slash_astrick_flag = 1 ;
            i = i+1 ; 
          }else if (slash_astrick_flag) {
            if (i>=1 && str[i-1] == '*' && str[i] == '/'){
               slash_astrick_flag = 0 ;
            }
          }else {
            new_str = new_str + str[i] ;
          }
       }
       
      if (slash_astrick_flag == 1){
        continue ; 
      }
      
      if (new_str == "input" || new_str == "output" || new_str == "wire" ){
            if (str == "input"){
              input_flag = 1 ; 
            }else if (str == "output"){
              output_flag = 1 ; 
            }
            
            getline(gate_file , str , ' ' ) ;
            getline(gate_file , str , ';' ) ;
            each_line << str  ;
            
            while (getline(each_line , str , ',') ){
              new_str = "" ; 
               for (unsigned int i=0 ; i< str.size() ; i=i+1){
                 if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' ' || str[i] == '\n' || str[i] == '\t' )
                   continue ;
                 else
                   new_str = new_str + str[i] ; 
               }
              gptr = new GATE( new_str , 0 , 0.0 ) ;
              Netlist.push_back(gptr) ;
              if (input_flag){
                  PIlist.push_back (gptr) ;
                  gptr -> set_function(PI) ;
              }else if (output_flag){
                  POlist.push_back (gptr) ;
                  gptr -> set_output_c(0.03) ; 
              }  
              Gate_map.insert( pair<string , GATE*> (new_str , gptr)) ;
              //getline(each_line , str , ' ') ;
            }
            input_flag = 0 ;
            output_flag = 0 ; 
            
            each_line.str("") ;
            each_line.clear() ;  
            
            continue ; 
      }
    
 //   cout << new_str << endl ;
     if (new_str == "NOR2X1"){
         //cout << "NOR2X1" << endl ;
         
         getline(gate_file , str , '(') ;
         function_name = "" ; 
         while (true){
           for (unsigned int i=0 ; i< str.size() ; i=i+1){
             if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' '  || str[i] == '\t' )
               continue ;
             else
               function_name = function_name + str[i] ; 
           }
           
           if ( function_name == "" )
             getline(gate_file , str , '(') ;
           else 
             break ;
           
         }
         //cout << 22 << function_name << endl ;
         
         getline(gate_file , str , ';') ;
         each_line << str ;
         while ( getline(each_line , d_str , '.') ){
         //  cout << d_str << endl ;
           getline(each_line , d_str , '(') ;
          // cout << d_str << endl ;
           if (d_str == "ZN"){
             getline(each_line , str , ')') ;
             new_str = "" ; 
             for (unsigned int i=0 ; i< str.size() ; i=i+1){
               if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' ' || str[i] == '\n' || str[i] == '\t' )
                 continue ;
               else
                 new_str = new_str + str[i] ; 
             }
             ZN = str ;
    //         cout << "ZN " << ZN << endl ;

             Gate_it = Gate_map.find(ZN) ;
             if ( Gate_it != Gate_map.end()){
               gptr_ZN =  Gate_it -> second ;
               gptr_ZN -> set_function ( NOR ) ;
               gptr_ZN -> set_function_name ( function_name ) ;
             }
           }else if (d_str == "A1"){
             getline(each_line , str , ')') ;
             new_str = "" ; 
             for (unsigned int i=0 ; i< str.size() ; i=i+1){
               if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' ' || str[i] == '\n' || str[i] == '\t' )
                 continue ;
               else
                 new_str = new_str + str[i] ; 
             }
             A1 = str ;
   //          cout << "A1 "<< A1 << endl ;

             Gate_it = Gate_map.find(A1) ;
             if ( Gate_it != Gate_map.end()){
               gptr_A1 =  Gate_it -> second ;
               gptr_A1 -> set_output_c ( stod(NOR2_input_c[0]) +  gptr_A1->get_output_c() ) ;
              // cout << " " << gptr -> get_output_c() ;
             }
           }else if (d_str == "A2"){
             getline(each_line , str , ')') ;
             new_str = "" ; 
             for (unsigned int i=0 ; i< str.size() ; i=i+1){
               if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' ' || str[i] == '\n' || str[i] == '\t' )
                 continue ;
               else
                 new_str = new_str + str[i] ; 
             }
             A2 = str ;
   //          cout << "A2 "<< A2 << endl ;

             Gate_it = Gate_map.find(A2) ;
             if ( Gate_it != Gate_map.end()){
               gptr_A2 =  Gate_it -> second ;
               gptr_A2 -> set_output_c ( stod(NOR2_input_c[1]) +  gptr_A2->get_output_c()  ) ;
             }
           }
         }

         gptr_ZN -> set_Fan_in_list(gptr_A1) ;
         gptr_ZN -> set_Fan_in_list(gptr_A2) ;
         gptr_A1 -> set_Fan_out_list(gptr_ZN) ;
         gptr_A2 -> set_Fan_out_list(gptr_ZN) ;

     } else if (new_str == "INVX1") {
        // cout << "INVX1" << endl ;
         getline(gate_file , str , '(') ;
         function_name = "" ; 
         while (true){
           for (unsigned int i=0 ; i< str.size() ; i=i+1){
             if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' '  || str[i] == '\t' )
               continue ;
             else
               function_name = function_name + str[i] ; 
           }
           
           if ( function_name == "" )
             getline(gate_file , str , '(') ;
           else 
             break ;
           
         }
         
         getline(gate_file , str , ';') ;
         each_line << str ;
         while ( getline(each_line , d_str , '.') ){
         //  cout << d_str << endl ;
           getline(each_line , d_str , '(') ;
          // cout << d_str << endl ;
           if (d_str == "ZN"){
             getline(each_line , str , ')') ;
             new_str = "" ; 
             for (unsigned int i=0 ; i< str.size() ; i=i+1){
               if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' ' || str[i] == '\n' || str[i] == '\t' )
                 continue ;
               else
                 new_str = new_str + str[i] ; 
             }
             ZN = str ;
      //       cout << "ZN "<< ZN << endl ;

             Gate_it = Gate_map.find(ZN) ;
             if ( Gate_it != Gate_map.end()){
               gptr_ZN =  Gate_it -> second ;
               gptr_ZN -> set_function ( INV ) ;
               gptr_ZN -> set_function_name ( function_name ) ;
             }
           }else if (d_str == "I"){
             getline(each_line , str , ')') ;
             new_str = "" ; 
             for (unsigned int i=0 ; i< str.size() ; i=i+1){
               if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' ' || str[i] == '\n' || str[i] == '\t' )
                 continue ;
               else
                 new_str = new_str + str[i] ; 
             }
             I = str ;
      //       cout << "I "<< I << endl ;

             Gate_it = Gate_map.find(I) ;
             if ( Gate_it != Gate_map.end()){
               gptr_I =  Gate_it -> second ;
               gptr_I -> set_output_c ( stod(INV_input_c) +  gptr_I->get_output_c() ) ;
              // cout << " " << gptr -> get_output_c() ;
             }
           }
         }

         gptr_ZN -> set_Fan_in_list(gptr_I) ;
         gptr_I -> set_Fan_out_list(gptr_ZN) ;

     } else if (new_str == "NANDX1") {
         // cout << "NANDX1" << endl ;
         getline(gate_file , str , '(') ;
         function_name = "" ; 
         while (true){
           for (unsigned int i=0 ; i< str.size() ; i=i+1){
             if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' '  || str[i] == '\t' )
               continue ;
             else
               function_name = function_name + str[i] ; 
           }
           
           if ( function_name == "" )
             getline(gate_file , str , '(') ;
           else 
             break ;
           
         }
         getline(gate_file , str , ';') ;
         each_line << str ;
         while ( getline(each_line , d_str , '.') ){
         //  cout << d_str << endl ;
           getline(each_line , d_str , '(') ;
          // cout << d_str << endl ;
           if (d_str == "ZN"){
             getline(each_line , str , ')') ;
             new_str = "" ; 
             for (unsigned int i=0 ; i< str.size() ; i=i+1){
               if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' ' || str[i] == '\n' || str[i] == '\t' )
                 continue ;
               else
                 new_str = new_str + str[i] ; 
             }
             ZN = str ;
      //       cout << "ZN "<< ZN << endl ;

             Gate_it = Gate_map.find(ZN) ;
             if ( Gate_it != Gate_map.end()){
               gptr_ZN =  Gate_it -> second ;
               gptr_ZN -> set_function ( NAND ) ;
               gptr_ZN -> set_function_name ( function_name ) ;
             }
           }else if (d_str == "A1"){
             getline(each_line , str , ')') ;
             new_str = "" ; 
             for (unsigned int i=0 ; i< str.size() ; i=i+1){
               if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' ' || str[i] == '\n' || str[i] == '\t' )
                 continue ;
               else
                 new_str = new_str + str[i] ; 
             }
             A1 = str ;
      //       cout << "A1 "<< A1 << endl ;

             Gate_it = Gate_map.find(A1) ;
             if ( Gate_it != Gate_map.end()){
               gptr_A1 =  Gate_it -> second ;
               gptr_A1 -> set_output_c ( stod(NAND2_input_c[0]) +  gptr_A1->get_output_c() ) ;
              // cout << " " << gptr -> get_output_c() ;
             }
           }else if (d_str == "A2"){
             getline(each_line , str , ')') ;
             new_str = "" ; 
             for (unsigned int i=0 ; i< str.size() ; i=i+1){
               if( str[i] == '/'  || str[i] == '*' || str[i] == '\\' || str[i] == ' ' || str[i] == '\n' || str[i] == '\t' )
                 continue ;
               else
                 new_str = new_str + str[i] ; 
             }
             A2 = str ;
   //         cout << "A2 "<< A2 << endl ;

             Gate_it = Gate_map.find(A2) ;
             if ( Gate_it != Gate_map.end()){
               gptr_A2 =  Gate_it -> second ;
               gptr_A2 -> set_output_c ( stod(NAND2_input_c[1]) +  gptr_A2->get_output_c() ) ;
             }
           }
         }

         gptr_ZN -> set_Fan_in_list(gptr_A1) ;
         gptr_ZN -> set_Fan_in_list(gptr_A2) ;
         gptr_A1 -> set_Fan_out_list(gptr_ZN) ;
         gptr_A2 -> set_Fan_out_list(gptr_ZN) ;
     }

  }
  /*
  cout << "=========================================" << endl ;
  cout << "NOR2  _ A1_C : " <<  NOR2_input_c[0]   << endl ;
  cout << "NOR2  _ A2_C : " <<  NOR2_input_c[1]   << endl ;
  cout << "NAND2 _ A1_C : " <<  NAND2_input_c[0]   << endl ;
  cout << "NAND2 _ A2_C : " <<  NAND2_input_c[1]   << endl ;
  cout << "INV   _    C : " <<  INV_input_c   << endl ;
  for (map<string , GATE*>::iterator it = Gate_map.begin() ; it != Gate_map.end() ; it++ ){
    cout << "pointer_name : "  << setw(5) << (*it).second -> get_name() << " function_name " << setw(5) <<((*it).second) -> get_function_name()  << setw(5) << ((*it).second) -> get_function() << setw(20) <<  ((*it).second) -> get_output_c()  << endl ;
  }
  cout << endl ;
  */


}

void CIRCUIT::write_delay_file(ofstream& delay_file) {

   // ofstream delay_file ; 
    //cout << delay_file_name << endl ;
    //delay_file.open(delay_file_name) ; 
    
    sort (Netlist.begin() , Netlist.end() , sort_delay_value) ;
    
    for (vector<GATE*>::iterator it = Netlist.begin() ; it != Netlist.end() ; it++ ){
       if (  (*it) -> get_function() == 3)
         continue ; 
       else {
        //  cout << (*it) -> get_function_name() << " " << (*it) -> get_logic_value() << " " << (*it) -> get_propogation_delay()<< " " << (*it) -> get_transition_time() << endl ; 
          delay_file << (*it) -> get_function_name() << " " << (*it) -> get_logic_value() << " " << (*it) -> get_propogation_delay()<< " " << (*it) -> get_transition_time() << endl ; 
       }
    }
    
    delay_file << endl ;
    
    
    //delay_file.close() ; 
}

void CIRCUIT::write_load_file(string load_file_name) {

    ofstream load_file ; 
    //cout << load_file_name << endl ;
    load_file.open(load_file_name) ; 

 //   cout << " =========================After_sorting===============" << endl ;
  sort (Netlist.begin() , Netlist.end() , sort_c_value) ;
  for (vector<GATE*>::iterator it = Netlist.begin() ; it != Netlist.end() ; it++ ){
   // cout << "string : "  << setw(5) << (*it) -> get_name() << " pointer_name " << setw(5) <<((*it)) -> get_function_name()  << setw(5) << ((*it)) -> get_function() << setw(20) << "logic value : " << ((*it)) -> get_logic_value() << setw(20) <<  ((*it)) -> get_output_c()  << endl ;
    
    if ( (*it) -> get_function_name() == "")
       continue ;
    else
      load_file << (*it) -> get_function_name() << " " << ((*it)) -> get_output_c()  << endl ;
  }
  
  
    load_file.close() ; 
}


