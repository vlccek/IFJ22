# IFJ22


## generator
 - kvůli tomu, že nejsou deklarace funkcí, tak při jejím zavolání nevíme datový typ návratové hodnoty \
    (a ani jestli vůbec něco vrací). Takže:
   - implicitně `undefinedDataType` datový typ, bude potřeba kontrola po naparsování celého programu
   - možná použít `nil` do té proměnné, i když funkce nic nevrací?

### Konvence
 - při vnitřní tříadresné instrukci jsou argumenty vždy `arg1` a `arg2`, kam se má uložit výsledek je pak v `dest`
 - 