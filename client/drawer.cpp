#include "drawer.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <iostream>

Drawer::Drawer(QWidget *parent, Qt::WindowFlags f)
    :QToolBox(parent,f)
{
    this->setWindowFlags(Qt::FramelessWindowHint);//Remove Edges
    setWindowTitle(tr("Chat Room"));
    setWindowIcon(QPixmap(":/images/ChatRoom.jpg"));

    initialMapstylestring();
    tabWidget=new QTabWidget(this);
    QWidget *sysWidget=new QWidget;
    QVBoxLayout *sysLayout=new QVBoxLayout(sysWidget);
    addFriendsBtn = new QPushButton(tr("Add New Friend"));
    connect(addFriendsBtn, SIGNAL(clicked()), this, SLOT(slotAddFriends()));
    QPushButton *EnterChatRoom=new QPushButton(tr("Chat Room"));
    connect(EnterChatRoom, SIGNAL(clicked()), this, SLOT(showChatWidget1()));
    QPushButton *QuitButton=new QPushButton(tr("Exit Client"));
    connect(QuitButton, SIGNAL(clicked()), this, SLOT(Quit()));
    SelectMapStytle=new QComboBox(this);
    SelectMapStytle->addItem(tr("Change Avatar"));
    SelectMapStytle->addItem(tr("Boss Wizard"));
    SelectMapStytle->addItem(tr("Boss Lang"));
     SelectMapStytle->addItem(tr("Boss Zalinger"));
     SelectMapStytle->addItem(tr("Boss Zhao"));
     SelectMapStytle->addItem(tr("Bob"));
     SelectMapStytle->addItem(tr("BOSS"));
      SelectMapStytle->addItem(tr("Jim"));
      SelectMapStytle->addItem(tr("Jim 2"));
      SelectMapStytle->addItem(tr("Bob 2"));
      SelectMapStytle->addItem(tr("Boss Wizard 2"));
     SelectMapStytle->setCurrentIndex(0);
     connect(SelectMapStytle,SIGNAL(currentIndexChanged(int)),this,SLOT(MapStytleSet()));
    sysLayout->setMargin(20);
    sysLayout->setAlignment(Qt::AlignTop);//aligned to top
    sysLayout->addWidget(addFriendsBtn);
    sysLayout->addWidget(EnterChatRoom);
    sysLayout->addWidget(SelectMapStytle);
    sysLayout->addWidget(QuitButton);
    sysWidget->setLayout(sysLayout);

    myFriends=new QWidget;
    friLayout=new QVBoxLayout(myFriends);
    myFriends->setLayout(friLayout);
    friLayout->setAlignment(Qt::AlignTop);//aligned to top

    tabWidget->addTab(sysWidget,"System Functions");
    tabWidget->addTab(myFriends,"My friends");

    toolBtn1 = new QToolButton(this);
    toolBtn1->setIcon(QPixmap(":/images/qq.png"));
    toolBtn1->setIconSize(QPixmap(":/images/qq.png").size());
    toolBtn1->setAutoRaise(true);
    toolBtn1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //toolBtn1->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //connect(toolBtn1, SIGNAL(clicked()), this, SLOT(showChatWidget1()));

    toolBtn2 = new QToolButton(this);
    toolBtn2->setIcon(QPixmap(":/images/1.png"));
    //toolBtn2->setIconSize(QPixmap(":/images/1.png").size());
    toolBtn2->setAutoRaise(true);
    toolBtn2->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBtn2->setText("new message");
    toolBtn2->hide();
    connect(toolBtn2, SIGNAL(clicked()), this, SLOT(showaddfriendWidget()));

    QGroupBox *groupBox = new QGroupBox;
    QVBoxLayout *layout = new QVBoxLayout(groupBox);
    layout->setMargin(0);
    layout->setAlignment(Qt::AlignLeft);
    layout->addWidget(toolBtn1);
    layout->addWidget(tabWidget);
    layout->addWidget(toolBtn2);
    //layout->addStretch();

    this->addItem((QWidget*)groupBox, tr("Client"));
    addflag=false;
    refuseflag=false;
    changefunc=false;
    PicChangeflag=false;

    getfdlTimer=new QTimer(this);
    connect(getfdlTimer,SIGNAL(timeout()),this,SLOT(getFDL()));
    getfdlTimer->start(10);

    havedoneTimer=new QTimer(this);
    connect(havedoneTimer,SIGNAL(timeout()),this,SLOT(CheckFlag()));
    havedoneTimer->start(100);


    onlineStateTimer=new QTimer(this);
    connect(onlineStateTimer,SIGNAL(timeout()),this,SLOT(CheckonlineState()));

    users=new userInfo[20];

}

