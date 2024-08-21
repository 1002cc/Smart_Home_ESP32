import requests

API_KEY = "P9SYyz2QOA4lsXjFLv8DRv8p"
SECRET_KEY = "rKFhtw5D3H3pRUPY44jrvsb46Qr3prSr"


def main():
    url = "https://tsn.baidu.com/text2audio"
    print(get_access_token())
    payload = 'tex=你好&tok=' + get_access_token() + '&cuid=aAzA4gBN5mmIViCqofGDaMUseHsVs4AS&ctp=1&lan=zh&spd=5&pit=5&vol=5&per=1&aue=3'
    headers = {
        'Content-Type': 'application/x-www-form-urlencoded',
        'Accept': '*/*'
    }

    response = requests.request("POST", url, headers=headers, data=payload)

    print(response.text)


def get_access_token():
    """
    使用 AK，SK 生成鉴权签名（Access Token）
    :return: access_token，或是None(如果错误)
    """
    url = "https://aip.baidubce.com/oauth/2.0/token"
    params = {"grant_type": "client_credentials", "client_id": API_KEY, "client_secret": SECRET_KEY}
    return str(requests.post(url, params=params).json().get("access_token"))


if __name__ == '__main__':
    main()
