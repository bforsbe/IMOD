package etomo.ui;

import java.awt.event.*;
import javax.swing.*;

/**
 * <p>Description: </p>
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
 * <p> Revision 1.1  2002/09/09 22:57:02  rickg
 * <p> Initial CVS entry, basic functionality not including combining
 * <p> </p>
 */
public class PostProcessingDialog extends ProcessDialog {
  public static final String rcsid =
    "$Id$";

  JPanel contentPane;

  public PostProcessingDialog() {
    contentPane = (JPanel) this.getContentPane();
    contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.Y_AXIS));
    this.setTitle("eTomo Post-processing and Trimming");

    contentPane.add(Box.createVerticalGlue());
    contentPane.add(Box.createRigidArea(FixedDim.x0_y10));
    contentPane.add(panelExitButtons);
    contentPane.add(Box.createRigidArea(FixedDim.x0_y10));

    //
    // Calcute the necessary window size
    //
    pack();
  }

  //
  //  Action function overides for buttons
  //
  public void buttonPostponeAction(ActionEvent event) {
    super.buttonPostponeAction(event);
  }

  public void buttonExecuteAction(ActionEvent event) {
    super.buttonExecuteAction(event);
  }
}
