from bs4 import BeautifulSoup

with open("LL(1) Parser Generator.html") as fp:
    soup = BeautifulSoup(fp, 'html.parser')
    table = soup.find_all("table", {'class': 'pure-table pure-table-bordered'})[1]
    header_top = [i.text for i in table.find_all("th")][1::]
    header_left = [i.select("td") for i in table.select("#llTableRows > tr")]
    left = []
    for i in header_left:
        for l in i:
            if ( not ("::=" in l.text) and l.text != ""):
                left.append(l.text)

    header_left = left

    rows = soup.find_all("table", {'class': 'pure-table pure-table-bordered'})[1].find("tbody").find_all("tr")

    sloupec = 0
    for row in rows:
        cells = row.find_all("td")[1::]
        try:
            for i in range(len(cells)):
                x = header_left[sloupec].replace("$", "ending")
                y = header_top[i].replace("$", "ending")
                mc = cells[i].text
                if mc != "":
                    pravidla = mc.split("::=")[1].split()
                    delka = len(pravidla)
                    if "ε" in pravidla:
                        print(f""" insertMember({x}, {y}, 0 )""", end="\n")
                        continue
                    print(f""" insertMember({x}, {y}, {delka}, """, end="")
                    for pr in pravidla:
                        if pr == "$":
                            pr = "ending"
                        elif pr == "ε":
                            continue
                        print(f"""partOfRulesRightSide({pr})""", end="")
                        if (pr == pravidla[-1] or pr == "ending"):
                            print("", end="")
                        else:
                            print("," , end="")
                    print(");")
        except:
            pass
        sloupec+=1





