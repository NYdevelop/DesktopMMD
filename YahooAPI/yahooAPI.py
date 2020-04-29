
#coding:utf-8

import scrapingSyllabus

import sys
import urllib
import urllib.parse
import urllib.request
import xml.etree.ElementTree as ET

def yapi(text):
    url = 'https://jlp.yahooapis.jp/MAService/V1/parse?'
    appid = '取得したappid'
    params = urllib.parse.urlencode(
            {'appid': 'dj00aiZpPTdZQTFnMFZZczJWQiZzPWNvbnN1bWVyc2VjcmV0Jng9MmY-',
             'results':'ma',
             'sentence': text,
            })

    # print (url + params)
    response = urllib.request.urlopen(url + params)
    return response.read().decode('utf-8')

def do_xml(s):
    root = ET.fromstring(s)
    reading_list = []
    for i in root.iter():
        if i.tag == '{urn:yahoo:jp:jlp}reading':
            reading_list.append(i.text)

#    for i in reading_list:
#        print(i)
    return reading_list


if __name__ == '__main__':
    # load file : filename args[1]
    news = scrapingSyllabus.get_news()
    print(news)
    xml = yapi(news)
    reading_list = do_xml(xml)
    with open('sep_text.txt', mode='w') as sep_f:
        with open('text.txt', mode='w') as f:
            for r in reading_list:
                f.write(r);
                sep_f.write(r + "\n")