/***************************************************************************
 *   Copyright (c) 2019 WandererFan <wandererfan@gmail.com>                *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
# include <QMessageBox>
# include <iostream>
# include <string>
# include <sstream>
# include <cstdlib>
# include <exception>
#endif  //#ifndef _PreComp_

#include <QGraphicsView>

# include <App/DocumentObject.h>
# include <Gui/Action.h>
# include <Gui/Application.h>
# include <Gui/BitmapFactory.h>
# include <Gui/Command.h>
# include <Gui/Control.h>
# include <Gui/Document.h>
# include <Gui/Selection.h>
# include <Gui/MainWindow.h>
# include <Gui/FileDialog.h>
# include <Gui/ViewProvider.h>

# include <Mod/Part/App/PartFeature.h>

#include <Mod/TechDraw/App/DrawView.h>
#include <Mod/TechDraw/App/DrawViewPart.h>
#include <Mod/TechDraw/App/DrawViewAnnotation.h>
#include <Mod/TechDraw/App/DrawPage.h>
#include <Mod/TechDraw/App/DrawUtil.h>
#include <Mod/TechDraw/Gui/QGVPage.h>

#include "DrawGuiUtil.h"
#include "MDIViewPage.h"
#include "TaskLeaderLine.h"
#include "TaskRichAnno.h"
#include "TaskCosVertex.h"
#include "ViewProviderPage.h"

using namespace TechDrawGui;
using namespace std;


//internal functions
bool _checkSelectionHatch(Gui::Command* cmd);

//===========================================================================
// TechDraw_Leader
//===========================================================================

DEF_STD_CMD_A(CmdTechDrawLeaderLine);

CmdTechDrawLeaderLine::CmdTechDrawLeaderLine()
  : Command("TechDraw_LeaderLine")
{
    sAppModule      = "TechDraw";
    sGroup          = QT_TR_NOOP("TechDraw");
    sMenuText       = QT_TR_NOOP("Add a line to a view");
    sToolTipText    = QT_TR_NOOP("Add a line to a view");
    sWhatsThis      = "TechDraw_LeaderLine";
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/techdraw-mline";
}

void CmdTechDrawLeaderLine::activated(int iMsg)
{
    Q_UNUSED(iMsg);

    Gui::TaskView::TaskDialog *dlg = Gui::Control().activeDialog();
    if (dlg != nullptr) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Task In Progress"),
            QObject::tr("Close active task dialog and try again."));
        return;
    }

    TechDraw::DrawPage* page = DrawGuiUtil::findPage(this);
    if (!page) {
        return;
    }

    std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();
    TechDraw::DrawView* baseFeat = nullptr;
    if (!selection.empty()) {
        baseFeat =  dynamic_cast<TechDraw::DrawView *>(selection[0].getObject());
        if( baseFeat == nullptr ) {
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection Error"),
                                 QObject::tr("Can not attach leader.  No base View selected."));
            return;
        }
    } else {
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection Error"),
                                 QObject::tr("You must select a base View for the line."));
            return;
    }

    Gui::Control().showDialog(new TechDrawGui::TaskDlgLeaderLine(baseFeat,
                                                    page));
}

bool CmdTechDrawLeaderLine::isActive(void)
{
    bool havePage = DrawGuiUtil::needPage(this);
    bool haveView = DrawGuiUtil::needView(this, false);
    return (havePage && haveView);
}

//===========================================================================
// TechDraw_RichAnno
//===========================================================================

DEF_STD_CMD_A(CmdTechDrawRichAnno);

CmdTechDrawRichAnno::CmdTechDrawRichAnno()
  : Command("TechDraw_RichAnno")
{
    sAppModule      = "TechDraw";
    sGroup          = QT_TR_NOOP("TechDraw");
    sMenuText       = QT_TR_NOOP("Add a rich text annotation");
    sToolTipText    = QT_TR_NOOP("Add a rich text annotation");
    sWhatsThis      = "TechDraw_RichAnno";
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/techdraw-textleader";
}

void CmdTechDrawRichAnno::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    Gui::TaskView::TaskDialog *dlg = Gui::Control().activeDialog();
    if (dlg != nullptr) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Task In Progress"),
            QObject::tr("Close active task dialog and try again."));
        return;
    }

    TechDraw::DrawPage* page = DrawGuiUtil::findPage(this);
    if (!page) {
        return;
    }

    std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();
    TechDraw::DrawView* baseFeat = nullptr;
    if (!selection.empty()) {
        baseFeat =  dynamic_cast<TechDraw::DrawView *>(selection[0].getObject());
//        if( baseFeat == nullptr ) {
//            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection Error"),
//                                 QObject::tr("Can not attach leader.  No base View selected."));
//            return;
//        }
//    } else {
//            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Selection Error"),
//                                 QObject::tr("You must select a base View for the line."));
//            return;
    }

    Gui::Control().showDialog(new TaskDlgRichAnno(baseFeat,
                                                  page));
}

bool CmdTechDrawRichAnno::isActive(void)
{
    bool havePage = DrawGuiUtil::needPage(this);
    bool haveView = DrawGuiUtil::needView(this, false);
    return (havePage && haveView);
}

//===========================================================================
// TechDraw_CosmeticVertex
//===========================================================================

DEF_STD_CMD_A(CmdTechDrawCosmeticVertex);

CmdTechDrawCosmeticVertex::CmdTechDrawCosmeticVertex()
  : Command("TechDraw_CosmeticVertex")
{
    sAppModule      = "TechDraw";
    sGroup          = QT_TR_NOOP("TechDraw");
    sMenuText       = QT_TR_NOOP("Add a cosmetic vertex");
    sToolTipText    = QT_TR_NOOP("Add a cosmetic vertex");
    sWhatsThis      = "TechDraw_CosmeticVertex";
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/techdraw-point";
}

void CmdTechDrawCosmeticVertex::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    Gui::TaskView::TaskDialog *dlg = Gui::Control().activeDialog();
    if (dlg != nullptr) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Task In Progress"),
            QObject::tr("Close active task dialog and try again."));
        return;
    }

    TechDraw::DrawPage* page = DrawGuiUtil::findPage(this);
    if (!page) {
        return;
    }

    std::vector<App::DocumentObject*> shapes = getSelection().
                                       getObjectsOfType(TechDraw::DrawViewPart::getClassTypeId());
    if (shapes.empty()) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("No DrawViewPart objects in this selection"));
        return;
    }

    TechDraw::DrawViewPart* baseFeat = nullptr;
    baseFeat =  dynamic_cast<TechDraw::DrawViewPart*>((*shapes.begin()));
    if (baseFeat == nullptr) {
        Base::Console().Message("CMD::CosmeticVertex - 1st shape is not DVP.  WTF?\n");
        return;
    }

    Gui::Control().showDialog(new TaskDlgCosVertex(baseFeat,
                                                   page));
}

bool CmdTechDrawCosmeticVertex::isActive(void)
{
    bool havePage = DrawGuiUtil::needPage(this);
    bool haveView = DrawGuiUtil::needView(this, false);
    return (havePage && haveView);
}

//===========================================================================
// TechDraw_Midpoints
//===========================================================================

DEF_STD_CMD_A(CmdTechDrawMidpoints);

CmdTechDrawMidpoints::CmdTechDrawMidpoints()
  : Command("TechDraw_Midpoints")
{
    sAppModule      = "TechDraw";
    sGroup          = QT_TR_NOOP("TechDraw");
    sMenuText       = QT_TR_NOOP("Add midpoint vertices");
    sToolTipText    = QT_TR_NOOP("Add midpoint vertices");
    sWhatsThis      = "TechDraw_Midpoints";
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/techdraw-midpoint";
}

void CmdTechDrawMidpoints::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    Gui::TaskView::TaskDialog *dlg = Gui::Control().activeDialog();
    if (dlg != nullptr) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Task In Progress"),
            QObject::tr("Close active task dialog and try again."));
        return;
    }

    TechDraw::DrawPage* page = DrawGuiUtil::findPage(this);
    if (!page) {
        return;
    }

    std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();
//    TechDraw::DrawView* baseFeat = nullptr;
//    if (!selection.empty()) {
//        baseFeat =  dynamic_cast<TechDraw::DrawView *>(selection[0].getObject());
//    }

//    Gui::Control().showDialog(new TaskDlgMidpoints(baseFeat,
//                                                        page));
    Base::Console().Message("CMD::Midpoints - start dialog here!\n");
}

bool CmdTechDrawMidpoints::isActive(void)
{
    bool havePage = DrawGuiUtil::needPage(this);
    bool haveView = DrawGuiUtil::needView(this, false);
    return (havePage && haveView);
}

//===========================================================================
// TechDraw_Quadrant
//===========================================================================

DEF_STD_CMD_A(CmdTechDrawQuadrant);

CmdTechDrawQuadrant::CmdTechDrawQuadrant()
  : Command("TechDraw_Quadrant")
{
    sAppModule      = "TechDraw";
    sGroup          = QT_TR_NOOP("TechDraw");
    sMenuText       = QT_TR_NOOP("Add quadrant vertices");
    sToolTipText    = QT_TR_NOOP("Add quadrant vertices");
    sWhatsThis      = "TechDraw_Quadrant";
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/techdraw-quadrant";
}

void CmdTechDrawQuadrant::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    Gui::TaskView::TaskDialog *dlg = Gui::Control().activeDialog();
    if (dlg != nullptr) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Task In Progress"),
            QObject::tr("Close active task dialog and try again."));
        return;
    }

    TechDraw::DrawPage* page = DrawGuiUtil::findPage(this);
    if (!page) {
        return;
    }

    std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();
//    TechDraw::DrawView* baseFeat = nullptr;
//    if (!selection.empty()) {
//        baseFeat =  dynamic_cast<TechDraw::DrawView *>(selection[0].getObject());
//    }

//    Gui::Control().showDialog(new TaskDlgQuadrant(baseFeat,
//                                                        page));
    Base::Console().Message("CMD::Quadrant - start dialog here!\n");
}

bool CmdTechDrawQuadrant::isActive(void)
{
    bool havePage = DrawGuiUtil::needPage(this);
    bool haveView = DrawGuiUtil::needView(this, false);
    return (havePage && haveView);
}

//===========================================================================
// TechDraw_Annotation
//===========================================================================

DEF_STD_CMD_A(CmdTechDrawAnnotation);

CmdTechDrawAnnotation::CmdTechDrawAnnotation()
  : Command("TechDraw_Annotation")
{
    // setting the Gui eye-candy
    sGroup        = QT_TR_NOOP("TechDraw");
    sMenuText     = QT_TR_NOOP("Insert Annotation");
    sToolTipText  = QT_TR_NOOP("Insert Annotation");
    sWhatsThis    = "TechDraw_NewAnnotation";
    sStatusTip    = sToolTipText;
    sPixmap       = "actions/techdraw-annotation";
}

void CmdTechDrawAnnotation::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    TechDraw::DrawPage* page = DrawGuiUtil::findPage(this);
    if (!page) {
        return;
    }
    std::string PageName = page->getNameInDocument();

    std::string FeatName = getUniqueObjectName("Annotation");
    openCommand("Create Annotation");
    doCommand(Doc,"App.activeDocument().addObject('TechDraw::DrawViewAnnotation','%s')",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.addView(App.activeDocument().%s)",PageName.c_str(),FeatName.c_str());
    updateActive();
    commitCommand();
}

bool CmdTechDrawAnnotation::isActive(void)
{
    return DrawGuiUtil::needPage(this);
}

void CreateTechDrawCommandsAnnotate(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdTechDrawLeaderLine());
    rcCmdMgr.addCommand(new CmdTechDrawRichAnno());
    rcCmdMgr.addCommand(new CmdTechDrawCosmeticVertex());
    rcCmdMgr.addCommand(new CmdTechDrawMidpoints());
    rcCmdMgr.addCommand(new CmdTechDrawQuadrant());
    rcCmdMgr.addCommand(new CmdTechDrawAnnotation());
}

//===========================================================================
// Selection Validation Helpers
//===========================================================================


