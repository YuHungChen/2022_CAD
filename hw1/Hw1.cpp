#include <iostream>
#include <fstream>
#include <string>

#include <vector>
#include <algorithm>
#include <set>

using namespace std ;


bool check_merge( string one , string two ) {

    int a = 0 ;

    for (int i = 0 ; i< one.length() ; i++){
        if (one[i] != two[i])
            a++ ;
    }

    return (a==1) ;
}


string Merge_implicant(string one , string two ){


    for (int i=0 ; i< one.length() ; i++){
        if (one[i] != two[i])
            one[i] = '-' ;
    }

    return one ;
}

bool sort_implicant(string one , string two ){

    int count_one = 0 ;
    int count_two = 0 ;

    for (int i=0 ; i< one.length() ; i++){
        if (one[i] == '1')
            count_one ++ ;

        if (two[i] == '1')
            count_two ++ ;
    }



    return (count_one < count_two) ;
}


void  evaluate (vector <string>& All_2bit , vector<string>& PM_implicant ){

    vector<string> temp ;
    string s ;
    vector<bool> flag ;
    flag.reserve(All_2bit.size()) ;
    for (int i=0 ; i<= All_2bit.size() ; i++){
        flag.push_back(false) ;
    }


    bool All_PM_implicant = true  ;
    for (int i=0 ; i < All_2bit.size() ; i++){
        for (int j = i+1 ; j < All_2bit.size() ; j++){
            if (check_merge (All_2bit[i],All_2bit[j])){
                s = Merge_implicant (All_2bit[i],All_2bit[j]) ;
                if ( find(temp.begin() , temp.end() , s ) == temp.end()  ){
                    temp.push_back(s) ;
                   // cout << s << endl ;
                }

                flag[i] = true ;
                flag[j] = true ;
                All_PM_implicant = false ;
            }
        }

        if (flag[i] == false)   // prime implicant
            PM_implicant.push_back(All_2bit[i]) ;

   //     cout << "***************" << endl ;
    }

    All_2bit.clear() ;

    for (auto k = temp.begin() ; k<temp.end() ; k++){
         //cout << *k<< endl ;
        All_2bit.push_back(*k) ;
    }
   // cout << "===================================" << endl ;
    if (All_PM_implicant == false)
        evaluate(All_2bit , PM_implicant) ;

}


// 1. X + XY = X
// 2. XX = X
// 3. X + X = X

bool PM_check_onset(string onset , string Prime_I ) ;
void expansion(vector< set<string> > &Sum_of_Product   , vector<string> sum ) ;
int count_literal(set<string> & chosen );
void k_map_merge(vector< set<string> > &Sum_of_Product);

void Petrick_method (vector<string> &PM_implicant , vector<string> &onset_list){

    vector<string> sum ;
    vector< vector<string> > product ;
    //vector<bool>

    // ====== form the table =======
    for (int i=0 ; i < onset_list.size() ; i++ ){
        for (int j=0 ; j< PM_implicant.size() ; j++){
            if (PM_check_onset( onset_list[i] , PM_implicant[j]  ) ){
                sum.push_back(PM_implicant[j]) ;
            }
        }
        product.push_back(sum) ;
        sum.clear() ;
    }
    /*
    for ( int i=0 ; i< product.size() ; i++){
        for (int j=0 ; j< product[i].size() ; j++){
            cout << product[i][j] << endl ;
        }
        cout << "=======new_product====" << endl ;
    }
    */
    PM_implicant.clear() ;

    vector <bool> flag ;
    for (int i=0 ; i<onset_list.size() ; i++){
        flag.push_back(false) ;
    }

    for (int i=0 ; i<product.size() ; i++){
        if (product[i].size() == 1  && flag[i] == false){    // if just only one implicant can include the onset list
  //          cout << i<< endl ;
   //         cout << "=========="<< endl ;
            for (int j=0 ; j< onset_list.size() ; j++){
                if (PM_check_onset (onset_list[j] , product[i][0]) ){
                    flag[j] = true ;
                }
            }
            PM_implicant.push_back(product[i][0]) ;
        }
    }

/*
    for (int i=0 ; i<onset_list.size() ; i++){
        cout << flag[i] << endl ;
    }
*/
  //  cout << "======init_PM======" <<endl ;
//    for (int i=0 ; i<PM_implicant.size() ; i++){
 //       cout << PM_implicant[i] << endl ;
  //  }

    vector< set<string> > Sum_of_Product  ;

    for (int i = onset_list.size() -1 ; i>=0 ; i-- ){
        if (flag[i] == true)
            product.pop_back() ;
        else
            expansion(Sum_of_Product , product[i] ) ;

       // cout << Sum_of_Product.size() << endl ;

       /*
        for (int j=0 ; j<Sum_of_Product.size() ; j++){
            cout << " set : " << j << endl ;
            for (auto it=Sum_of_Product[j].begin() ; it!=Sum_of_Product[j].end() ; it++){
                cout << *it << endl ;
            }
        }
        cout << "===next_product ===" << endl ;
        */
    }
   // after expansion() ;
      int minterm_idx = 0  ;
      int minterm_num = 0 ;
      int literal = 0 ;
      int temp_num = 0 ;

      for (int i=0 ; i<Sum_of_Product.size() ; i++){
          if (i==0){
            minterm_num = Sum_of_Product[i].size() ;
            literal = count_literal(Sum_of_Product[i]) ;

          }else if (Sum_of_Product[i].size() == minterm_num){
            temp_num = count_literal(Sum_of_Product[i]) ;
            if (temp_num < literal){
                minterm_idx = i ;
                literal = temp_num ;
            }
          }else if (Sum_of_Product[i].size() < minterm_num){
            minterm_idx = i ;
            minterm_num = Sum_of_Product[i].size() ;
            literal = count_literal(Sum_of_Product[i]) ;
          }
      }

    if (Sum_of_Product.size() != 0 ){
      for (auto it = Sum_of_Product[minterm_idx].begin() ; it!= Sum_of_Product[minterm_idx].end() ; it++ ){
        // cout << *it << endl ;
         PM_implicant.push_back(*it) ;

      }
    }

   // choose_min_lateral() ;

}

