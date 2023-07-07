#include "ClientTabWidget.h"
#include <QLabel>
#include <QTabBar>
#include <QMenu>
#include "DesktopControlWidget.h"
#include <QDebug>

CilentTabWidget::CilentTabWidget(QWidget *parent) : QTabWidget(parent)
{
    QString tab_qss =  "QTabWidget::pane{} \
              QTabBar::tab {color:rgb(0,0,0);font-size:14px;min-width:110px;height:30px;margin:2px;} \
              QTabBar::tab:!selected {background:rgb(255,255,255);border:1px solid rgb(233,233,233);} \
              QTabBar::tab:hover{background:rgba(255,255,255,0.8);} \
              QTabBar::tab:selected  {background:rgb(219,228,241);}";

    setStyleSheet(tab_qss);
    QTabBar *tabBar = this->tabBar();
    tabBar->setTabsClosable(true);
    tabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
    tabBar->setMovable(true);
    tabBar->setContextMenuPolicy(Qt::CustomContextMenu);

//    connect(tabBar, &QTabBar::customContextMenuRequested, this, &CilentTabWidget::onHandleContextMenuRequested);
    connect(tabBar, &QTabBar::tabCloseRequested, this, &CilentTabWidget::onCloseTab);
    setDocumentMode(true);
//    setElideMode(Qt::ElideRight);

}


void CilentTabWidget::addDesktopTab(){

    DesktopControlWidget *desktop = new DesktopControlWidget(this);
    addTab(desktop,tr("首页"));
    setCurrentWidget(desktop);

//    connect(desktop,&PageIndex::notifyCreateCustomTask,this,[this](const QString &address){
//        createPageCustomTask(address);
//    });
}

void CilentTabWidget::onCloseTab(int index){
    QWidget *cw = widget(index);
    if("PageIndex"==QString(cw->metaObject()->className())){

    }else{
        removeTab(index);

        cw->deleteLater();
//        delete currentWidget;

    //    if (WebView *view = webView(index)) {
    //        bool hasFocus = view->hasFocus();
    //        removeTab(index);
    //        if (hasFocus && count() > 0)
    //            currentWebView()->setFocus();
    //        if (count() == 0)
    //            createTab();
    //        view->deleteLater();
    //    }


    }


}



