package etomo.ui;

import etomo.ApplicationManager;
import etomo.type.AxisID;
import etomo.type.DialogType;
import etomo.type.ProcessResultDisplay;
import etomo.type.Run3dmodMenuOptions;

/**
 * <p>Description: </p>
 * 
 * <p>Copyright: Copyright 2008</p>
 *
 * <p>Organization:
 * Boulder Laboratory for 3-Dimensional Electron Microscopy of Cells (BL3DEMC),
 * University of Colorado</p>
 * 
 * @author $Author$
 * 
 * @version $Revision$
 * 
 * <p> $Log$ </p>
 */
final class TiltPanel extends AbstractTiltPanel {
  public static final String rcsid = "$Id$";

  //backward compatibility functionality - if the metadata binning is missing
  //get binning from newst
  private TiltPanel(ApplicationManager manager, AxisID axisID,
      DialogType dialogType, TiltParent parent,
      GlobalExpandButton globalAdvancedButton) {
    super(manager, axisID, dialogType, parent, globalAdvancedButton);
  }

  static TiltPanel getInstance(ApplicationManager manager, AxisID axisID,
      DialogType dialogType, TiltParent parent,
      GlobalExpandButton globalAdvancedButton) {
    TiltPanel instance = new TiltPanel(manager, axisID, dialogType, parent,
        globalAdvancedButton);
    instance.createPanel();
    instance.setToolTipText();
    instance.addListeners();
    return instance;
  }

  Run3dmodButton getTiltButton(ApplicationManager manager, AxisID axisID) {
    return (Run3dmodButton) manager.getProcessResultDisplayFactory(axisID)
        .getGenerateTomogram();
  }

  public static ProcessResultDisplay getGenerateTomogramResultDisplay(
      DialogType dialogType) {
    return Run3dmodButton.getDeferredToggle3dmodInstance("Generate Tomogram",
        dialogType);
  }

  public static ProcessResultDisplay getDeleteAlignedStackResultDisplay(
      DialogType dialogType) {
    return MultiLineButton.getToggleButtonInstance(
        "Delete Aligned Image Stack", dialogType);
  }

  /**
   * Z shift is an advanced field.
   */
  void updateAdvanced(final boolean advanced) {
    super.updateAdvanced(advanced);
    ltfZShift.setVisible(advanced);
  }

  void tiltAction(final Deferred3dmodButton deferred3dmodButton,
      final Run3dmodMenuOptions run3dmodMenuOptions) {
    manager.tiltAction(getTiltProcessResultDisplay(), null,
        deferred3dmodButton, run3dmodMenuOptions, this, axisID, dialogType);
  }

  void imodTomogramAction(final Deferred3dmodButton deferred3dmodButton,
      final Run3dmodMenuOptions run3dmodMenuOptions) {
    manager.imodFullVolume(axisID, run3dmodMenuOptions);
  }
}