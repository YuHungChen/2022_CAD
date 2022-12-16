第一步驟 : 
  (a.)	先讀取test_lib.lib，因為助教是用的檔案與我們相同，所以只需專心讀取INV、NAND、NOR的transition time 就可以，讀出來的資料用二維資料的方式存在Circuit.h的 vector當中。
第二步驟 : 
  (a.)	接著讀取case.v file，將case.v file 的case 取出來當作我們的txt的部分的file name。 
  (b.)	在讀取case.v file 的過程，需特別注意 input wire output的順序不同，以及 “/* */ “、 “/* input */ “ 、 “ // ” 等，會讓讀取的資料不是正確的。 
  (c.)	最後將 讀取的內容 input output wire 放在 netlist的 vector當中，input 則放在 PIlist的vector當中， output 則放在 POlist的vector當中，並特別建立一個data strcture “ GATE ” ，每個input output wire都視為一種GATE，而GATE當中存有各自對應的 capacitance、function名稱、function、fanin的GATE、fanout的GATE，方便我們最後續運算
第三步驟 : 
  (a.)	讀取pattern file，需特別注意input patteren的順序並不會照著我們的原本 PIlist的順序，所以我另外採取map的方式存取資料，利用function 去找到 pattern中對應的 PIlist
第四步驟 : 
  (a.)	在第一步驟讀檔以及第二步驟建circuit的過程，就能順便建立好所有gate的output capacitance，也就能完成作業要求的第一部分
  (b.)	第二部分會利用查表的方式，外插及內插幫助找到所需要的值
  (c.)	在每次選input transition time的過程，會需要選取最大的input delay當作input transition time，選取的path 也正好是我們的sensitizable path，就能藉此算出所有output的longest path，比較誰有最大的delay就是longest path，最小的delay就是 shortest path。 
