#!/bin/bash

# 安装Samba服务
sudo apt-get update
sudo apt-get install -y samba

# 备份原始配置文件
sudo cp /etc/samba/smb.conf /etc/samba/smb.conf.bak

# 创建Samba共享目录
sudo mkdir ~/samba
sudo chmod 777 ~/samba

# 设置默认的Samba共享
echo "[samba_share]" | sudo tee -a /etc/samba/smb.conf
echo "path = /home/$(whoami)/samba" | sudo tee -a /etc/samba/smb.conf
echo "available = yes" | sudo tee -a /etc/samba/smb.conf
echo "valid users = $(whoami)" | sudo tee -a /etc/samba/smb.conf
echo "read only = no" | sudo tee -a /etc/samba/smb.conf
echo "browsable = yes" | sudo tee -a /etc/samba/smb.conf

# 重启Samba服务
sudo ufw disabled
sudo systemctl restart smbd.service

# 打印Samba共享配置信息
echo "Samba服务器已经安装并配置成功。"
echo "共享文件夹路径：/home/$(whoami)/samba"
echo "共享名称：samba_share"
echo "登录账户：$(whoami)"

sudo smbpasswd -a dp
