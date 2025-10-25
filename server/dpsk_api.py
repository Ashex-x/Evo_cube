import requests

class Deepseek_api:
    def __init__(self):
        self.apiKey = "sk-f7efbc5143204001aa97633dd04310e2"
        self.url = "https://api.deepseek.com/v1/chat/completions"


    def initDeepseek(self):

        headers = {
            "Content-Type": "application/json",
            "Authorization": f"Bearer {self.apiKey}"
        }

        data = {
            "model": "deepseek-chat",
            "messages": [
                {"role": "user", "content": "Do you think a jok is really fun? Please pick up one emoji(just one word) to reply:[Smile, Cry, Angry, Grin]"}
            ],
            "temperature": 0.2,
            "max_tokens": 200
        }

        response = requests.post(self.url, headers=headers, json=data)
        result = response.json()

        print(response.status_code)
        print(result["choices"][0]["message"]["content"])

if (__name__ == "__main__"):
    dpsk = Deepseek_api()
    dpsk.initDeepseek()