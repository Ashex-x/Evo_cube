import requests

class Deepseek_api:
    def __init__(self):
        self.apiKey = "sk-f7efbc5143204001aa97633dd04310e2"
        self.url = "https://api.deepseek.com/v1/chat/completions"


    def launch_Deepseek(self, content):
        
        # content += "\nPlease pick up one emoji(just one word) to reply(1st line):[Smile, Sad, Cry, Wear sunglasses, Love] \nIf you don't Know which emoji to choose, just reply [None]"
        # content += "\nAnd there is a LED you can control, please reply(2nd line):Brightness[Down, Up](choose one), color"
        print(content)

        headers = {
            "Content-Type": "application/json",
            "Authorization": f"Bearer {self.apiKey}"
        }
        data = {
            "model": "deepseek-chat",
            "messages": [
                {"role": "system", "content": "Reply format(all blackets are not allowed):\n"
                "1st line:Pick up just one emoji to reply[Smile, Sad, Cry, Wear sunglasses, Love](if you don't know which one to choose, just reply[None])\n"
                # "2nd line:(you can control a LED)Swhich[on, off], Brightness[+30, +10, -10, -30], color"
                },
                {"role": "user", "content": content}
            ],
            "temperature": 0.3,
            "max_tokens": 200,
            "stream": False
        }
        response = requests.post(self.url, headers=headers, json=data)

        result = response.json()
        print(response.status_code)
        emoji = result["choices"][0]["message"]["content"]
        
        return emoji
            