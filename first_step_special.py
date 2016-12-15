# Author:Haozhen Liu
# # -*- coding: utf-8 -*-
import sys;
reload(sys);
sys.setdefaultencoding("utf-8");

import requests;
import bs4;
from bs4 import BeautifulSoup;
import csv;

def main(url="",name=""):
    url_journal=url;
    response=requests.get(url_journal);
    soup=BeautifulSoup(response.content.decode("utf-8"),"html5lib");

    # 定义变量
    Year="";
    Journal=name;
    paper_id=1;
    total_paper=[];
    first_row=["ID","Title","Year","Journal","Author"];
    total_paper.append(first_row);

    for tag in soup.find_all("li"):
        if len(tag.contents) > 0:
            contents=tag.contents;
            if isinstance(contents[0],bs4.element.Tag) and "Volume" in contents[0].text:
                str_need=contents[0].text;# 把需要的信息保留下来
                print str_need
                print "123"
                if int(str(str_need.split(":")[1][:5])) > 1999:
                    print str(str_need.split(":")[1]);
                    Year=str(str_need.split(":")[1]);# 把年份保存下来
                    for element in contents:
                        if isinstance(element,bs4.element.NavigableString)==False:
                            for (key,value) in element.attrs.items():
                                # 解析每个volume下的文章
                                url_volume=value;# value表示url
                                response_volume=requests.get(url_volume);
                                soup_volume=BeautifulSoup(response_volume.content.decode("utf-8"),"html5lib");

                                volume_paper_temp=[];# 暂时存放下面计算得到的数据

                                for tag_volume in soup_volume.find_all("div",attrs={"class": "data"}):
                                    temp_paper=[];
                                    paper_title=[];
                                    paper_author="";

                                    # 处理每篇文章
                                    for element_volume in tag_volume.contents:
                                        # 判断该元素是不是一个标签
                                        if isinstance(element_volume,bs4.element.Tag):
                                            # 判断该元素是不是一个作者标签
                                            if element_volume.has_attr("itemtype"):
                                                if paper_author=="":
                                                    paper_author+=str(element_volume.string);
                                                else:
                                                    paper_author=paper_author+","+str(element_volume.string);
                                            elif element_volume.has_attr("class"):
                                                paper_title.append(str(element_volume.string));
                                                volume_paper_temp.append(str(element_volume.string));

                                    # 保存每篇文章的信息
                                    temp_paper.append(str(paper_id));
                                    temp_paper.extend(paper_title);
                                    temp_paper.append(Year);
                                    temp_paper.append(Journal);
                                    temp_paper.append(paper_author);
                                    # 将信息保存到总的列表中
                                    total_paper.append(temp_paper);
                                    paper_id=paper_id+1;
                else:
                    break;

    # 保存到csv文件中
    with open("papers_"+name+".csv","wb") as ft:
        writer=csv.writer(ft);
        for x in total_paper:
            writer.writerow(x);


if __name__ == '__main__':
    main(url="http://dblp.uni-trier.de/db/journals/jmlr/",name="Journal of Machine Learning Research");