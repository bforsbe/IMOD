package etomo.ui;

import java.awt.Component;
import java.awt.event.*;
import javax.swing.*;

import etomo.ApplicationManager;
import etomo.type.AxisID;
import etomo.comscript.BeadtrackParam;
import etomo.comscript.ConstBeadtrackParam;
import etomo.comscript.FortranInputSyntaxException;

/**
 * <p>Description: The dialog box for creating the fiducial model(s).</p>
 *
 * <p>Copyright: Copyright (c) 2002</p>
 *
 * <p>Organization: Boulder Laboratory for 3D Fine Structure,
 * University of Colorado</p>
 *
 * @author $Author$
 *
 * @version $Revision$
 *
 * <p> $Log$
 * <p> Revision 2.1  2003/04/28 23:25:25  rickg
 * <p> Changed visible imod references to 3dmod
 * <p>
 * <p> Revision 2.0  2003/01/24 20:30:31  rickg
 * <p> Single window merge to main branch
 * <p>
 * <p> Revision 1.7.2.1  2003/01/24 18:43:37  rickg
 * <p> Single window GUI layout initial revision
 * <p>
 * <p> Revision 1.7  2002/12/19 17:45:22  rickg
 * <p> Implemented advanced dialog state processing
 * <p> including:
 * <p> default advanced state set on start up
 * <p> advanced button management now handled by
 * <p> super class
 * <p>
 * <p> Revision 1.6  2002/12/19 06:02:57  rickg
 * <p> Implementing advanced parameters handling
 * <p>
 * <p> Revision 1.5  2002/12/19 00:30:05  rickg
 * <p> app manager and root pane moved to super class
 * <p>
 * <p> Revision 1.4  2002/11/14 21:18:37  rickg
 * <p> Added anchors into the tomoguide
 * <p>
 * <p> Revision 1.3  2002/10/17 22:39:42  rickg
 * <p> Added fileset name to window title
 * <p> this reference removed applicationManager messages
 * <p>
 * <p> Revision 1.2  2002/10/07 22:31:18  rickg
 * <p> removed unused imports
 * <p> reformat after emacs trashed it
 * <p>
 * <p> Revision 1.1  2002/09/09 22:57:02  rickg
 * <p> Initial CVS entry, basic functionality not including combining
 * <p> </p>
 */
public class FiducialModelDialog extends ProcessDialog implements ContextMenu {
  public static final String rcsid =
    "$Id$";

  JPanel panelFiducialModel = new JPanel();

  BeveledBorder border = new BeveledBorder("Fiducial Model Generation");

  JToggleButton buttonSeed =
    new JToggleButton("<html><b>Seed fiducial<br>model using 3dmod</b>");

  BeadtrackPanel panelBeadtrack;

  private JButton buttonTrack =
    new JButton("<html><b>Track fiducial<br>seed model</b>");

  JButton buttonFixModel =
    new JButton("<html><b>Fix fiducial model<br>using bead fixer</b>");