void expansion(vector< set<string> > &Sum_of_Product   , vector<string> sum ) {



    vector< set<string> > temp ;
    temp.reserve(Sum_of_Product.size() ) ;
    for (int i=0 ; i< Sum_of_Product.size() ; i++)
        temp.push_back(Sum_of_Product[i]) ;

    set<string> new_set ;

    bool flag = Sum_of_Product.empty() ;

    for (int i=0 ; i< sum.size() ; i++ ){
        if (flag){
            new_set.insert(sum[i]) ;
            Sum_of_Product.push_back(new_set) ;
            new_set.clear() ;
        }else if (i==0){
            for (int j=0 ; j < Sum_of_Product.size() ; j++){
                Sum_of_Product[j].insert(sum[0]) ;
            }
        }else {
              for (int j=0 ; j < temp.size() ; j++){
                //    cout << 1 ;
                new_set.insert(temp[j].begin() , temp[j].end() ) ;
                new_set.insert(sum[i]) ;

                Sum_of_Product.push_back(new_set) ;
                new_set.clear() ;
            }
        }
    }

   // for debug
  /*
      cout << "=====before delete ======" << endl ;
      for (int i=0 ; i<Sum_of_Product.size() ; i++){
         for (auto j=Sum_of_Product[i].begin() ; j != Sum_of_Product[i].end() ; j++ ){
            cout << *j << endl ;
         }
         cout << "*********************" << endl ;
      }
*/
      k_map_merge( Sum_of_Product) ;
/*
      cout << "======after delete ======" << endl ;
      for (int i=0 ; i<Sum_of_Product.size() ; i++){
         for (auto j=Sum_of_Product[i].begin() ; j != Sum_of_Product[i].end() ; j++ ){
            cout << *j << endl ;
         }
         cout << "*********************" << endl ;
      }

      cout << "======finish_expansion=====" << endl ;
*/

}

void k_map_merge(vector< set<string> > &Sum_of_Product ) {

    int min_size = Sum_of_Product[0].size() ;
    vector<int> erase_idx ;
    bool to_merge = false ;

    for (int i=1 ; i< Sum_of_Product.size() ; i++){
        if (Sum_of_Product[i].size() > min_size )
            to_merge = true ;
        else if (Sum_of_Product[i].size() < min_size){
            to_merge = true ;
            min_size = Sum_of_Product[i].size() ;
        }
    }

    if (to_merge){
        for (int i=0 ; i< Sum_of_Product.size() ; i++){
            if (Sum_of_Product[i].size() == min_size){   // maybe can use this set to delete other sets
                for (int j=0 ; j< Sum_of_Product.size() ; j++){
                    if (j==i)
                        continue ;
                    else {
                        bool delete_set = true ;
                        for (auto it=Sum_of_Product[i].begin() ; it != Sum_of_Product[i].end() ; it++){
                            if (Sum_of_Product[j].find(*it) == Sum_of_Product[j].end() ){
                                delete_set = false ;
                                break ;
                            }
                        }

                        if (delete_set )
                            erase_idx.push_back(j) ;
                   }
                }
              break ; 
            }
        }
    }

    //  for debug
  //  cout << "=======" <<endl ;
    sort(erase_idx.begin() , erase_idx.end()) ;
    int last_idx ; 
    int now_ids ; 
    for (int i=erase_idx.size()-1 ; i>=0 ; i--){
        Sum_of_Product.erase(Sum_of_Product.begin() + erase_idx[i]  ) ;
      //  cout << erase_idx[i] << endl ;
    }





}

