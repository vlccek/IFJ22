# IFJ22


## generator
 - kvůli tomu, že nejsou deklarace funkcí, tak při jejím zavolání nevíme datový typ návratové hodnoty \
    (a ani jestli vůbec něco vrací). Takže:
   - implicitně `undefinedDataType` datový typ, bude potřeba kontrola po naparsování celého programu
   - možná použít `nil` do té proměnné, i když funkce nic nevrací?
   - generování DEFVAR bude probíhat na začátku generování jdnoho arraye, při čemž se bude hlídat jedinečnost
     identifikátoru (aby nenastala redefinice) TODO!!

### Konvence

- při vnitřní tříadresné instrukci jsou argumenty vždy `arg1` a `arg2`, kam se má uložit výsledek je pak v `dest`
- volaná funkce dostane parametry a už definovanou proměnnou na return hodnotu s nastavenou hodnotou nil@nil v TF, sama
  pak bude pushovat frame jenom když bude volat nějakou další funkci.
- takže volající vytvoří TF, nastaví argumenty a return, zavolá funkci.
- volaná si pracuje v tomto TF, ale když bude volat nějakou funkci, tak ho pushne, a udělá viz bod výše
  -v

### Post processing

- Bude stačit procházet jednou.
- datové typy při interním generování nejsme schopní všechny znát