void Drawer::initialMapstylestring()
{
    MapStytle[0]=":/images/qq.png";
    MapStytle[1]=":/images/w.png";
    MapStytle[2]=":/images/l.png";
    MapStytle[3]=":/images/zz.png";
   MapStytle[4]=":/images/z.png";
   MapStytle[5]=":/images/m.png";
   MapStytle[6]=":/images/boss.png";
   MapStytle[7]=":/images/jj.png";
   MapStytle[8]=":/images/jj2.png";
  MapStytle[9]=":/images/m2.png";
  MapStytle[10]=":/images/w2.png";
}
void Drawer::initialfriendsList()
{
    //qDebug()<<"initial List";
    int i;
    if(!friendList.isEmpty())
    {
        friendsListButtons=new QToolButton *[friendList.size()+10];
        for(i=0;i<(friendList.size()+10);i++)
        {
            //qDebug()<<"if i:"<<i;
            friendsListButtons[i]=new QToolButton;
        }
    }
    friendsNum=0;
    while(!friendList.isEmpty())
    {
        QString friendName=friendList.dequeue();
        //friendName+="[Offline]";
        friendsListButtons[friendsNum]->setText(friendName);
        friendsListButtons[friendsNum]->setIcon(QPixmap(":/images/qq.png"));
        friendsListButtons[friendsNum]->setIconSize(QPixmap(":/images/qq.png").size());
        friendsListButtons[friendsNum]->setAutoRaise(true);
        friendsListButtons[friendsNum]->setEnabled(false);
        friendsListButtons[friendsNum]->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        friLayout->addWidget(friendsListButtons[friendsNum]);
        friendsNum++;
    }
    checkOnlineState=new onlineState;
    onlineStateTimer->start(1100);
}
void Drawer::CheckonlineState()
{
    if(checkOnlineState->StateUpdateDone)
    {
        users=checkOnlineState->users;
        int i,j;
        //qDebug()<<"--------------------friendsNum-----------------------"<<friendsNum;
        for(i=0;i<friendsNum;i++)
        {
           QString friendName=friendsListButtons[i]->text();
           //qDebug()<<"---friendName---"<<friendName;
           for(j=0;j<checkOnlineState->count;j++)
           {
               if(checkOnlineState->users[j].name==friendName.toStdString())
               {
                   if(checkOnlineState->users[j].online==1)
                   {
                       //friendsListButtons[i]->setText(friendName);
                       friendsListButtons[i]->setEnabled(true);
                   }
                   else
                   {
                       friendsListButtons[i]->setEnabled(false);
                   }
                   friendsListButtons[i]->setIcon(QPixmap(MapStytle[checkOnlineState->users[j].pic]));
                   friendsListButtons[i]->setIconSize(QPixmap(MapStytle[checkOnlineState->users[j].pic]).size());
               }
               if(checkOnlineState->users[j].name==username.toStdString())
               {
                   //qDebug()<<"---Name---"<<username;
                   toolBtn1->setIcon(QPixmap(MapStytle[checkOnlineState->users[j].pic]));
                   toolBtn1->setIconSize(QPixmap(MapStytle[checkOnlineState->users[j].pic]).size());
               }

           }

        }
        checkOnlineState->StateUpdateDone=false;
    }
    //onlineStateTimer->stop();
}

void Drawer::MapStytleSet()
{
    toolBtn1->setIcon(QPixmap(MapStytle[SelectMapStytle->currentIndex()]));
    toolBtn1->setIconSize(QPixmap(MapStytle[SelectMapStytle->currentIndex()]).size());
    PicChangeflag=true;
}

void Drawer::showChatWidget1()
{
    chat1 = new TcpClient();
    chat1->username = toolBtn1->text();
    chat1->setWindowTitle(toolBtn1->text());
    chat1->show();
}
void Drawer::getFDL()
{
    getFriendsList=new friendsList;
    getFriendsList->username=username;
    getFriendsList->sendRQ();
    getfdlTimer->stop();
}
void Drawer::CheckFlag()
{
    if(getFriendsList->havedoneFlag)
    {
        friendList=getFriendsList->friendList;
        initialfriendsList();
        havedoneTimer->stop();
        //getFriendsList->havedoneFlag=false;
    }
     //qDebug()<<"checking..."<<getFriendsList->havedoneFlag;
}
void Drawer::slotAddFriends()
{
    addFriends = new AddFriends();
    addFriends->username=username;
    addFriends->show();

}
void Drawer::showaddfriendWidget()
{
    if(!changefunc)
    {
        toolBtn2->hide();
        QString msg;
        msg+=fromname+" want to add you as a friend,do you agree?";
        QMessageBox message(QMessageBox::Information,"addfriend request...",msg,QMessageBox::Yes|QMessageBox::No,this);
        if(message.exec()==QMessageBox::Yes)
        {
            addflag=true;
            int currNum=friendsNum;
            //qDebug()<<"fromname  "<<fromname;
            friendsListButtons[currNum]->setText(fromname);
            friendsListButtons[currNum]->setIcon(QPixmap(":/images/qq.png"));
            friendsListButtons[currNum]->setIconSize(QPixmap(":/images/qq.png").size());
            friendsListButtons[currNum]->setAutoRaise(true);
            friendsListButtons[currNum]->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            friLayout->addWidget(friendsListButtons[currNum]);
            friendsNum++;
        }
        else
        {
            refuseflag=true;
        }
    }
    else
    {
        toolBtn2->hide();
        QMessageBox::information(this, tr("the message from server..."), tr("your requet had been refused..."));
    }
}
//Mouse Click and Moving events
void Drawer::mousePressEvent(QMouseEvent *event)
{
    this->windowPos = this->pos();                // Get current position
    this->mousePos = event->globalPos();     // Get mouse position
    this->dPos = mousePos - windowPos;       // position af
}

void Drawer::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - this->dPos);
}

void Drawer::Quit()
{
    QMessageBox message(QMessageBox::Information,"Exit Client ","Are you Sure？",QMessageBox::Yes|QMessageBox::No,this);
    message.setButtonText(QMessageBox::Yes, QString("SUre"));
    message.setButtonText(QMessageBox::No, QString("Back"));
    if(message.exec()==QMessageBox::Yes)
    {
        qApp->quit();
    {
        return;
    }
   }
}
