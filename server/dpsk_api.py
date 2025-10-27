import requests

class Deepseek_api:
    def __init__(self):
        self.apiKey = "sk-f7efbc5143204001aa97633dd04310e2"
        self.url = "https://api.deepseek.com/v1/chat/completions"


    def launch_Deepseek(self, content):
        
        content = content + "\nPlease pick up one emoji(just one word) to reply:[Smile, Sad, Cry, Wear sunglasses, Love] \nIf you don't Know which emoji to choose, just reply [None]"
        print(content)

        headers = {
            "Content-Type": "application/json",
            "Authorization": f"Bearer {self.apiKey}"
        }
        data = {
            "model": "deepseek-chat",
            "messages": [
                {"role": "user", "content": content}
            ],
            "temperature": 0.2,
            "max_tokens": 200,
            "stream": False
        }
        response = requests.post(self.url, headers=headers, json=data)

        result = response.json()
        print(response.status_code)
        emoji = result["choices"][0]["message"]["content"]
        
        return emoji
            