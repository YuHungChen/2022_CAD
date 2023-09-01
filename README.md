# 2022_CAD

## HW1

* 第一步驟 : 先求出所有 prime implicant
  * (a.) 將 onset、don’t care 的數值 轉換成 bit (用 string 呈現)
  * (b.)進行 evaluate () ， 將類似的 4bit 的 string 進行 merge
      (像是 1011、1111 ➔ 1-11) 直到所有 string 都不能合併
  * (c.) 應題目要求，進行 sorting 後並輸出 
* 第二步驟 : 利用 Patrick’s method 找出 minimum cover
  * (a.) 針對 XX = X ➔ 會藉由 <set> 本身的功能去判斷
  * (b.) 針對 X+XY = X ➔ 會將所有 set 中 擁有最少的 string 的 set 代入到其
他的 set，看最少 string 的 set 的所有 string 是否同時存在於其他 set 當
中，若存在，就把被代入的 set 給刪除。
例如 : XY + XYZ = XY ， 因為 XYZ 這個 set 包含 XY 這個 set ，所以
XYZ 會被刪除)
  * (c.) 依照 Patrick’s method 所描述的內容，會先將只被一個 prime implicant
所包住的 onset 作為 minimum cover 的其中一個，接著將其餘的 onset
用 Product Of Sum 的方式呈現，再來展開形成 Sum Of Product 的形式，
找出含有最少 string 的 Sum of Product，若有相同最少數量的 string，則
比較 literal 大小。
  * (d.)應題目要求，做 sorting 後並輸出
## compile  
* g++ hw1.cpp  
## Execute  
* ./a.out [input_file] [output_file]  

## HW2 

* 第一步驟 : 
  * (a.) 先讀取 test_lib.lib，而因為助教是用的檔案與我們相同，所以只需專心
讀取 INV、NAND、NOR 的 transition time 就可以，讀出來的資料用二維
資料的方式存在 Circuit.h 的 vector 當中。
* 第二步驟 : 
  * (a.) 接著讀取 case.v file，將 case.v file 的 case 取出來當作我們的 txt 的部分
的 file name。
  * (b.)在讀取 case.v file 的過程，需特別注意 input wire output 的順序不同，
以及 “/* */ “、 “/* input */ “ 、 “ // ” 等，會讓讀取的資料不是正確
的。
  * (c.) 最後將 讀取的內容 input output wire 放在 netlist 的 vector 當中，
input 則放在 PIlist 的 vector 當中， output 則放在 POlist 的 vector 當
中，並特別建立一個 data strcture “ GATE ” ，每個 input output wire 都
視為一種 GATE，而 GATE 當中存有各自對應的 capacitance、function 名
稱、function、fanin 的 GATE、fanout 的 GATE，方便我們最後續運算
* 第三步驟 : 
  * (a.) 讀取 pattern file，需特別注意 input patteren 的順序並不會照著我們的
原本 PIlist 的順序，所以我另外採取 map 的方式存取資料，利用
function 去找到 pattern 中對應的 PIlist
* 第四步驟 : 
  * (a.) 在第一步驟讀檔以及第二步驟建 circuit 的過程，就能順便建立好所有
gate 的 output capacitance，也就能完成作業要求的第一部分
  * (b.)第二部分會利用查表的方式，外插及內插幫助找到所需要的值
  * (c.) 在每次選 input transition time 的過程，會需要選取最大的 input delay 當
作 input transition time，選取的 path 也正好是我們的 sensitizable 
path，就能藉此算出所有 output 的 longest path，比較誰有最大的 delay
就是 longest path，最小的 delay 就是 shortest path

## compile  
* make  
## Execute  
* ./hw2.o [circuit file] -p [pattern_file] -l [library_file]  

