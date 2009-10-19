package etomo.ui;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.BoxLayout;
import javax.swing.JFileChooser;

import etomo.PeetManager;
import etomo.storage.MatlabParamFileFilter;
import etomo.storage.PeetAndMatlabParamFileFilter;
import etomo.storage.PeetFileFilter;
import etomo.type.Run3dmodMenuOptions;

/**
 * <p>Description: Panel that creates a PEET dataset using an existing project.
 * It currently has no saved state so it is completely reusable.</p>
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
final class UseExistingProjectPanel {
  public static final String rcsid = "$Id$";

  private final MultiLineButton btnImportMatlabParamFile = new MultiLineButton(
      "Import a .prm File");
  private final MultiLineButton btnDuplicateProject = new MultiLineButton(
      "Duplicate an Existing Project");
  private final MultiLineButton btnCopyParameters = new MultiLineButton(
      "Copy Parameters");
  private final EtomoPanel pnlRoot = new EtomoPanel();

  private final PeetManager manager;
  private final UseExistingProjectParent parent;

  private UseExistingProjectPanel(final PeetManager manager,
      final UseExistingProjectParent parent) {
    this.manager = manager;
    this.parent = parent;
  }

  /**
   * Create instance and panel.  Add tooltips and listeners.
   * @param manager
   * @param parent
   * @return
   */
  static UseExistingProjectPanel getInstance(final PeetManager manager,
      final UseExistingProjectParent parent) {
    UseExistingProjectPanel instance = new UseExistingProjectPanel(manager,
        parent);
    instance.createPanel();
    instance.setTooltips();
    instance.addListeners();
    return instance;
  }

  /**
   * Create the panel.
   */
  private void createPanel() {
    //use existing project
    pnlRoot.setLayout(new BoxLayout(pnlRoot, BoxLayout.X_AXIS));
    pnlRoot.setBorder(new EtchedBorder("Use Existing Project").getBorder());
    btnImportMatlabParamFile.setSize();
    pnlRoot.add(btnImportMatlabParamFile.getComponent());
    btnDuplicateProject.setSize();
    pnlRoot.add(btnDuplicateProject.getComponent());
    btnCopyParameters.setSize();
    pnlRoot.add(btnCopyParameters.getComponent());
  }

  /**
   * Add listeners.
   */
  private void addListeners() {
    UseExistingProjectActionListener actionListener = new UseExistingProjectActionListener(
        this);
    btnImportMatlabParamFile.addActionListener(actionListener);
    btnDuplicateProject.addActionListener(actionListener);
    btnCopyParameters.addActionListener(actionListener);
  }

  /**
   * Get the root panel.
   * @return
   */
  Component getComponent() {
    return pnlRoot;
  }

  /**
   * Toggles between a setup-like mode where the location and root name being
   * chosen, and a regular mode.
   * @param paramFileSet
   */
  public void updateDisplay(final boolean paramFileSet) {
    btnImportMatlabParamFile.setEnabled(!paramFileSet);
    btnDuplicateProject.setEnabled(!paramFileSet);
    btnCopyParameters.setEnabled(!paramFileSet);
  }

  /**
   * Create a project out of a matlab param file.
   */
  private void importMatlabParam() {
    String path = parent.getDirectory().getText();
    if (path == null || path.matches("\\s*")) {
      UIHarness.INSTANCE.openMessageDialog(
          "Please set the " + PeetDialog.DIRECTORY_LABEL
              + " field before importing a .prm file.", "Entry Error", manager
              .getManagerKey());
      return;
    }
    File dir = new File(parent.getDirectory().getText());
    if (!dir.exists()) {
      UIHarness.INSTANCE.openMessageDialog("Please create "
          + dir.getAbsolutePath() + " before importing a .prm file.",
          "Entry Error", manager.getManagerKey());
      return;
    }
    File matlabParamFile = null;
    JFileChooser chooser = new JFileChooser(dir);
    chooser.setFileFilter(new MatlabParamFileFilter());
    chooser.setPreferredSize(UIParameters.INSTANCE.getFileChooserDimension());
    chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    int returnVal = chooser.showOpenDialog(pnlRoot);
    if (returnVal != JFileChooser.APPROVE_OPTION) {
      return;
    }
    matlabParamFile = chooser.getSelectedFile();
    manager.loadMatlabParam(matlabParamFile, false);
  }

  /**
   * Create a project out of a peet file from another directory.
   */
  private void duplicateExistingProject() {
    String path = parent.getDirectory().getText();
    if (path == null || path.matches("\\s*")) {
      UIHarness.INSTANCE.openMessageDialog(
          "Please set the " + PeetDialog.DIRECTORY_LABEL
              + " field before importing a .prm file.", "Entry Error", manager
              .getManagerKey());
      return;
    }
    File dir = new File(parent.getDirectory().getText());
    if (!dir.exists()) {
      UIHarness.INSTANCE.openMessageDialog("Please create "
          + dir.getAbsolutePath() + " before importing a .prm file.",
          "Entry Error", manager.getManagerKey());
      return;
    }
    JFileChooser chooser = new JFileChooser(dir);
    chooser.setFileFilter(new PeetFileFilter());
    chooser.setPreferredSize(UIParameters.INSTANCE.getFileChooserDimension());
    chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    int returnVal = chooser.showOpenDialog(pnlRoot);
    if (returnVal != JFileChooser.APPROVE_OPTION) {
      return;
    }
    File peetFile = chooser.getSelectedFile();
    manager.loadParamFile(peetFile, false);
  }

  /**
   * Create a project out of a peet file or a .prm file from another directory.
   * Copy everything but the volume table
   */
  private void copyParameters() {
    String path = parent.getDirectory().getText();
    if (path == null || path.matches("\\s*")) {
      UIHarness.INSTANCE.openMessageDialog("Please set the "
          + PeetDialog.DIRECTORY_LABEL + " field before copying parameters.",
          "Entry Error", manager.getManagerKey());
      return;
    }
    File dir = new File(parent.getDirectory().getText());
    if (!dir.exists()) {
      UIHarness.INSTANCE.openMessageDialog("Please create "
          + dir.getAbsolutePath() + " before copy parameters.", "Entry Error",
          manager.getManagerKey());
      return;
    }
    JFileChooser chooser = new JFileChooser(dir);
    chooser.setFileFilter(new PeetAndMatlabParamFileFilter());
    chooser.setPreferredSize(UIParameters.INSTANCE.getFileChooserDimension());
    chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    int returnVal = chooser.showOpenDialog(pnlRoot);
    if (returnVal != JFileChooser.APPROVE_OPTION) {
      return;
    }
    File file = chooser.getSelectedFile();
    manager.copyParameters(file);
  }

  /**
   * Responds to button presses.
   * @param actionCommand
   * @param run3dmodMenuOptions
   */
  private void action(final String actionCommand,
      final Run3dmodMenuOptions run3dmodMenuOptions) {
    if (actionCommand.equals(btnImportMatlabParamFile.getActionCommand())) {
      importMatlabParam();
    }
    else if (actionCommand.equals(btnDuplicateProject.getActionCommand())) {
      duplicateExistingProject();
    }
    else if (actionCommand.equals(btnCopyParameters.getActionCommand())) {
      copyParameters();
    }
  }

  /**
   * Set tooltips.
   */
  private void setTooltips() {
    btnImportMatlabParamFile
        .setToolTipText("Create a new PEET project from a .prm file.");
    btnDuplicateProject
        .setToolTipText("Create a new PEET project from .epe and .prm files in "
            + "another directory.");
    btnCopyParameters
        .setToolTipText("Create a new PEET project and copy the parameters "
            + "(everything but the volume table) from .epe and/or .prm file(s) "
            + "in another directory.");
  }

  /**
   * Action listener for UseExistingProjectPanel.
   * @author sueh
   */
  private static final class UseExistingProjectActionListener implements
      ActionListener {
    private final UseExistingProjectPanel panel;

    private UseExistingProjectActionListener(final UseExistingProjectPanel panel) {
      this.panel = panel;
    }

    public void actionPerformed(final ActionEvent event) {
      panel.action(event.getActionCommand(), null);
    }
  }
}