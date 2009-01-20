package etomo.ui;

import javax.swing.JPanel;
import javax.swing.border.TitledBorder;

import etomo.EtomoDirector;
import etomo.storage.autodoc.AutodocTokenizer;
import etomo.type.UITestFieldType;
import etomo.util.Utilities;

/**
 * <p>Description: A JPanel that names itself when it receives a title.</p>
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
class EtomoPanel extends JPanel {
  public static final String rcsid = "$Id$";

  void setBorder(final TitledBorder border) {
    super.setBorder(border);
    setName(border.getTitle());
  }

  void add(final PanelHeader panelHeader) {
    add(panelHeader.getContainer());
    setName(panelHeader.getTitle());
  }

  public void setName(String text) {
    String name = Utilities.convertLabelToName(text);
    super.setName(name);
    if (EtomoDirector.INSTANCE.getArguments().isPrintNames()) {
      System.out.println(UITestFieldType.PANEL.toString()
          + AutodocTokenizer.SEPARATOR_CHAR + name + ' '
          + AutodocTokenizer.DEFAULT_DELIMITER + ' ');
    }
  }
}
