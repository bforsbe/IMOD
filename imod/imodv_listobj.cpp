/*
 *  imodv_listobj.cpp - Object list dialog with controls for grouping objects
 *
 *  Author: David Mastronarde   email: mast@colorado.edu
 *
 *  Copyright (C) 1995-2008 by Boulder Laboratory for 3-Dimensional Electron
 *  Microscopy of Cells ("BL3DEMC") and the Regents of the University of 
 *  Colorado.  See dist/COPYRIGHT for full copyright notice.
 *
 *  $Id$
 *  Log at end of file
 */

#include <qscrollview.h>
#include <qframe.h>
#include <qapplication.h>
#include <qcheckbox.h>
#include <qsignalmapper.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qvgroupbox.h>
#include "dia_qtutils.h"
#include "objgroup.h"

#include "imodv.h"
#include "imodv_gfx.h"
#include "imod.h"
#include "control.h"
#include "preferences.h"
#include "imodv_objed.h"
#include "imodv_listobj.h"
#include "imodv_input.h"

// The pointer to the instance
static ImodvOlist *Oolist_dialog = NULL;

#define MAX_OOLIST_BUTTONS  5000
#define MAX_OOLIST_WIDTH 384
#define MAX_LIST_IN_COL 36
#define MAX_LIST_NAME 40

enum {OBJGRP_NEW = 0, OBJGRP_DELETE, OBJGRP_CLEAR, OBJGRP_ADDALL, OBJGRP_SWAP,
      OBJGRP_TURNON, OBJGRP_TURNOFF, OBJGRP_OTHERSOFF};

// The button arrays, and variable to keep track of grouping
static QCheckBox **OolistButtons;
static QCheckBox **groupButtons;
static int numOolistButtons = 0;
static bool grouping = false;

/*
 * Create the object list dialog
 */ 
void imodvObjectListDialog(ImodvApp *a, int state)
{
  int m;
  QString qstr;
  char *window_name;

  if (!state){
    if (Oolist_dialog)
      Oolist_dialog->close();
    return;
  }
  if (Oolist_dialog){
    Oolist_dialog->raise();
    return;
  }
  grouping = false;

  // Get number of buttons, number of columns and number per column
  // Make maximum number of buttons needed for all loaded models
  for (m = 0; m < a->nm; m++)
    if (numOolistButtons < a->mod[m]->objsize) 
      numOolistButtons = a->mod[m]->objsize; 
  if (numOolistButtons > MAX_OOLIST_BUTTONS)
    numOolistButtons = MAX_OOLIST_BUTTONS;

  OolistButtons = (QCheckBox **)malloc(numOolistButtons * sizeof(QCheckBox *));
  groupButtons = (QCheckBox **)malloc(numOolistButtons * sizeof(QCheckBox *));
  if (!OolistButtons || !groupButtons) {
    if (OolistButtons)
      free(OolistButtons);
    if (groupButtons)
      free(groupButtons);
    numOolistButtons = 0;
    wprint("\aMemory error getting array for checkboxes\n");
    return;
  }
       
  Oolist_dialog = new ImodvOlist(imodvDialogManager.parent(IMODV_DIALOG));

  imodvOlistUpdateOnOffs(a);

  // Get sizes to adjust window size with
  QSize svSize = Oolist_dialog->mScroll->sizeHint();
  QSize frameSize = Oolist_dialog->mFrame->sizeHint();
  Oolist_dialog->adjustSize();

  // 4 pixels added was enough to prevent scroll bars
  // If width is constrained, allow more height for horizontal scroll bar
  int newWidth = Oolist_dialog->width() + frameSize.width() - svSize.width() +
    8;
  int newHeight = Oolist_dialog->height() + frameSize.height() - 
    svSize.height() + 8;
  if (newWidth > MAX_OOLIST_WIDTH) {
    newWidth = MAX_OOLIST_WIDTH;
    newHeight += 20;
  }
  if (newHeight > QApplication::desktop()->height() - 100)
    newHeight = QApplication::desktop()->height() - 100;
  Oolist_dialog->resize(newWidth, newHeight);

  window_name = imodwEithername("3dmodv Object List: ", a->imod->fileName, 1);
  if (window_name) {
    qstr = window_name;
    free(window_name);
  }
  if (qstr.isEmpty())
    qstr = "3dmodv Object List";
  Oolist_dialog->setCaption(qstr);
  imodvDialogManager.add((QWidget *)Oolist_dialog, IMODV_DIALOG);

  // After getting size with group buttons present, maybe hide them
  Oolist_dialog->updateGroups(a);
  Oolist_dialog->show();
}

