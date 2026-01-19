@echo off
setlocal enabledelayedexpansion


:: 获取当前Git分支名称
for /f "tokens=*" %%a in ('git symbolic-ref --short HEAD 2^>nul') do (
    set GIT_BRANCH_NAME="%%a"
)

:: 获取当前Commit id
for /f "tokens=*" %%a in ('git rev-parse --short HEAD 2^>nul') do (
    set GIT_COMMIT_ID="%%a"
)

:: 检查是否成功获取到分支名称
if "%GIT_BRANCH_NAME%"=="" (
	set GIT_BRANCH_NAME="unknown"
)

:: 检查是否成功获取到分支名称
if "%GIT_COMMIT_ID%"=="" (
	set GIT_COMMIT_ID="unknown"
)

:: 创建或更新git_info.h文件
echo #define GIT_BRANCH %GIT_BRANCH_NAME% > %~dp0/include/alglibrary/zkhyProjectHuaNan/git_info.h
echo #define GIT_COMMIT_ID %GIT_COMMIT_ID% >> %~dp0/include/alglibrary/zkhyProjectHuaNan/git_info.h
