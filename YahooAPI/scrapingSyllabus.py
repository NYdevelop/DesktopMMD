
#import http_req
# import mp3
import requests
import os

from bs4 import BeautifulSoup

def get_news():
    html = requests.get('https://news.yahoo.co.jp')
    print("request ret:" + str(html.status_code))

    yahoo = BeautifulSoup(html.content, "html.parser")

    numStr = ["ぜろ", "いち", "に", "さん", "よん", "ご", "ろく", "なな", "はち", "きゅう"]
    fileList = []
    count = 1;
    voiceText = ""
    for title in yahoo.select("ul.topicsList_main > li > a"):
        string = numStr[count] + "、" + title.getText().replace("\n", "").replace(" ", "").replace("NEW", "")
        #print(string)
        voiceText += string + "。"
        count += 1
    #     mp3file = http_req.getMp3(string)
    #     mp3.play(mp3file)
    #     fileList.append(mp3file)
    return voiceText


if __name__ == '__main__':
    voiceText = get_news();
    with open('text.txt', mode='w') as f:
        f.write(voiceText)

    # for file in fileList:
    #     os.remove(file)

    #cmd = "SofTalk.exe" + " /X:1 /W:" + voiceText
    #print(cmd)

    # sts = os.system(cmd)