// Set On/Off state for one object
void imodvOlistSetChecked(ImodvApp *a, int ob, bool state)
{
  if (!Oolist_dialog)
    return;
  if (ob < numOolistButtons && ob < a->imod->objsize)
    diaSetChecked(OolistButtons[ob], state);
}

// Set color for one object
void imodvOlistSetColor(ImodvApp *a, int ob)
{
  Iobj *obj;
  if (!Oolist_dialog || a->ob >= numOolistButtons)
    return;
  obj = &a->imod->obj[ob];
  OolistButtons[a->ob]->setPaletteBackgroundColor
    (QColor((int)(255 * obj->red), (int)(255 * obj->green),
            (int)(255 * obj->blue)));
}

// Update the group control buttons and buttons for objects
void imodvOlistUpdateGroups(ImodvApp *a)
{
  Oolist_dialog->updateGroups(a);
}

void imodvOlistUpdateOnOffs(ImodvApp *a)
{
  int ob;
  bool state;
  QString qstr;
  char obname[MAX_LIST_NAME];
  int len;
  QColor bkgColor;
  QColor gray;
  if (!Oolist_dialog || !numOolistButtons)
    return;

  gray = Oolist_dialog->paletteBackgroundColor();
  for (ob = 0; ob < numOolistButtons; ob++) {
    if (ob < a->imod->objsize) {
      // Get a truncated name
      // DMN 9/20/04: just truncate all columns a little bit now
      len = strlen(a->imod->obj[ob].name);
      if (len > MAX_LIST_NAME - 1)
        len = MAX_LIST_NAME - 1;
      strncpy(obname, a->imod->obj[ob].name, len);
      obname[len] = 0x00;
      qstr.sprintf("%d: %s",ob + 1, obname);
      OolistButtons[ob]->setText(qstr);
      state = !(a->imod->obj[ob].flags & IMOD_OBJFLAG_OFF);
      bkgColor.setRgb((int)(255. * a->imod->obj[ob].red),
                      (int)(255. * a->imod->obj[ob].green),
                      (int)(255. * a->imod->obj[ob].blue));
    } else {
      state = false;
      bkgColor = gray;
    }
    OolistButtons[ob]->setEnabled(ob < a->imod->objsize);
    OolistButtons[ob]->setPaletteBackgroundColor(bkgColor);
    diaSetChecked(OolistButtons[ob], state);
  }
}

/*
 * Return true if an object should be edited as part of the group
 */
bool imodvOlistObjInGroup(ImodvApp *a, int ob)
{
  if (!Oolist_dialog || !numOolistButtons || !grouping || 
      ob >= numOolistButtons) 
    return false;
  return groupButtons[ob]->isOn();
}

// Simply return flag for whether grouping is on
bool imodvOlistGrouping(void)
{
  return grouping;
}

/*
 * Object list class constructor
 */
