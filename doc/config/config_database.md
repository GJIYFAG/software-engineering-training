*这个文件是用来讲解如何配置数据库的*
*this is file can tell you how to config database*

1. postgresql
首先，我们需要安装postgresql
❯ sudo pacman -S postgresql   
然后我们需要初始化数据库
initdb ... <略>

2. libpqxx
sudo pacman -S libpqxx
这个会自动安装这个C语言的libpq库
所以就算这个c++的库出现问题，也能使用c语言版本解决问题

