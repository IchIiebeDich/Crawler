# Author:Haozhen Liu
# # -*- coding: utf-8 -*-

import sys;
reload(sys);
sys.setdefaultencoding("utf-8");

import requests;
import csv;
import bs4;
import re;
from bs4 import BeautifulSoup;
import traceback;

#JOURNAL="Machine Translation";

def main(name=""):
	JOURNAL=name;
	# 构造URL需要用
	str1="http://www.scopus.com/results/results.uri?numberOfFields=0&src=s&clickedLink=&edit=&editSaveSearch=&origin=searchbasic&authorTab=&affiliationTab=&advancedTab=&scint=1&menu=search&tablin=&searchterm1=";
	str2="&field1=TITLE&dateType=Publication_Date_Type&yearFrom=Before+1960&yearTo=Present&loadDate=7&documenttype=All&subjects=LFSC&_subjects=on&subjects=HLSC&_subjects=on&subjects=PHSC&_subjects=on&subjects=SOSC&_subjects=on&st1="
	str3="&st2=&sot=b&sdt=b&sl=66&s=TITLE%28";
	str4="%29&sid=B1008C74CB35D28A3D2FA576D9FBD993.53bsOu7mi7A1NSY7fPJf1g%3A380&searchId=B1008C74CB35D28A3D2FA576D9FBD993.53bsOu7mi7A1NSY7fPJf1g%3A380&txGid=B1008C74CB35D28A3D2FA576D9FBD993.53bsOu7mi7A1NSY7fPJf1g%3A38&sort=plf-f&originationType=b&rr=&null="

	papers_all=[]# paper_title存储着文章名称
	first_row=["ID","Title","Year","Journal","Author"];
	papers_all.append(first_row);

	# 读取每篇论文的题目，构造URL
	count=0;
	with open("papers_"+name+".csv","rb") as ft:
		reader=csv.reader(ft);
		for row in reader:
			if row[0]!="ID":
				count=count+1;
				temp=row[1].split(" ");
				index=0;
				length=len(temp);
				str5="";
				while index<length-1:
					str5=str5+temp[index];
					str5=str5+"+";
					index=index+1;
				str5=str5+temp[index];
				str6=str1+str5+str2+str5+str3+str5+str4;
				url_paper=str6;#构造好的url

				try:
					# 解析该url，得到每篇文章的id
					response_paper=requests.get(url_paper);
					soup_paper=BeautifulSoup(response_paper.content.decode("utf-8"),"html5lib");

					# if soup_paper.find("a",text=)==None:
					# 	print "YES";
					# 	continue;

					for tag_paper in soup_paper.find_all("div",attrs={"class": "docMain"}):
						tag1=tag_paper.contents[3].contents[3].contents[1];
						tag2=tag_paper.contents[9].contents[3].contents[1];
				except BaseException:
					traceback.print_exc();# 把异常信息输出
				else:
					if True:#tag2.text==JOURNAL:
						http_str=str(tag1.attrs['href']);
						pid=str(re.findall('2-s2.0-\d+',http_str)[0]);# 得到每篇文章的id
						temp_row=[];
						temp_row.append(pid);
						temp_row.extend(row[1:]);
						papers_all.append(temp_row);# 把每篇文章的信息得到
						print count,pid;


	# 保存到csv文件中
	with open("papers_"+name+"_modify.csv","wb") as ft:
		writer=csv.writer(ft);
		for x in papers_all:
			writer.writerow(x);


if __name__ == '__main__':
	main(name="Journal of Machine Learning Research");