ImodvOlist::ImodvOlist(QWidget *parent, const char *name, WFlags fl)
  : QWidget(parent, name, fl)
{
  int nPerCol, olistNcol, ob, i;
  QString qstr;
  char *labels[] = {"New", "Delete", "Clear", "Add All", "Swap", "ON", "OFF",
                    "Others Off"};
  char *tips[] = {"Start a new object group, copied from current group",
                  "Remove the current object group from list of groups",
                  "Remove all objects from the current group",
                  "Add all objects to the current group",
                  "Remove current members and add all non-members",
                  "Turn ON all objects in the current group",
                  "Turn OFF all objects in the current group",
                  "Turn OFF objects not in the current group"};

  QVBoxLayout *layout = new QVBoxLayout(this, 11, 6, "list layout");

  QVGroupBox *grpbox = new QVGroupBox("Object Group Selection", this);
  layout->addWidget(grpbox);
  QHBox *hbox = new QHBox(grpbox);
  new QLabel("Group", hbox);
  mGroupSpin = new QSpinBox(0, 0, 1, hbox);
  mGroupSpin->setSpecialValueText("None");
  mGroupSpin->setFocusPolicy(QWidget::ClickFocus);
  connect(mGroupSpin, SIGNAL(valueChanged(int)), this, 
          SLOT(curGroupChanged(int)));
  QToolTip::add(mGroupSpin, "Select the current object group or turn off "
                "selection");

  mNumberLabel = new QLabel("/99", hbox);
  mNameEdit = new QLineEdit(hbox);
  mNameEdit->setMaxLength(OBJGRP_STRSIZE - 1);
  mNameEdit->setFocusPolicy(QWidget::ClickFocus);
  connect(mNameEdit, SIGNAL(returnPressed()), this,
          SLOT(returnPressed()));
  connect(mNameEdit, SIGNAL(textChanged(const QString&)), this,
          SLOT(nameChanged(const QString&)));
  QToolTip::add(mNameEdit, "Enter a name for the current group");

  QSignalMapper *clickMapper = new QSignalMapper(this);
  connect(clickMapper, SIGNAL(mapped(int)), this,
          SLOT(actionButtonClicked(int)));

  // Make the buttons and put in hbox
  for (i = 0; i < OBJLIST_NUMBUTTONS; i++) {

    if (i == 0 || i == 5)
      hbox = new QHBox(grpbox);

    qstr = labels[i];
    mButtons[i] = new QPushButton(qstr, hbox);
    mButtons[i]->setFocusPolicy(QWidget::NoFocus);
    clickMapper->setMapping(mButtons[i], i);
    connect(mButtons[i], SIGNAL(clicked()), clickMapper, SLOT(map()));
    QToolTip::add(mButtons[i], tips[i]);
  }

  mScroll = new QScrollView(this);
  layout->addWidget(mScroll);
  mFrame = new QFrame(mScroll->viewport());
  mScroll->addChild(mFrame);
  mScroll->viewport()->setPaletteBackgroundColor
    (mFrame->paletteBackgroundColor());
  mGrid = new QGridLayout(mFrame, 1, 1, 0, 2, "list grid");

  olistNcol = (numOolistButtons + MAX_LIST_IN_COL - 1) / MAX_LIST_IN_COL;
  nPerCol = (numOolistButtons + olistNcol - 1) / olistNcol;

  // Get a signal mapper, connect to the slot for these buttons
  QSignalMapper *mapper = new QSignalMapper(this);
  connect(mapper, SIGNAL(mapped(int)), this, SLOT(toggleListSlot(int)));
  QSignalMapper *gmapper = new QSignalMapper(this);
  connect(gmapper, SIGNAL(mapped(int)), this, SLOT(toggleGroupSlot(int)));
  
  // Make the buttons, set properties and map them
  for (ob = 0; ob < numOolistButtons; ob++) {
    QHBoxLayout *hLayout = new QHBoxLayout();
    mGrid->addLayout(hLayout, ob % nPerCol, ob / nPerCol);
    groupButtons[ob] = diaCheckBox("", mFrame, hLayout);
    qstr.sprintf("%d: ",ob + 1);
    OolistButtons[ob] = diaCheckBox((char *)qstr.latin1(), mFrame, hLayout);
    mapper->setMapping(OolistButtons[ob], ob);
    connect(OolistButtons[ob], SIGNAL(toggled(bool)), mapper, SLOT(map()));
    gmapper->setMapping(groupButtons[ob], ob);
    connect(groupButtons[ob], SIGNAL(toggled(bool)), gmapper, SLOT(map()));
    hLayout->setStretchFactor(OolistButtons[ob], 100);

    // Hide the buttons later after window size is set
    grouping = true;
  }

  // Make a line
  QFrame *line = new QFrame(this);
  line->setFrameShape( QFrame::HLine );
  line->setFrameShadow( QFrame::Sunken );
  layout->addWidget(line);

  QHBox *box = new QHBox(this);
  layout->addWidget(box);
  mDoneButton = new QPushButton("Done", box);
  mDoneButton->setFocusPolicy(QWidget::NoFocus);
  connect(mDoneButton, SIGNAL(clicked()), this, SLOT(donePressed()));
  mHelpButton = new QPushButton("Help", box);
  mHelpButton->setFocusPolicy(QWidget::NoFocus);
  connect(mHelpButton, SIGNAL(clicked()), this, SLOT(helpPressed()));
  setFontDependentWidths();
}