  public FiducialModelDialog(ApplicationManager appMgr, AxisID axisID) {
    super(appMgr, axisID);
    fixRootPanel(rootSize);

    panelBeadtrack = new BeadtrackPanel(axisID);

    buttonExecute.setText("Done");

    buttonSeed.setAlignmentX(Component.CENTER_ALIGNMENT);
    buttonSeed.setPreferredSize(FixedDim.button2Line);
    buttonSeed.setMaximumSize(FixedDim.button2Line);

    buttonTrack.setAlignmentX(Component.CENTER_ALIGNMENT);
    buttonTrack.setPreferredSize(FixedDim.button2Line);
    buttonTrack.setMaximumSize(FixedDim.button2Line);

    buttonFixModel.setAlignmentX(Component.CENTER_ALIGNMENT);
    buttonFixModel.setPreferredSize(FixedDim.button2Line);
    buttonFixModel.setMaximumSize(FixedDim.button2Line);

    panelFiducialModel.setLayout(
      new BoxLayout(panelFiducialModel, BoxLayout.Y_AXIS));
    panelFiducialModel.setBorder(border.getBorder());

    panelFiducialModel.add(buttonSeed);
    panelFiducialModel.add(Box.createRigidArea(FixedDim.x0_y5));

    panelFiducialModel.add(panelBeadtrack.getContainer());
    panelFiducialModel.add(Box.createRigidArea(FixedDim.x0_y5));

    panelFiducialModel.add(buttonTrack);
    panelFiducialModel.add(Box.createRigidArea(FixedDim.x0_y5));

    panelFiducialModel.add(buttonFixModel);

    rootPanel.setLayout(new BoxLayout(rootPanel, BoxLayout.Y_AXIS));
    rootPanel.add(panelFiducialModel);
    rootPanel.add(Box.createVerticalGlue());
    rootPanel.add(Box.createRigidArea(FixedDim.x0_y10));
    rootPanel.add(panelExitButtons);
    rootPanel.add(Box.createRigidArea(FixedDim.x0_y10));

    //
    //  Action listener assignments for the buttons
    //
    buttonSeed.addActionListener(new FiducialModelActionListener(this));
    buttonTrack.addActionListener(new FiducialModelActionListener(this));
    buttonFixModel.addActionListener(new FiducialModelActionListener(this));

    //  Mouse adapter for context menu
    GenericMouseAdapter mouseAdapter = new GenericMouseAdapter(this);
    panelFiducialModel.addMouseListener(mouseAdapter);

    //  Set the advanced state to the default
    updateAdvanced(isAdvanced);
  }

  /**
   * Set the advanced state for the dialog box
   */
  public void updateAdvanced(boolean state) {
    panelBeadtrack.setAdvanced(state);
    applicationManager.packMainWindow();
  }

  /**
   * Set the parameters for the specified beadtrack panel
   */
  public void setBeadtrackParams(ConstBeadtrackParam beadtrackParams) {
    panelBeadtrack.setParameters(beadtrackParams);
  }

  /**
   * Get the parameters for the specified beadtrack command
   */
  public void getBeadtrackParams(BeadtrackParam beadtrackParams)
    throws FortranInputSyntaxException {
    panelBeadtrack.getParameters(beadtrackParams);
  }

  /**
   * Right mouse button context menu
   */
  public void popUpContextMenu(MouseEvent mouseEvent) {
    String[] manPagelabel = { "beadtrack", "3dmod" };
    String[] manPage = { "beadtrack.html", "3dmod.html" };
    String[] logFileLabel = { "track" };
    String[] logFile = new String[1];
    logFile[0] = "track" + axisID.getExtension() + ".log";
    //    ContextPopup contextPopup =
    new ContextPopup(
      panelFiducialModel,
      mouseEvent,
      "TRACKING FIDUCIALS",
      manPagelabel,
      manPage,
      logFileLabel,
      logFile);
  }

  //  Action function for buttons
  void buttonAction(ActionEvent event) {
    String command = event.getActionCommand();

    if (command.equals(buttonSeed.getActionCommand())) {
      applicationManager.imodSeedFiducials(axisID);
    }

    else if (command.equals(buttonTrack.getActionCommand())) {
      applicationManager.fiducialModelTrack(axisID);
    }

    else if (command.equals(buttonFixModel.getActionCommand())) {
      applicationManager.imodFixFiducials(axisID);
    }
  }

  //  Action function overides for buttons
  public void buttonCancelAction(ActionEvent event) {
    super.buttonCancelAction(event);
    applicationManager.doneFiducialModelDialog(axisID);
  }

  public void buttonPostponeAction(ActionEvent event) {
    super.buttonPostponeAction(event);
    applicationManager.doneFiducialModelDialog(axisID);
  }

  public void buttonExecuteAction(ActionEvent event) {
    super.buttonExecuteAction(event);
    applicationManager.doneFiducialModelDialog(axisID);
  }

  public void buttonAdvancedAction(ActionEvent event) {
    super.buttonAdvancedAction(event);
    updateAdvanced(isAdvanced);
  }
}

//
//  Action listener adapters
//
class FiducialModelActionListener implements ActionListener {

  FiducialModelDialog adaptee;

  FiducialModelActionListener(FiducialModelDialog adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent event) {
    adaptee.buttonAction(event);
  }
}
