
ns
###############################################################################

function ShowInfo() {
    echo ""
    echo -e "\033[34m $1 \033[0m"
    echo ""
}

function ReportError() {
    if [ $? -ne 0 ];then
        echo -e "\033[31m $1 \033[0m"        
        exit 1
    fi
}

function MsgInput() {
    read -r -p "Proceed ? [Y/n] " input

    case $input in
        [yY][eE][sS]|[yY])
            echo -e "\033[34m ok go! \033[0m"
            ;;

        [nN][oO]|[nN])
            exit 0
            ;;

        *)
            echo "invalid input..."
            exit 1
            ;;
    esac
}

###############################################################################
# main func
###############################################################################

ShowInfo "This script helps install mysqld service on current local machine."
ShowInfo "Installation Starts ..."

yum install -y gcc gcc-c++ lrzsz autoconf automake libtool readline-devel git gdb cmake net-tools bzip2 wget zip unzip
yum install -y mysql mysql-devel libcurl libcurl-devel libevent libevent-devel openssl openssl-devel openssl-libs 

ReportError "failed to install basic dependencies"

mkdir -p .tmp/
cd .tmp/
HOME_PATH=`pwd`


###############################################################################
# install mysqld service
###############################################################################

ShowInfo "Installing mysql server 5.5 ..."

groupadd mysql
useradd -g mysql mysql

cp ../mysql-5.5.27.tar.gz .
tar -xvf mysql-5.5.27.tar.gz
cd mysql-5.5.27/
cmake .
ReportError "cmake failed to generate makefile!"
make -j4
ReportError "build failed!"
make install

cd /usr/local/mysql
chown -R root .
chown -R mysql .
chgrp -R mysql .

mkdir -p /data/mysql
chown -R mysql:mysql /data/mysql
./scripts/mysql_install_db --user=mysql --basedir=/usr/local/mysql --datadir=/data/mysql

mkdir -p /data/log/mariadb
cp -rf ${HOME_PATH}/../my.cnf /etc/my.cnf
cp support-files/my-medium.cnf /etc/mysql.cnf
cp support-files/mysql.server /etc/init.d/mysql
chmod +x /etc/init.d/mysql
chkconfig --add mysql
chkconfig mysql on

# start mysqld
service mysql start
ReportError "failed to start mysql server!"

echo ""
echo "mysql server intalled"
echo "pls create your own username/password"

###############################################################################
# final cleanup
###############################################################################
cd ../
rm -rf .tmp/

ShowInfo "All Done."


mysql -e "CREATE USER 'game'@'%' IDENTIFIED BY 'game123u'"
ReportError "failed to create user: game/game123u"

mysql -e "grant all privileges on *.* to game@'%' identified by 'game123u'"
ReportError "failed to grant all privileges"

mysql -e "delete from mysql.user where User = ''"
mysql -e "flush privileges"
ReportError "failed to flush privileges"