void ImodvOlist::toggleGroupSlot(int ob)
{
  int index;
  bool state;
  IobjGroup *group = (IobjGroup *)ilistItem(Imodv->imod->groupList, 
                                            Imodv->imod->curObjGroup);
  if (!group)
    return;
  index = objGroupLookup(group, ob);
  state = groupButtons[ob]->isOn();
  if (state && index < 0)
    objGroupAppend(group, ob);
  else if (!state && index >= 0)
    ilistRemove(group->objList, index);
}
 
void ImodvOlist::toggleListSlot(int ob)
{
  objedToggleObj(ob, OolistButtons[ob]->isOn());
}

void ImodvOlist::donePressed()
{
  close();
}

void ImodvOlist::helpPressed()
{
  imodShowHelpPage("objectList.html");
}

void ImodvOlist::actionButtonClicked(int which)
{
  Imod *imod = Imodv->imod;
  IobjGroup *group, *ogroup;
  int ob, index, changed = 1;

  if (which != OBJGRP_NEW && which != OBJGRP_DELETE) {
    group = (IobjGroup *)ilistItem(imod->groupList, imod->curObjGroup);
    if (!group)
      return;
  }

  switch (which) {
  case OBJGRP_NEW:
    imodvRegisterModelChg();
    group = objGroupListExpand(&imod->groupList);
    if (!group)
      break;
    ogroup = (IobjGroup *)ilistItem(imod->groupList, imod->curObjGroup);
    if (ogroup)
      group->objList = ilistDup(ogroup->objList);
    imod->curObjGroup = ilistSize(imod->groupList) - 1;
    updateGroups(Imodv);
    break;

  case OBJGRP_DELETE:
    if (imod->curObjGroup < 0)
      return;
    imodvRegisterModelChg();
    if (objGroupListRemove(imod->groupList, imod->curObjGroup))
      break;
    imod->curObjGroup = B3DMIN(B3DMAX(0, imod->curObjGroup - 1),
                               ilistSize(imod->groupList) - 1);
    updateGroups(Imodv);
    break;

  case OBJGRP_CLEAR:
    imodvRegisterModelChg();
    ilistTruncate(group->objList, 0);
    updateGroups(Imodv);
    break;

  case OBJGRP_ADDALL:
    imodvRegisterModelChg();
    if (group->objList)
      ilistTruncate(group->objList, 0);
    for (ob = 0; ob < imod->objsize; ob++)
      if (objGroupAppend(group, ob))
        return;
    updateGroups(Imodv);
    break;

  case OBJGRP_SWAP:
    imodvRegisterModelChg();
    for (ob = 0; ob < imod->objsize; ob++) {
      index = objGroupLookup(group, ob);
      if (index >= 0)
        ilistRemove(group->objList, index);
      else
        if (ilistAppend(group->objList, &ob))
          return;
    }
    updateGroups(Imodv);
    break;

  case OBJGRP_TURNON:
  case OBJGRP_TURNOFF:
  case OBJGRP_OTHERSOFF:
    changed = 0;
    for (ob = 0; ob < imod->objsize; ob++) {
      index = objGroupLookup(group, ob);
      if (index >= 0 && which == OBJGRP_TURNON && 
          iobjOff(imod->obj[ob].flags)) {
        imodvRegisterObjectChg(ob);
        imod->obj[ob].flags &= ~IMOD_OBJFLAG_OFF;
        changed = 1;
      } else if (((index >= 0 && which == OBJGRP_TURNOFF) ||
                  (index < 0 && which == OBJGRP_OTHERSOFF)) &&
                 !iobjOff(imod->obj[ob].flags)) {
        imodvRegisterObjectChg(ob);
        imod->obj[ob].flags |= IMOD_OBJFLAG_OFF;
        changed = 1;
      }
    }
    imodvDraw(Imodv);
    imodvDrawImodImages();
    imodvObjedNewView();
    break;

  }
  if (changed)
    imodvFinishChgUnit();
}

