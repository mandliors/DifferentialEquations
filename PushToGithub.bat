@echo off
set /p message="message: "
git add *
git commit -m "%message%"
git push origin master
pause