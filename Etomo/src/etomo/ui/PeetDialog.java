package etomo.ui;

import java.awt.Container;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JFileChooser;
import javax.swing.JPanel;

import etomo.PeetManager;
import etomo.comscript.ParallelParam;
import etomo.comscript.ProcesschunksParam;
import etomo.type.AxisID;
import etomo.type.ConstPeetMetaData;
import etomo.type.ConstPeetScreenState;
import etomo.type.DialogType;
import etomo.type.PeetMetaData;
import etomo.type.PeetScreenState;

/**
 * <p>Description: </p>
 * 
 * <p>Copyright: Copyright 2006</p>
 *
 * <p>Organization:
 * Boulder Laboratory for 3-Dimensional Electron Microscopy of Cells (BL3DEMC),
 * University of Colorado</p>
 * 
 * @author $Author$
 * 
 * @version $Revision$
 * 
 * <p> $Log$
 * <p> Revision 1.4  2007/02/21 22:30:22  sueh
 * <p> bug# 964 Fixing null pointer exception which occurred when loading the .epe file.
 * <p>
 * <p> Revision 1.3  2007/02/21 04:24:32  sueh
 * <p> bug# 964 Setting Output and Directory when Save As is called.  Disabling edit
 * <p> for Output and Directory when the paramFile is set.
 * <p>
 * <p> Revision 1.2  2007/02/20 20:36:46  sueh
 * <p> bug# 964 Started the setup panel.
 * <p>
 * <p> Revision 1.1  2007/02/19 22:03:19  sueh
 * <p> bug# 964 Dialog for PEET interface.
 * <p> </p>
 */

public final class PeetDialog implements AbstractParallelDialog, Expandable {
  public static final String rcsid = "$Id$";

  static final String DIRECTORY_LABEL = "Directory";
  static final String OUTPUT_LABEL = "Output";

  private static final DialogType DIALOG_TYPE = DialogType.PEET;
  private final FileTextField ftfDirectory = new FileTextField(
      DIRECTORY_LABEL + ": ");
  private final LabeledTextField ltfOutput = new LabeledTextField(OUTPUT_LABEL
      + ": ");
  private final SpacedPanel pnlSetup = new SpacedPanel();
  private final SpacedPanel pnlSetupBody = new SpacedPanel();
  private final VolumeTable volumeTable;
  private final PeetManager manager;
  private final AxisID axisID;
  private final PanelHeader setupHeader;
  private JPanel rootPanel = null;

  public PeetDialog(PeetManager manager, AxisID axisID) {
    this.manager = manager;
    this.axisID = axisID;
    volumeTable = new VolumeTable(manager);
    setupHeader = PanelHeader.getInstance("Setup", this, DIALOG_TYPE);
  }

  public void updateDisplay(boolean paramFileSet) {
    ftfDirectory.setEnabled(!paramFileSet);
    ltfOutput.setEditable(!paramFileSet);
  }

  public Container getContainer() {
    create();
    return rootPanel;
  }

  public DialogType getDialogType() {
    return DIALOG_TYPE;
  }
  
  public void getParameters(ParallelParam param) {
    ProcesschunksParam processchunksParam = (ProcesschunksParam) param;
    processchunksParam.setRootName(ltfOutput.getText());
  }
  
  public void getParameters(PeetScreenState screenState) {
    setupHeader.getState(screenState.getPeetSetupHeaderState());
  }
  
  public void setParameters(ConstPeetScreenState screenState) {
    setupHeader.setState(screenState.getPeetSetupHeaderState());
  }  
  
  public void getParameters(PeetMetaData metaData) {
    metaData.setName(ltfOutput.getText());
  }
  
  public void setParameters(ConstPeetMetaData metaData) {
    ltfOutput.setText(metaData.getName());
  }

  public final boolean usingParallelProcessing() {
    return true;
  }

  public void expand(ExpandButton button) {
    if (setupHeader != null) {
      if (setupHeader.equalsOpenClose(button)) {
        pnlSetupBody.setVisible(button.isExpanded());
      }
    }
    UIHarness.INSTANCE.pack(axisID, manager);
  }
  
  public String getDirectory() {
    return ftfDirectory.getText();
  }
  
  public void setDirectory(String directory) {
    ftfDirectory.setText(directory);
  }
  
  public void setOutput(String output) {
    ltfOutput.setText(output);
  }
  
  void directoryAction() {
    JFileChooser chooser = new JFileChooser(new File(manager
        .getPropertyUserDir()));
    chooser.setPreferredSize(UIParameters.INSTANCE.getFileChooserDimension());
    chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
    int returnVal = chooser.showOpenDialog(rootPanel);
    if (returnVal == JFileChooser.APPROVE_OPTION) {
      ftfDirectory.setText(chooser.getSelectedFile().getAbsolutePath());
    }
  }

  private void create() {
    if (rootPanel != null) {
      return;
    }
    //setup
    pnlSetupBody.setBoxLayout(BoxLayout.Y_AXIS);
    pnlSetupBody.add(ftfDirectory.getContainer());
    pnlSetupBody.add(ltfOutput.getContainer());
    pnlSetupBody.add(volumeTable.getContainer());
    //setup header
    pnlSetup.setBoxLayout(BoxLayout.Y_AXIS);
    pnlSetup.setBorder(BorderFactory.createEtchedBorder());
    pnlSetup.add(setupHeader.getContainer());
    pnlSetup.add(pnlSetupBody);
    //root
    rootPanel = new JPanel();
    rootPanel.setLayout(new BoxLayout(rootPanel, BoxLayout.Y_AXIS));
    rootPanel.add(pnlSetup.getContainer());
    //actions
    ftfDirectory.addActionListener(new DirectoryActionListener(this));
  }
  
  private class DirectoryActionListener implements ActionListener {
    private PeetDialog peetDialog;

    private DirectoryActionListener(PeetDialog peetDialog) {
      this.peetDialog = peetDialog;
    }

    public void actionPerformed(ActionEvent event) {
      peetDialog.directoryAction();
    }
  }
}