void ImodvOlist::nameChanged(const QString &str)
{
  IobjGroup *group = (IobjGroup *)ilistItem(Imodv->imod->groupList, 
                                            Imodv->imod->curObjGroup);
  if (!group)
    return;
  strncpy(&group->name[0], str.latin1(), OBJGRP_STRSIZE);
  group->name[OBJGRP_STRSIZE - 1] = 0x00;
}

void ImodvOlist::curGroupChanged(int value)
{
  setFocus();
  Imodv->imod->curObjGroup = value - 1;
  updateGroups(Imodv);
}

void ImodvOlist::returnPressed()
{
  setFocus();
}

void ImodvOlist::updateGroups(ImodvApp *a)
{
  IobjGroup *group;
  int i, ob, numGroups = ilistSize(a->imod->groupList);
  int curGrp = a->imod->curObjGroup;
  int *objs;
  bool *states;
  QString str;
  diaSetSpinMMVal(mGroupSpin, 0, numGroups, curGrp + 1);
  str.sprintf("/%d", numGroups);
  mNumberLabel->setText(str);
  mNameEdit->setEnabled(curGrp >= 0);
  for (i = 1; i < OBJLIST_NUMBUTTONS; i++)
    mButtons[i]->setEnabled(curGrp >= 0);
  if (curGrp < 0) {
    if (grouping)
      for (ob = 0; ob < numOolistButtons; ob++)
        groupButtons[ob]->hide();
    grouping = false;
    return;
  }
  if (!grouping)
    for (ob = 0; ob < numOolistButtons; ob++)
      groupButtons[ob]->show();
  grouping = true;

  group = (IobjGroup *)ilistItem(a->imod->groupList, curGrp);
  if (!group)
    return;
  mNameEdit->setText(group->name);
  states = (bool *)malloc(sizeof(bool) * numOolistButtons);
  if (!states)
    return;
  for (ob = 0; ob < numOolistButtons; ob++)
    states[ob] = false;
  objs = (int *)ilistFirst(group->objList);
  if (objs) {
    for (i = 0; i < ilistSize(group->objList); i++)
      if (objs[i] >= 0 && objs[i] < numOolistButtons)
        states[objs[i]] = true;
  }
  for (ob = 0; ob < numOolistButtons; ob++)
    diaSetChecked(groupButtons[ob], states[ob]);

  free(states);
}


void ImodvOlist::setFontDependentWidths()
{
  int i, width, minWidth;
  bool rounded = ImodPrefs->getRoundedStyle();
  minWidth = diaGetButtonWidth(this, rounded, 1.3, mButtons[0]->text());
  for (i = 0; i < OBJLIST_NUMBUTTONS; i++) {
    width = diaGetButtonWidth(this, rounded, 1.3, mButtons[i]->text());
    width = B3DMAX(minWidth, width);
    mButtons[i]->setFixedWidth(width);
  }
  width = diaGetButtonWidth(this, rounded, 1.8, "Help");
  mHelpButton->setFixedWidth(width);
  mDoneButton->setFixedWidth(width);
}

void ImodvOlist::fontChange(const QFont &oldFont)
{
  setFontDependentWidths();
  QWidget::fontChange(oldFont);
}


void ImodvOlist::closeEvent ( QCloseEvent * e )
{
  imodvDialogManager.remove((QWidget *)Oolist_dialog);
  Oolist_dialog  = NULL;
  numOolistButtons = 0;
  free(OolistButtons);
  free(groupButtons);
  grouping = false;
  e->accept();
}

void ImodvOlist::keyPressEvent ( QKeyEvent * e )
{
  if (e->key() == Qt::Key_Escape)
    close();
  else
    imodvKeyPress(e);
}

void ImodvOlist::keyReleaseEvent ( QKeyEvent * e )
{
  imodvKeyRelease(e);
}

/*

$Log$
Revision 4.1  2008/01/21 17:48:54  mast
Split into new module, added provisional grouping capability


*/
