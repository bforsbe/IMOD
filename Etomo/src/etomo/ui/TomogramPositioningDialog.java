/**
 * <p>Description: Tomogram Positioning User Interface</p>
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
 * <p> Revision 3.7  2004/04/29 20:25:57  sueh
 * <p> bug# 427 added commented out testing code
 * <p>
 * <p> Revision 3.6  2004/04/27 01:02:46  sueh
 * <p> bugt# 427 added set and get TomopitchParam
 * <p>
 * <p> Revision 3.5  2004/04/26 18:32:05  rickg
 * <p> bug #426 Added full image code
 * <p>
 * <p> Revision 3.4  2004/04/26 00:20:21  rickg
 * <p> bug #426 Implemented full tomogram sampling
 * <p>
 * <p> Revision 3.3  2004/04/22 23:25:05  rickg
 * <p> bug #391 adding non-fid alignement capability
 * <p>
 * <p> Revision 3.2  2004/03/15 20:33:55  rickg
 * <p> button variable name changes to btn...
 * <p>
 * <p> Revision 3.1  2004/01/30 22:45:28  sueh
 * <p> bug# 356 Changing buttons with html labels to
 * <p> MultiLineButton and MultiLineToggleButton
 * <p>
 * <p> Revision 3.0  2003/11/07 23:19:01  rickg
 * <p> Version 1.0.0
 * <p>
 * <p> Revision 2.10  2003/10/30 01:43:44  rickg
 * <p> Bug# 338 Remapped context menu entries
 * <p>
 * <p> Revision 2.9  2003/10/29 00:28:32  rickg
 * <p> Bug# 297 Tooltips
 * <p>
 * <p> Revision 2.8  2003/10/28 23:35:48  rickg
 * <p> Bug# 336 Context menu label capitalization
 * <p>
 * <p> Revision 2.7  2003/10/28 00:23:47  rickg
 * <p> Bug# 336 Context menu label capitalization
 * <p>
 * <p> Revision 2.6  2003/10/27 23:58:18  rickg
 * <p> Bug# 284 tooltips
 * <p>
 * <p> Revision 2.5  2003/10/14 23:15:24  rickg
 * <p> Bug# 282 Label fixes
 * <p>
 * <p> Revision 2.4  2003/06/05 21:07:12  rickg
 * <p> Label change to match log file
 * <p>
 * <p> Revision 2.3  2003/05/23 22:13:47  rickg
 * <p> Removed any extensions from log file labels in context menu
 * <p>
 * <p> Revision 2.2  2003/04/28 23:25:25  rickg
 * <p> Changed visible imod references to 3dmod
 * <p>
 * <p> Revision 2.1  2003/03/02 23:30:41  rickg
 * <p> Combine layout in progress
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
 * <p> Revision 1.6  2002/12/19 00:30:26  rickg
 * <p> app manager and root pane moved to super class
 * <p>
 * <p> Revision 1.5  2002/11/14 21:18:37  rickg
 * <p> Added anchors into the tomoguide
 * <p>
 * <p> Revision 1.4  2002/10/17 22:40:29  rickg
 * <p> Added fileset name to window title
 * <p> this reference removed applicationManager messages
 * <p>
 * <p> Revision 1.3  2002/10/07 22:31:18  rickg
 * <p> removed unused imports
 * <p> reformat after emacs trashed it
 * <p>
 * <p> Revision 1.2  2002/09/19 21:37:57  rickg
 * <p> Removed stdout messages
 * <p>
 * <p> Revision 1.1  2002/09/09 22:57:02  rickg
 * <p> Initial CVS entry, basic functionality not including combining
 * <p> </p>
 */

package etomo.ui;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JCheckBox;
import javax.swing.JPanel;
import javax.swing.SpinnerModel;
import javax.swing.SpinnerNumberModel;

import etomo.ApplicationManager;
import etomo.comscript.ConstNewstParam;
import etomo.comscript.ConstTiltParam;
import etomo.comscript.ConstTiltalignParam;
import etomo.comscript.ConstTomopitchParam;
import etomo.comscript.NewstParam;
import etomo.comscript.TiltParam;
import etomo.comscript.TiltalignParam;
import etomo.comscript.TomopitchParam;
import etomo.type.AxisID;

