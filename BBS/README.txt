/*****************************************

**Simple Rtree and BBS implementation**

:Author: Otto Huang
:Contact: xhomegg@gmail.com
:Revision: 1.0 beta
:Date: 21/07/2014

basic algorithm :Hilbert_R-tree
see more:http://en.wikipedia.org/wiki/Hilbert_R-tree

*******************************************/

本專案包含兩個程式檔
1.randomPoint.exe
2.BBS.exe

可先用randomPoint.exe產生所需要的隨機點(0<x,y<1000)

********************************************************
使用方法:

命令提示字元(包含exe檔的目錄下)輸入BBS.exe "filename"

BBS.exe 回要求使用者輸入R-tree 每個node的Entry大小
程式會依據Entry大小不同而導致執行的時間不同。

Ex:1000筆資料用100個entry會比用10個entry來的快。