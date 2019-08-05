/********************************************************************************
** Form generated from reading UI file 'panel.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PANEL_H
#define UI_PANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_panel
{
public:
    QWidget *centralWidget;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QLabel *label;
    QScrollBar *horizontalScrollBar;
    QLineEdit *lineEdit;
    QScrollArea *scrollArea_2;
    QWidget *scrollAreaWidgetContents_2;
    QTreeView *treeView;
    QScrollBar *verticalScrollBar;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *panel)
    {
        if (panel->objectName().isEmpty())
            panel->setObjectName(QStringLiteral("panel"));
        panel->resize(400, 676);
        centralWidget = new QWidget(panel);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        scrollArea = new QScrollArea(centralWidget);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setGeometry(QRect(0, 0, 401, 41));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 399, 39));
        label = new QLabel(scrollAreaWidgetContents);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(0, 0, 111, 21));
        horizontalScrollBar = new QScrollBar(scrollAreaWidgetContents);
        horizontalScrollBar->setObjectName(QStringLiteral("horizontalScrollBar"));
        horizontalScrollBar->setGeometry(QRect(110, 20, 291, 16));
        horizontalScrollBar->setOrientation(Qt::Horizontal);
        lineEdit = new QLineEdit(scrollAreaWidgetContents);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(110, 0, 291, 21));
        scrollArea->setWidget(scrollAreaWidgetContents);
        scrollArea_2 = new QScrollArea(centralWidget);
        scrollArea_2->setObjectName(QStringLiteral("scrollArea_2"));
        scrollArea_2->setGeometry(QRect(0, 40, 411, 571));
        scrollArea_2->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QStringLiteral("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 409, 569));
        treeView = new QTreeView(scrollAreaWidgetContents_2);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setGeometry(QRect(0, 0, 381, 571));
        verticalScrollBar = new QScrollBar(scrollAreaWidgetContents_2);
        verticalScrollBar->setObjectName(QStringLiteral("verticalScrollBar"));
        verticalScrollBar->setGeometry(QRect(380, 0, 16, 571));
        verticalScrollBar->setOrientation(Qt::Vertical);
        scrollArea_2->setWidget(scrollAreaWidgetContents_2);
        panel->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(panel);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 22));
        panel->setMenuBar(menuBar);
        mainToolBar = new QToolBar(panel);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        panel->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(panel);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        panel->setStatusBar(statusBar);

        retranslateUi(panel);

        QMetaObject::connectSlotsByName(panel);
    } // setupUi

    void retranslateUi(QMainWindow *panel)
    {
        panel->setWindowTitle(QApplication::translate("panel", "panel", nullptr));
        label->setText(QApplication::translate("panel", "   Circuit Name: ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class panel: public Ui_panel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PANEL_H