public class TomogramPositioningDialog extends ProcessDialog
    implements ContextMenu {
  public static final String rcsid = "$Id$";

  private JPanel pnlPosition = new JPanel();

  private JPanel pnlTomoParams = new JPanel();

  private LabeledTextField ltfSampleTomoThickness = new LabeledTextField(
    "Sample tomogram thickness: ");

  private JCheckBox cbFiducialess = new JCheckBox("Fiducialess alignment");

  private JPanel pnlWholeTomogram = new JPanel();

  private LabeledSpinner spinBinning;

  private JCheckBox cbWholeTomogram = new JCheckBox("Use whole tomogram");

  private JPanel pnlPositionButtons = new JPanel();

  private MultiLineToggleButton btnSample = new MultiLineToggleButton(
    "Create Sample Tomograms");

  private MultiLineToggleButton btnCreateBoundary = new MultiLineToggleButton(
    "<html><b>Create Boundary Models</b>");

  private MultiLineToggleButton btnTomopitch = new MultiLineToggleButton(
    "<html><b>Compute Z Shift & Pitch Angles</b>");

  private JPanel pnlFinalAlign = new JPanel();

  private LabeledTextField ltfTiltAngleOffset = new LabeledTextField(
    "Total angle offset: ");

  private LabeledTextField ltfTiltAxisZShift = new LabeledTextField(
    "Total Z shift: ");

  private MultiLineToggleButton btnAlign = new MultiLineToggleButton(
    "<html><b>Create Final Alignment</b>");

  private Dimension fullImageSize = new Dimension();

  public TomogramPositioningDialog(ApplicationManager appMgr, AxisID axisID) {
    super(appMgr, axisID);
    fixRootPanel(rootSize);

    rootPanel.setLayout(new BoxLayout(rootPanel, BoxLayout.Y_AXIS));
    btnExecute.setText("Done");
    //  Construct the binning spinner
    SpinnerModel integerModel = new SpinnerNumberModel(1, 1, 50, 1);
    spinBinning = new LabeledSpinner("   binning ", integerModel);
    spinBinning.setTextMaxmimumSize(UIParameters.getSpinnerDimension());

    //  Create the primary panels
    pnlWholeTomogram
      .setLayout(new BoxLayout(pnlWholeTomogram, BoxLayout.X_AXIS));
    pnlWholeTomogram.add(cbWholeTomogram);
    pnlWholeTomogram.add(spinBinning.getContainer());

    pnlTomoParams.setLayout(new BoxLayout(pnlTomoParams, BoxLayout.Y_AXIS));
    UIUtilities.addWithYSpace(pnlTomoParams, ltfSampleTomoThickness
      .getContainer());
    UIUtilities.addWithYSpace(pnlTomoParams, cbFiducialess);
    UIUtilities.addWithYSpace(pnlTomoParams, pnlWholeTomogram);
    UIUtilities.alignComponentsX(pnlTomoParams, Component.LEFT_ALIGNMENT);

    pnlFinalAlign.setLayout(new BoxLayout(pnlFinalAlign, BoxLayout.Y_AXIS));
    UIUtilities.addWithYSpace(pnlFinalAlign, ltfTiltAngleOffset.getContainer());
    UIUtilities.addWithYSpace(pnlFinalAlign, ltfTiltAxisZShift.getContainer());
    UIUtilities.alignComponentsX(pnlFinalAlign, Component.LEFT_ALIGNMENT);

    pnlPosition
      .setBorder(new BeveledBorder("Tomogram Positioning").getBorder());
    pnlPosition.setLayout(new BoxLayout(pnlPosition, BoxLayout.Y_AXIS));

    UIUtilities.addWithYSpace(pnlPosition, pnlTomoParams);
    UIUtilities.addWithSpace(pnlPosition, btnSample, FixedDim.x0_y10);
    UIUtilities.addWithSpace(pnlPosition, btnCreateBoundary, FixedDim.x0_y10);
    UIUtilities.addWithSpace(pnlPosition, btnTomopitch, FixedDim.x0_y10);
    UIUtilities.addWithYSpace(pnlPosition, pnlFinalAlign);
    UIUtilities.addWithSpace(pnlPosition, btnAlign, FixedDim.x0_y10);
    UIUtilities.alignComponentsX(pnlPosition, Component.CENTER_ALIGNMENT);
    UIUtilities
      .setButtonSizeAll(pnlPosition, UIParameters.getButtonDimension());

    //  Create dialog content pane
    rootPanel.add(pnlPosition);
    rootPanel.add(Box.createVerticalGlue());
    rootPanel.add(Box.createRigidArea(FixedDim.x0_y10));
    rootPanel.add(pnlExitButtons);
    rootPanel.add(Box.createRigidArea(FixedDim.x0_y10));

    // Bind the buttons to the action listener
    LocalActionListener localActionListener = new LocalActionListener(this);
    cbFiducialess.addActionListener(localActionListener);
    cbWholeTomogram.addActionListener(localActionListener);
    btnSample.addActionListener(localActionListener);
    btnCreateBoundary.addActionListener(localActionListener);
    btnTomopitch.addActionListener(localActionListener);
    btnAlign.addActionListener(localActionListener);

    //  Mouse adapter for context menu
    GenericMouseAdapter mouseAdapter = new GenericMouseAdapter(this);
    rootPanel.addMouseListener(mouseAdapter);

    // Set the default advanced dialog state
    updateAdvanced();
    setToolTipText();
    updateUIState();
  }

  public void setFiducialessAlignment(boolean state) {
    cbFiducialess.setSelected(state);
    updateUIState();
  }

  public boolean getFiducialessAlignment() {
    return cbFiducialess.isSelected();
  }

  /**
   * Set the align.com parameters in the dialog
   * @param tiltalignParam
   */
  public void setAlignParams(ConstTiltalignParam tiltalignParam) {
    ltfTiltAngleOffset.setText(tiltalignParam.getTiltAngleOffset());
    ltfTiltAxisZShift.setText(tiltalignParam.getTiltAxisZShift());
  }

  /**
   * Get the align.com parameters from the dialog
   * @param tiltalignParam
   * @throws NumberFormatException
   */
  public void getAlignParams(TiltalignParam tiltalignParam)
      throws NumberFormatException {
    try {
      tiltalignParam.setTiltAngleOffset(ltfTiltAngleOffset.getText());
      tiltalignParam.setTiltAxisZShift(ltfTiltAxisZShift.getText());
    }
    catch (NumberFormatException except) {
      throw new NumberFormatException(except.getMessage());
    }
  }

  /**
   * Get the tomopitch.com parameters from the dialog
   * @param TomopitchParam
   */
  public void getTomopitchParams(TomopitchParam tomopitchParam) {
    //TODO set ScaleFactor
    double binning = Double.parseDouble(spinBinning.getValue().toString());
    tomopitchParam.setScaleFactor(binning);
    //temp test
    /*
    tomopitchParam.setExtraThickness("3");
    tomopitchParam.resetModelFiles();
    tomopitchParam.setModelFile("topa.mod");
    tomopitchParam.setModelFile("mida.mod");
    tomopitchParam.setModelFile("bota.mod");
    tomopitchParam.setParameterFile("bob");
    tomopitchParam.setScaleFactor(binning);
    tomopitchParam.setSpacingInY("-150");
    */
    //System.out.println("In getTomopitchParams:" + tomopitchParam.toString());
  }

  /**
   * Set the tilt.com parameters in the dialog
   * @param tiltParam
   */
  public void setTiltParams(ConstTiltParam tiltParam) {
    int binning = ((Integer) spinBinning.getValue()).intValue();

    ltfSampleTomoThickness.setText(tiltParam.getThickness() * binning);
  }

  /**
   * Get the tilt.com parameters from the dialog
   * @param tiltParam
   * @throws NumberFormatException
   */
  public void getTiltParams(TiltParam tiltParam) throws NumberFormatException {
    int binning = ((Integer) spinBinning.getValue()).intValue();
    try {
      // Set the appropriate FULLIMAGE line
      tiltParam.setFullImageX(fullImageSize.width / binning);
      tiltParam.setFullImageY(fullImageSize.height / binning);

      tiltParam.setThickness(Integer.parseInt(ltfSampleTomoThickness.getText())
          / binning);
    }
    catch (NumberFormatException except) {
      String message = "Axis " + axisID.getExtension() + except.getMessage();
      throw new NumberFormatException(message);
    }
  }

  /**
   * Set the newst.com parameters in the dialog
   * @param newstParam
   */
  public void setNewstParams(ConstNewstParam newstParam) {
    int binning = newstParam.getBinByFactor();
    if (binning > 1) {
      spinBinning.setValue(binning);
    }
  }
  
  public void setTomopitchParams(ConstTomopitchParam tomopitchParam) {
    //temp test
    //System.out.println("In setTomopitchParams:" + tomopitchParam.toString());
  }

  /**
   * Get the newst.com parameters from the dialog
   * @param newstParam
   */
  public void getNewstParamst(NewstParam newstParam) {
    int binning = ((Integer) spinBinning.getValue()).intValue();

    // Only explcitly write out the binning if its value is something other than
    // the default of 1 to keep from cluttering up the com script  
    if (binning > 1) {
      newstParam.setBinByFactor(binning);
    }
    else {
      newstParam.setBinByFactor(Integer.MIN_VALUE);
    }
  }

  /**
   * @return Returns the fullImageSize.
   */
  public Dimension getFullImageSize() {
    return new Dimension(fullImageSize);
  }

  /**
   * Set the full image size
   * @param width
   * @param height
   */
  public void setFullImageSize(int width, int height) {
    fullImageSize.setSize(width, height);
  }

  /**
   * Right mouse button context menu
   */
  public void popUpContextMenu(MouseEvent mouseEvent) {
    String[] manPagelabel = {"Tomopitch", "Newst", "3dmod", "Tilt"};
    String[] manPage = {"tomopitch.html", "newst.html", "3dmod.html",
        "tilt.html"};

    String[] logFileLabel = {"Tomopitch", "Sample"};
    String[] logFile = new String[2];
    logFile[0] = "tomopitch" + axisID.getExtension() + ".log";
    logFile[1] = "sample" + axisID.getExtension() + ".log";

    ContextPopup contextPopup = new ContextPopup(rootPanel, mouseEvent,
      "TOMOGRAM POSITIONING", manPagelabel, manPage, logFileLabel, logFile);
  }

  //  Button action handler methods
  private void buttonAction(ActionEvent event) {
    String command = event.getActionCommand();

    if (command.equals(btnSample.getActionCommand())) {
      if (cbWholeTomogram.isSelected()) {
        applicationManager.wholeTomogram(axisID);
      }
      else {
        applicationManager.createSample(axisID);
      }
    }

    else if (command.equals(btnCreateBoundary.getActionCommand())) {
      if (cbWholeTomogram.isSelected()) {
        applicationManager.imodFullSample(axisID);
      }
      else {
        applicationManager.imodSample(axisID);
      }
    }

    else if (command.equals(btnTomopitch.getActionCommand())) {
      applicationManager.tomopitch(axisID);
    }

    else if (command.equals(btnAlign.getActionCommand())) {
      applicationManager.finalAlign(axisID);
    }

    else if (command.equals(cbFiducialess.getActionCommand())) {
      updateUIState();
    }
    else if (command.equals(cbWholeTomogram.getActionCommand())) {
      updateUIState();
    }
  }

  public void updateUIState() {
    ltfTiltAngleOffset.setEnabled(!cbFiducialess.isSelected());
    ltfTiltAxisZShift.setEnabled(!cbFiducialess.isSelected());
    btnAlign.setEnabled(!cbFiducialess.isSelected());

    if (cbWholeTomogram.isSelected()) {
      spinBinning.setEnabled(true);
      btnSample.setText("Create Whole Tomogram");
    }
    else {
      spinBinning.setEnabled(false);
      btnSample.setText("Create Sample Tomograms");
    }
  }

  //  Action function overides for buttons
  public void buttonCancelAction(ActionEvent event) {
    super.buttonCancelAction(event);
    applicationManager.doneTomogramPositioningDialog(axisID);
  }

  public void buttonPostponeAction(ActionEvent event) {
    super.buttonPostponeAction(event);
    applicationManager.doneTomogramPositioningDialog(axisID);
  }

  public void buttonExecuteAction(ActionEvent event) {
    super.buttonExecuteAction(event);
    applicationManager.doneTomogramPositioningDialog(axisID);
  }

  public void buttonAdvancedAction(ActionEvent event) {
    super.buttonAdvancedAction(event);
    updateAdvanced();
  }

  private void updateAdvanced() {
    applicationManager.packMainWindow();
  }

  //
  //	Action listener adapters
  //
  class LocalActionListener implements ActionListener {

    TomogramPositioningDialog adaptee;

    LocalActionListener(TomogramPositioningDialog adaptee) {
      this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent event) {
      adaptee.buttonAction(event);
    }
  }

  /**
   * Initialize the tooltip text for the axis panel objects
   */
  private void setToolTipText() {
    String text;
    TooltipFormatter tooltipFormatter = new TooltipFormatter();
    text = "Thickness of sample slices.  Make this much larger than expected section"
        + " thickness to see borders of section.";
    ltfSampleTomoThickness.setToolTipText(tooltipFormatter.setText(text)
      .format());
    text = "Build 3 sample tomograms for finding location and angles of section.";
    btnSample.setToolTipText(tooltipFormatter.setText(text).format());

    text = "Open samples in 3dmod to make a model with lines along top and bottom "
        + "edges of the section in each sample.";
    btnCreateBoundary.setToolTipText(tooltipFormatter.setText(text).format());

    text = "Run tomopitch.  You need to examine the log file to determine the"
        + "Z shift, additional angle offset, and X-axis tilt.";
    btnTomopitch.setToolTipText(tooltipFormatter.setText(text).format());

    text = "Add the additional offset from tomopitch to the amount already "
        + "shown here to get the total offset.";
    ltfTiltAngleOffset.setToolTipText(tooltipFormatter.setText(text).format());

    text = "Add the additional shift from tomopitch to the amount shown here to get "
        + "the total shift.";
    ltfTiltAxisZShift.setToolTipText(tooltipFormatter.setText(text).format());

    text = "Run tiltalign with these final offset parameters.";
    btnAlign.setToolTipText(tooltipFormatter.setText(text).format());
  }
}