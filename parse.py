import pandas as pd
import numpy as np

with open("LL(1) Parser Generator.html") as fp:
    df_list = pd.read_html(fp, encoding='ISO-8859-1')  # this parses all the tables in webpages to a list
    df = df_list[0]
    df.head()
    for index, row in df.iterrows():
        for i in df.head():
            r = row[i]
            if not (pd.isnull(r)):
                x = i.replace("$", "ending").strip()
                try:
                    y = r.split("::=")[0].strip()
                    rule = r.split("::=")[1:]
                    for k in rule:
                        k = k.split()
                        if (k[0] == "Exp"):
                            if rule[-1].split() != k:
                                del k[-1]
                        if k == ["ε"]:
                            print(f"insertMember({x}, {y}, 0);")
                            continue
                        print(f"insertMember({x}, {y}, {len(k)}", end="")
                        for l in k:
                            l = l.replace("ε", "eps").replace("$", "ending").replace("S,", "ProgramBody,")
                            print(f",\n\t\tpartOfRulesRightSide({l})", end="")
                        print(");")
                except:
                    pass
