/***************************************************************************
 *   Copyright (C) 2008 by Heiko Koehn   *
 *   KoehnHeiko@googlemail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "TTreeWidget.h"
#include <QtGui>
#include "Host.h"
#include "HostManager.h"

TTreeWidget::TTreeWidget( QWidget * pW ) : QTreeWidget( pW )
{
    setSelectionMode( QAbstractItemView::SingleSelection );
    setDragEnabled( true );
    setAcceptDrops( true );
    setDropIndicatorShown( true );
    viewport()->setAcceptDrops( true );
    setDragDropMode( QAbstractItemView::InternalMove );
    mIsDropAction = false;
    mpHost = 0;
    mOldParentID = 0;
    
    mIsTriggerTree = false;
    mIsScriptTree = false;
    mIsTimerTree = false;
    mIsAliasTree = false;
    mIsActionTree = false;
    mIsKeyTree = false;
}

void TTreeWidget::setIsAliasTree()
{
    mIsAliasTree = true;
    mIsTriggerTree = false;
    mIsScriptTree = false;
    mIsTimerTree = false;
    mIsActionTree = false;
    mIsKeyTree = false;
}

void TTreeWidget::setIsTriggerTree()
{
    mIsTriggerTree = true;
    mIsAliasTree = false;
    mIsScriptTree = false;
    mIsTimerTree = false;
    mIsActionTree = false;
    mIsKeyTree = false;
}

void TTreeWidget::setIsActionTree()
{
    mIsTriggerTree = false;
    mIsAliasTree = false;
    mIsScriptTree = false;
    mIsTimerTree = false;
    mIsKeyTree = false;
    mIsActionTree = true;
}

void TTreeWidget::setIsKeyTree()
{
    mIsTriggerTree = false;
    mIsAliasTree = false;
    mIsScriptTree = false;
    mIsTimerTree = false;
    mIsActionTree = false;
    mIsKeyTree = true;
}

void TTreeWidget::setIsTimerTree()
{
    mIsTimerTree = true;
    mIsTriggerTree = false;
    mIsScriptTree = false;
    mIsAliasTree = false;
    mIsActionTree = false;
    mIsKeyTree = false;
}

void TTreeWidget::setIsScriptTree()
{
    mIsScriptTree = true;
    mIsTriggerTree = false;
    mIsAliasTree = false;
    mIsTimerTree = false;
    mIsActionTree = false;
    mIsKeyTree = false;
}

void TTreeWidget::setHost( Host * pH )
{
    mpHost = pH;    
}

void TTreeWidget::rowsAboutToBeRemoved( const QModelIndex & parent, int start, int end )
{
    mOldParentID = parent.data( Qt::UserRole ).toInt();
    if( ! mOldParentID )
    {
        mOldParentID = parent.sibling( start, 0 ).data( Qt::UserRole ).toInt();
    }
        
    QModelIndex child = parent.child( start, 0 );
    mChildID = child.data( Qt::UserRole ).toInt();
    if( mChildID == 0 )
    {
        if( parent.isValid() )
        child = parent.model()->index( start, 0, QModelIndex() );
        if( child.isValid() )        
        mChildID = child.data( Qt::UserRole ).toInt();
    }
    QTreeWidget::rowsAboutToBeRemoved( parent, start, end );
}


void TTreeWidget::rowsInserted( const QModelIndex & parent, int start, int end )
{
    if( mIsDropAction )
    {
        QModelIndex child = parent.child( start, 0 );
        if( mChildID == 0 )
        {
            mChildID = parent.model()->index( start, 0 ).data( Qt::UserRole ).toInt();
        }
        int newParentID = parent.data( Qt::UserRole ).toInt();
        
        if( mIsTriggerTree ) mpHost->getTriggerUnit()->reParentTrigger( mChildID, mOldParentID, newParentID );
        if( mIsAliasTree ) mpHost->getAliasUnit()->reParentAlias( mChildID, mOldParentID, newParentID );
        if( mIsTimerTree ) mpHost->getTimerUnit()->reParentTimer( mChildID, mOldParentID, newParentID );
        if( mIsScriptTree ) mpHost->getScriptUnit()->reParentScript( mChildID, mOldParentID, newParentID );
        
        mChildID = 0;
        mOldParentID = 0;
        mIsDropAction = false;
    }
    QTreeWidget::rowsInserted( parent, start, end );
}

Qt::DropActions TTreeWidget::supportedDropActions() const
{
    return Qt::MoveAction;
}


void TTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    mIsDropAction = true;
    QTreeWidget::dragEnterEvent( event );
}

void TTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QTreeWidget::dragMoveEvent( event );
}

void TTreeWidget::setPoint(QWidget * p)
{
    point = p;
}

void TTreeWidget::mousePressEvent ( QMouseEvent * event )
{
    //qDebug() << ((dlgTriggerEditor*)point)->CurrentView();
    //qDebug() << ((dlgTriggerEditor*)parentWidget())->CurrentView();
    //qDebug() << ((dlgTriggerEditor*)parent())->CurrentView();

    if (itemAt( event->pos() )) {
        QTreeWidgetItem * pItem = itemAt( event->pos() );

        if (mIsTriggerTree) {
            if (((dlgTriggerEditor*)point)->slot_saveTriggerAfterEdit())
                QTreeWidget::mousePressEvent( event );
            else
                event->ignore();
        } else if (mIsAliasTree) {
            if (((dlgTriggerEditor*)point)->slot_saveAliasAfterEdit())
                QTreeWidget::mousePressEvent( event );
            else
                event->ignore();
        } else if (mIsScriptTree) {
            if (((dlgTriggerEditor*)point)->slot_saveScriptAfterEdit())
                QTreeWidget::mousePressEvent( event );
            else
                event->ignore();
        } else if (mIsTimerTree) {
            if (((dlgTriggerEditor*)point)->slot_saveTimerAfterEdit())
                QTreeWidget::mousePressEvent( event );
            else
                event->ignore();
        } else if (mIsKeyTree) {
            if (((dlgTriggerEditor*)point)->slot_saveKeyAfterEdit())
                QTreeWidget::mousePressEvent( event );
            else
                event->ignore();
        } else if (mIsActionTree) {
            if (((dlgTriggerEditor*)point)->slot_saveActionAfterEdit())
                QTreeWidget::mousePressEvent( event );
            else
                event->ignore();
        } else {
            QTreeWidget::mousePressEvent( event );
        }
    }
}

void TTreeWidget::dropEvent(QDropEvent *event)
{
    qDebug()<<"dropEvent()";

    if (!itemAt( event->pos() )) {
        dragged_item->parent()->removeChild(dragged_item);
        topLevelItem(0)->addChild(dragged_item);
        setCurrentItem(dragged_item);
        event->ignore();
        return;    
    }

    QTreeWidgetItem * pItem = itemAt( event->pos() );
    if( pItem && pItem->parent() )
    {
        bool test_is_folder = false;
        int ID = pItem->data(0,Qt::UserRole).toInt();

        if( mIsTriggerTree )
        {
            if (!mpHost->getTriggerUnit()->getTrigger(ID)->isFolder())
                mpHost->getTriggerUnit()->getTrigger(ID)->setIsFolder(true);
        }
        if( mIsAliasTree )
        {
            if (!mpHost->getAliasUnit()->getAlias(ID)->isFolder())
                mpHost->getAliasUnit()->getAlias(ID)->setIsFolder(true);
        }
        if( mIsTimerTree )
        {
            if (!mpHost->getTimerUnit()->getTimer(ID)->isFolder())
                mpHost->getTimerUnit()->getTimer(ID)->setIsFolder(true);
        }
        if( mIsKeyTree )
        {
            if (!mpHost->getKeyUnit()->getKey(ID)->isFolder())
                mpHost->getKeyUnit()->getKey(ID)->setIsFolder(true);
        }
        if( mIsActionTree )
        {
            if (!mpHost->getActionUnit()->getAction(ID)->isFolder())
                mpHost->getActionUnit()->getAction(ID)->setIsFolder(true);
        }
        if( mIsScriptTree )
        {
            if (!mpHost->getScriptUnit()->getScript(ID)->isFolder())
                mpHost->getScriptUnit()->getScript(ID)->setIsFolder(true);
        }
    }

    //event->accept();
    mIsDropAction = true;
    QTreeWidget::dropEvent( event );
    expandItem(pItem);
    setCurrentItem(dragged_item);
    return;
}

void TTreeWidget::startDrag( Qt::DropActions supportedActions )
{
    dragged_item = currentItem();
    
    QTreeWidget::startDrag( supportedActions );
}

bool TTreeWidget::dropMimeData ( QTreeWidgetItem * parent, int index, const QMimeData * data, Qt::DropAction action )
{
    QTreeWidget::dropMimeData( parent, index, data, action );
}

