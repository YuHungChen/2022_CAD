第一步驟 : 先求出所有prime implicant
(a.)	將onset、don’t care的數值  轉換成 bit  (用string呈現)
(b.)	進行evaluate () ， 將類似的4bit的string進行merge
(像是 1011、1111  1-11) 直到所有string都不能合併
(c.)	應題目要求，進行sorting後並輸出  

第二步驟 : 利用 Patrick’s method 找出minimum cover
(a.)	 針對 XX = X     會藉由 <set> 本身的功能去判斷
(b.)	 針對 X+XY = X   會將所有set中 擁有最少的string的 set代入到其他的set，看最少string的set的所有string是否同時存在於其他set當中，若存在，就把被代入的set給刪除。
例如 : XY + XYZ = XY ， 因為 XYZ 這個set 包含 XY 這個set ，所以XYZ會被刪除)
(c.)	依照Patrick’s method 所描述的內容，會先將只被一個prime implicant所包住的onset作為minimum cover的其中一個，接著將其餘的onset用Product Of Sum的方式呈現，再來展開形成Sum Of Product的形式，找出含有最少string的Sum of Product，若有相同最少數量的string，則比較literal大小。
(d.)	應題目要求，做sorting後並輸出
