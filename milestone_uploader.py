import requests
import json

# 설정
TOKEN = ""
REPO_OWNER = "Hyun-nine-CH"         # GitHub 사용자명
REPO_NAME = "securitySoftProd"              # 저장소 이름
API_URL = f"https://api.github.com/repos/{REPO_OWNER}/{REPO_NAME}/milestones"

headers = {
    "Authorization": f"Bearer {TOKEN}",
    "Accept": "application/vnd.github+json",
    "Content-Type": "application/json"
}

# JSON 파일 불러오기
with open("github_milestones_create_script.json", "r", encoding="utf-8") as file:
    milestones = json.load(file)

# 마일스톤 하나씩 등록
for m in milestones:
    response = requests.post(API_URL, headers=headers, json=m)
    if response.status_code == 201:
        print(f"[✅] {m['title']} 마일스톤 생성 완료")
    elif response.status_code == 422:
        print(f"[⚠️] {m['title']} 이미 존재함 (무시됨)")
    else:
        print(f"[❌] {m['title']} 생성 실패 → {response.status_code}, {response.text}")