int count_literal(set<string> & chosen ){

    string s ;
    int number = 0 ;
    for (auto it=chosen.begin() ; it!=chosen.end() ; it++){
        s = *it ;
        for (int i = 0 ; i < s.size() ; i++){
            if (s[i] != '-')
                number ++ ;
        }
    }

    return number ;

}


bool PM_check_onset(string onset , string Prime_I ){

    bool flag = true ;

    for (int i=0 ; i < Prime_I.size() ; i++){
        if (Prime_I[i] == '-' )
            continue ;
        else if (onset[i] != Prime_I [i])
            flag = false ;
    }


    return flag ;
}



string int_to_bit(int number , int bit_num ){

    vector <char> My_Vector;
    My_Vector.reserve(bit_num) ;

    while( number != 0){
        if (number % 2 ==0)
            My_Vector.push_back('0') ;
        else
            My_Vector.push_back('1') ;

        number = number /2 ;
    }

    while (My_Vector.size() < bit_num)
        My_Vector.push_back('0') ;

    //cout << My_Vector.size()<< endl ;

    string return_str ;

    for (int i = My_Vector.size() -1 ; i>=0 ; i-- ){
        return_str = return_str + My_Vector[i] ;
    }

    //cout << return_str << endl ;

    return return_str ;

}

void Write_Prime_Implicant(ofstream& out_file , vector<string>& PM_implicant ){

    out_file << ".p " << PM_implicant.size() << endl ;
    for (int i= 0  ; i< PM_implicant.size() ; i++){
        if (i==15)
          break ;
        out_file << PM_implicant[i] << endl ;

    }
    out_file << endl ;
}

void Write_After_Patrick(ofstream& out_file , vector<string>& PM_implicant ){

    out_file << ".mc " << PM_implicant.size() << endl  ;
    int literal = 0  ;
    for (int i=0 ; i< PM_implicant.size() ; i++){
        out_file << PM_implicant[i] << endl ;
        for (int j=0 ; j<PM_implicant[i].size() ; j++){
            if (PM_implicant[i][j] != '-')
                literal ++ ;
        }
    }

    out_file << "literal=" <<literal ;
}

bool sort_output (string one , string two){



    for (int i=0 ; i< one.size() ; i++){
        if (one[i] == two [i]){
            continue ;
        }else {
            return (int(one[i]) < int(two[i])) ;
        }
    }

    return false ;
}

int main(int argc , char* argv[] ) {

    ifstream in1 ;
    ofstream out1 ;


    in1.open(argv[1]) ;
    out1.open(argv[2]) ;

    string word1 ;
    string bit_value ;
    vector <string> onset_list ;
    vector <string> All_2bit ;
    vector <string> PM_implicant ;

    int bit_num  ;
    in1 >> word1 ;
    in1 >> bit_num ;
    in1 >> word1 ;
    while (in1 >> word1 ){
        if (word1 == ".d")
            break ;

        bit_value = int_to_bit( stoi(word1) , bit_num ) ;

        onset_list.push_back(bit_value) ;
        All_2bit.push_back(bit_value) ;
    }

    while (in1 >> word1){
        bit_value = int_to_bit( stoi(word1) , bit_num ) ;
        All_2bit.push_back(bit_value) ;
    }

   // for (int i=0 ; i< onset_list.size() ; i++)
   //     cout << onset_list[i] << endl ;

   // sort (All_2bit.begin() , All_2bit.end() ) ;
    //sort (All_2bit.begin() , All_2bit.end() , sort_implicant ) ;

   // cout << "===========DC==========" << endl ;
  //  for (int i=0 ; i< All_2bit.size() ; i++)
  //      cout << All_2bit[i] << endl ;
  //  cout << "===================" << endl ;
    evaluate(All_2bit , PM_implicant) ;

//    for (int i=0 ; i < PM_implicant.size() ; i++){
//        cout << PM_implicant[i] << endl ;
//    }


    sort (PM_implicant.begin() , PM_implicant.end() , sort_output ) ;
 //   cout << "11" << endl ;
    Write_Prime_Implicant(out1 , PM_implicant) ;
 //   cout << "22" << endl ;
//    for (int i=0 ; i< onset_list.size() ; i++)
//        cout << onset_list[i] << endl ;

    Petrick_method(PM_implicant , onset_list) ;
  //  for (int i=0 ; i< PM_implicant.size() ; i++)
  //      cout << PM_implicant[i] << endl ;

    sort (PM_implicant.begin() , PM_implicant.end() , sort_output ) ;
 //   cout << "22" << endl ;
    Write_After_Patrick(out1 , PM_implicant) ;
  //  cout << "22" << endl ;
    in1.close() ;
    out1.close() ;


 return 0 ;
}

