import pandas as pd
import numpy as np
import requests

with open("LL(1) Parser Generator.html") as fp:
    df_list = pd.read_html(fp, encoding='ISO-8859-1')  # this parses all the tables in webpages to a list
    df = df_list[0]
    df.head()
    for index, row in df.iterrows():
        for i in df.head():
            r = row[i]
            if not (pd.isnull(r)):
                x = i.replace("$", "ending")
                try:
                    y = r.split("::=")[0]
                    rule = r.split("::=")[1].split()
                    if (rule == ["ε"]):
                        print(f"insertMember({x}, {y}, 0);")
                        continue
                    print(f"insertMember({x}, {y}, {len(rule)}", end="")
                    for l in rule:
                        l = l.replace("ε","eps").replace("$", "ending")                
                        print(f", \npartOfRulesRightSide({l})" ,end="")
                    print(");")
                except:
                    pass
