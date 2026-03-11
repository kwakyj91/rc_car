# 🔎 혹시 모르니까 이것도 확인

## SSH 키가 agent에 등록됐는지

ssh-add -l

정상 예시

256 SHA256:xxxx id_ed25519 (ED25519)
⚡ 빠른 체크 (이거 3개만 알려주면 바로 원인 찾음)

터미널에서 결과 보여주세요.

ssh-add -l
cat ~/.ssh/id_ed25519.pub
git remote -v

그러면 왜 Permission denied 뜨는지 정확히 잡아줄게요.
