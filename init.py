import subprocess

# 你要克隆的远程仓库地址
remote_repo_url = "https://gitee.com/liaohailong190/helloplayer-3rdpart.git"
# 克隆到当前目录的文件夹名称
target_dir = "3rdpart"

def clone_repo():
    try:
        print(f"Cloning {remote_repo_url} into {target_dir} ...")
        # 执行 git clone 命令
        subprocess.run(["git", "clone", remote_repo_url, target_dir], check=True)
        print("Repository cloned successfully!")
    except subprocess.CalledProcessError as e:
        print("Error occurred while cloning the repository:")
        print(e)

if __name__ == "__main__":
    clone_repo()
