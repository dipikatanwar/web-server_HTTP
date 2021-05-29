import requests
import threading
import os

def thread_function(r):
    print("in ", r)
    url = 'http://localhost:8888/index.html'
    print(url)
    try:
        requests.get(url)
    except Exception as e:
        print(e, url)
    # print(out.text)
    print("outf ", r)

for i in range(5):
    x = threading.Thread(target=thread_function, args=(i,))
    x.start()
    # print(i)


