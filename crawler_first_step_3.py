# Author:Haozhen Liu
# # -*- coding: utf-8 -*-
import sys
# reload(sys)
# sys.setdefaultencoding("utf-8")
import html5lib
import urllib
import requests
import bs4
from bs4 import BeautifulSoup
import csv


def main(url="",name=""):
    proxy_support = urllib.request.ProxyHandler({'sock5': 'localhost:1080'})
    opener = urllib.request.build_opener(proxy_support)
    urllib.request.install_opener(opener)
    # url_journal = "http://dblp.uni-trier.de/db/journals/nn/"
    url_journal = url
    response_journal = urllib.request.urlopen(url_journal)
    soup_journal = BeautifulSoup(
        response_journal.read().decode("utf-8"), "html5lib")

    # proxy_info = { 'host' : 'web-proxy.oa.com','port' : 8080 }

    # # We create a handler for the proxy
    # proxy_support = urllib2.ProxyHandler({"http" : "http://%(host)s:%(port)d" % proxy_info})

    # # We create an opener which uses this handler:
    # opener = urllib2.build_opener(proxy_support)

    # # Then we install this opener as the default opener for urllib2:
    # urllib2.install_opener(opener)

    Year = ""
    Journal = name
    paper_id = 1
    total_paper = []
    first_row = ["ID", "Title", "Year", "Journal", "Author"]
    total_paper.append(first_row)
    # print(soup_journal)

    for tag in soup_journal.find_all("li"):
        if len(tag.contents) > 0:
            contents = tag.contents
            if "Volume" in contents[0]:
                if int(str(contents[0].split(":")[0][:4])) > 1999:
                    print(int(str(contents[0].split(":")[0])))
                    temp = []
                    temp = contents[1:]
                    for element in temp:
                        if isinstance(element, bs4.element.NavigableString) == False:
                            for (key, value) in element.attrs.items():
                                # 解析每个volume下的文章
                                print(key, value)
                                url_volume = value  # value表示url
                                response_volume = requests.get(url_volume)
                                soup_volume = BeautifulSoup(
                                    response_volume.content.decode("utf-8"), "html5lib")

                                volume_paper_temp = []  # 暂时存放下面计算得到的数据

                                # find tags whose class = data
                                for tag_volume in soup_volume.find_all("div", attrs={"class": "data"}):
                                    temp_paper = []
                                    paper_title = []
                                    paper_author = ""
                                    #print (tag_volume)
                                    # 处理每篇文章
                                    for element_volume in tag_volume.contents:
                                        # 判断该元素是不是一个标签
                                        if isinstance(element_volume, bs4.element.Tag):
                                            # 判断该元素是不是一个作者标签
                                            if element_volume.has_attr("itemtype"): #author
                                                if paper_author=="":
                                                    paper_author+=str(element_volume.string)
                                                else:
                                                    paper_author=paper_author+";"+str(element_volume.string)
                                            elif element_volume.has_attr("class"):  #title
                                                paper_title.append(
                                                    str(element_volume.string))
                                                volume_paper_temp.append(
                                                    str(element_volume.string))

                                    # 保存每篇文章的信息
                                    temp_paper.append(str(paper_id))
                                    temp_paper.extend(paper_title)
                                    temp_paper.append(Year)
                                    temp_paper.append(Journal)
                                    temp_paper.append(paper_author)
                                    # 将信息保存到总的列表中
                                    total_paper.append(temp_paper)
                                    paper_id = paper_id + 1
                else:
                    break

    # for x in total_paper:
    #   print(x)

    #保存到csv文件中
    with open("papers_"+name+".csv", "w", encoding="utf-8") as ft:
        writer = csv.writer(ft)
        #writer_file = io.StringIO()
        for x in total_paper:
            writer.writerow(x)


if __name__ == '__main__':
    main(url="http://dblp.uni-trier.de/db/journals/ijcv/",name="International Journal of Computer Vision")
