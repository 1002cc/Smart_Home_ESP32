import requests
import sys
import tqdm

def download_audio():
    while True:
        text = input("请输入要转换为音频的文本内容（输入'q'退出）：")
        if text == 'q':
            break
        output_filename = input("请输入输出音频文件名：")
        group_id = "1793262119999783852"
        api_key = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJHcm91cE5hbWUiOiLpmYjlhYnlro8iLCJVc2VyTmFtZSI6IumZiOWFieWujyIsIkFjY291bnQiOiIiLCJTdWJqZWN0SUQiOiIxNzkzMjYyMTIwMDA4MTcyNDYwIiwiUGhvbmUiOiIxODkyNjI0NzExMiIsIkdyb3VwSUQiOiIxNzkzMjYyMTE5OTk5NzgzODUyIiwiUGFnZU5hbWUiOiIiLCJNYWlsIjoiIiwiQ3JlYXRlVGltZSI6IjIwMjQtMDYtMjIgMTU6NDc6MTYiLCJpc3MiOiJtaW5pbWF4In0.XCozhyQkIr1r-_JXamid9sOa4A83v9dTcsyHj5COIi1bCgrzD4ANb-ZpNBIRs_qmvx5mUobo9Q3u890JzDllimit15QiXhDVv8kV71jJcoW-i0CfXC2N5HagOVgIGpKi3CZ12X1c3szjcSjjFAseq1djFuzRp6lCwxtVrqCSJz8Nwx2FO_Q0ZdWKxso73i6MfgfJAkfLHqc87SFwZXmvun08JVZrgwlDOF3QqvXce002Tpa6h9d6y1dG_cs-hXS4Br31h-3W7g_JAfywOz5yVSLaXi5ghnRRHqTXohSSzhuA9ZcS4h0LDI81425GVbTmKwapu9Op26YV69hK6IPyFg"
        url = f"https://api.minimax.chat/v1/t2a_pro?GroupId={group_id}"
        headers = {
            "Authorization": f"Bearer {api_key}",
            "Content-Type": "application/json",
        }
        data = {
            "voice_id": "audiobook_female_1",
            "text": text,
            "model": "speech-01",
            "audio_sample_rate": 32000,
            "bitrate": 128000,
        }
        response = requests.post(url, headers=headers, json=data)

        audio_url = response.json()['audio_file']
        audio_response = requests.get(audio_url, stream=True)
        total_size = int(audio_response.headers.get('content-length', 0))
        block_size = 1024
        progress_bar = tqdm.tqdm(total=total_size, unit='iB', unit_scale=True)
        with open(output_filename, 'wb') as f:
            for data in audio_response.iter_content(block_size):
                progress_bar.update(len(data))
                f.write(data)
        progress_bar.close()

if __name__ == "__main__":
    download_audio()