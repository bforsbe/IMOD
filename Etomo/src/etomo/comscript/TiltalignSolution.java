package etomo.comscript;

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
 * <p> $Log$ </p>
 */
public class TiltalignSolution {
  public static final String rcsid = "$Id$";

  public int type;
  public FortranInputString referenceView;
  public FortranInputString params;
  public StringList additionalGroups;

  public TiltalignSolution() {
    params = new FortranInputString(2);
    params .setIntegerType(0, true);
    params .setIntegerType(1, true);
    referenceView = new FortranInputString(1);
    referenceView.setIntegerType(0, true);
    additionalGroups = new StringList(0);
  }

  /**
   * Copy constructor
   */
  public TiltalignSolution(TiltalignSolution src) {
    type = src.type;
    referenceView = src.referenceView;
    params = new FortranInputString(src.params);
    additionalGroups = new StringList(src.additionalGroups);
  }
}
