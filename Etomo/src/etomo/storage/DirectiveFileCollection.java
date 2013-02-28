package etomo.storage;

import java.io.File;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;

import etomo.BaseManager;
import etomo.logic.DatasetTool;
import etomo.logic.UserEnv;
import etomo.storage.DirectiveFile.AttributeName;
import etomo.storage.autodoc.ReadOnlyAttribute;
import etomo.storage.autodoc.ReadOnlyAttributeIterator;
import etomo.type.AxisID;
import etomo.type.EtomoNumber;
import etomo.type.TiltAngleSpec;
import etomo.type.TiltAngleType;
import etomo.ui.SetupReconInterface;

/**
* <p>Description: </p>
* 
* <p>Copyright: Copyright 2012</p>
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
public class DirectiveFileCollection implements SetupReconInterface {
  public static final String rcsid = "$Id:$";

  private final DirectiveFile[] directiveFileArray = new DirectiveFile[] { null, null,
      null, null };

  private final BaseManager manager;
  private final AxisID axisID;

  public DirectiveFileCollection(final BaseManager manager, final AxisID axisID) {
    this.manager = manager;
    this.axisID = axisID;
  }

  /**
   * Returns true if an attribute called name is in any of the directive files.
   * @param parentName
   * @param name
   * @return
   */
  private boolean containsAttribute(final DirectiveFile.AttributeName parentName,
      final String name) {
    for (int i = 0; i < directiveFileArray.length; i++) {
      if (directiveFileArray[i] != null) {
        if (directiveFileArray[i].containsAttribute(parentName, name)) {
          return true;
        }
      }
    }
    return false;
  }

  private String getValue(final DirectiveFile.AttributeName parentName, final String name) {
    String value = null;
    for (int i = 0; i < directiveFileArray.length; i++) {
      if (directiveFileArray[i] != null
          && directiveFileArray[i].containsAttribute(parentName, name)) {
        value = directiveFileArray[i].getValue(parentName, name);
      }
    }
    return value;
  }

  public boolean containsBinning() {
    return containsAttribute(AttributeName.COPY_ARG, DirectiveFile.BINNING_NAME);
  }

  public boolean containsDatasetDirectory() {
    return containsAttribute(DirectiveFile.AttributeName.SETUP_SET,
        DirectiveFile.DATASET_DIRECTORY_NAME);
  }

  public boolean containsDistort() {
    return containsAttribute(AttributeName.COPY_ARG, DirectiveFile.DISTORT_NAME);
  }

  public boolean containsFocus(final AxisID axisID) {
    return containsAttribute(AttributeName.COPY_ARG,
        DirectiveFile.convertAttributeName(axisID, DirectiveFile.FOCUS_NAME));
  }

  public boolean containsGold() {
    return containsAttribute(DirectiveFile.AttributeName.COPY_ARG,
        DirectiveFile.GOLD_NAME);
  }

  public boolean containsGradient() {
    return containsAttribute(AttributeName.COPY_ARG, DirectiveFile.GRADIENT_NAME);
  }

  public boolean containsPixel() {
    return containsAttribute(DirectiveFile.AttributeName.COPY_ARG,
        DirectiveFile.PIXEL_NAME);
  }

  public boolean containsRotation() {
    return containsAttribute(AttributeName.COPY_ARG, DirectiveFile.ROTATION_NAME);
  }

  public boolean containsTiltAngleSpec(final AxisID axisID) {
    return containsAttribute(AttributeName.COPY_ARG,
        DirectiveFile.convertAttributeName(axisID, DirectiveFile.FIRST_INC_NAME))
        || containsAttribute(AttributeName.COPY_ARG,
            DirectiveFile.convertAttributeName(axisID, DirectiveFile.USE_RAW_TLT_NAME))
        || containsAttribute(AttributeName.COPY_ARG,
            DirectiveFile.convertAttributeName(axisID, DirectiveFile.EXTRACT_NAME));
  }

  public String getBackupDirectory() {
    return null;
  }

  public DirectiveFile getBatchDirectiveFile() {
    return directiveFileArray[OverrideOrder.BATCH.index];
  }

  public String getBinning() {
    return getValue(DirectiveFile.AttributeName.COPY_ARG, DirectiveFile.BINNING_NAME);
  }

  /**
   * Returns an entry set containing the names/value pairs in all of the directive files
   * (one entry per name).  This function will not return null.  A name/value pair with a
   * blank values cause the name/value pair to be removed from the entry set.  The
   * name/value pair will be re-added afterwards if a pair with a non-blank value is
   * encountered.
   * @return
   */
  public CopyArgEntrySet getCopyArgEntrySet() {
    return CopyArgEntrySet.getInstance(directiveFileArray);
  }

  public String getDataset() {
    return getName();
  }

  public String getDatasetDirectory() {
    return getValue(DirectiveFile.AttributeName.SETUP_SET,
        DirectiveFile.DATASET_DIRECTORY_NAME);
  }

  public DirectiveFileCollection getDirectiveFileCollection() {
    return this;
  }

  public String getDistortionFile() {
    return getValue(DirectiveFile.AttributeName.COPY_ARG, DirectiveFile.DISTORT_NAME);
  }

  /**
   * @param doValidation has no effect
   */
  public String getExcludeList(final AxisID axisID, final boolean doValidation) {
    return getValue(DirectiveFile.AttributeName.COPY_ARG,
        DirectiveFile.convertAttributeName(axisID, DirectiveFile.SKIP_NAME));
  }

  /**
   * @param doValidation has no effect
   */
  public String getFiducialDiameter(final boolean doValidation) {
    return getValue(DirectiveFile.AttributeName.COPY_ARG, DirectiveFile.GOLD_NAME);
  }

  /**
   * @param doValidation has no effect
   */
  public String getImageRotation(final AxisID axisID, final boolean doValidation) {
    return getValue(DirectiveFile.AttributeName.COPY_ARG,
        DirectiveFile.convertAttributeName(axisID, DirectiveFile.ROTATION_NAME));
  }

  /**
   * Returns binning or defaultRetValue if binning is invalid or missing.
   * @param defaultRetValue
   * @return
   */
  public int getIntBinning(final int defaultRetValue) {
    EtomoNumber binning = new EtomoNumber();
    binning.set(getBinning());
    if (binning.isValid() && !binning.isNull()) {
      return binning.getInt();
    }
    return defaultRetValue;
  }

  public String getMagGradientFile() {
    return getValue(DirectiveFile.AttributeName.COPY_ARG, DirectiveFile.GRADIENT_NAME);
  }

  public String getName() {
    return getValue(DirectiveFile.AttributeName.COPY_ARG, DirectiveFile.NAME_NAME);
  }

  public String getPixelSize(final boolean doValidation) {
    return getValue(DirectiveFile.AttributeName.COPY_ARG, DirectiveFile.PIXEL_NAME);
  }

  public DirectiveFile getScopeTemplate() {
    return directiveFileArray[OverrideOrder.SCOPE.index];
  }

  public DirectiveFile getSystemTemplate() {
    return directiveFileArray[OverrideOrder.SYSTEM.index];
  }

  public boolean getTiltAngleFields(final AxisID axisID,
      final TiltAngleSpec tiltAngleSpec, final boolean doValidation) {
    tiltAngleSpec.reset();
    for (int i = 0; i < directiveFileArray.length; i++) {
      if (directiveFileArray[i] != null && containsTiltAngleSpec(axisID)) {
        directiveFileArray[i].getTiltAngleFields(axisID, tiltAngleSpec, doValidation);
      }
    }
    return true;
  }

  public DirectiveFile getUserTemplate() {
    return directiveFileArray[OverrideOrder.USER.index];
  }

  public boolean isAdjustedFocusSelected(final AxisID axisID) {
    return DirectiveFile.toBoolean(getValue(DirectiveFile.AttributeName.COPY_ARG,
        DirectiveFile.convertAttributeName(axisID, DirectiveFile.FOCUS_NAME)));
  }

  public boolean isDual() {
    return DirectiveFile.toBoolean(getValue(DirectiveFile.AttributeName.COPY_ARG,
        DirectiveFile.DUAL_NAME));
  }

  public boolean isDualAxisSelected() {
    return isDual();
  }

  public boolean isMontage() {
    return DirectiveFile.toBoolean(getValue(DirectiveFile.AttributeName.COPY_ARG,
        DirectiveFile.MONTAGE_NAME));
  }

  public boolean isGpuProcessingSelected(final String propertyUserDir) {
    return UserEnv.isGpuProcessing(manager, axisID, propertyUserDir);
  }

  public boolean isParallelProcessSelected(final String propertyUserDir) {
    return UserEnv.isParallelProcessing(manager, axisID, propertyUserDir);
  }

  public boolean isScanHeader() {
    return DirectiveFile.toBoolean(getValue(DirectiveFile.AttributeName.SETUP_SET,
        DirectiveFile.SCAN_HEADER_NAME));
  }

  public boolean isSingleAxisSelected() {
    return !isDual();
  }

  public boolean isSingleViewSelected() {
    return !DirectiveFile.toBoolean(getValue(DirectiveFile.AttributeName.COPY_ARG,
        DirectiveFile.MONTAGE_NAME));
  }

  public void setBatchDirectiveFile(final DirectiveFile baseDirectiveFile) {
    directiveFileArray[OverrideOrder.BATCH.index] = baseDirectiveFile;
    if (baseDirectiveFile != null) {
      String filePath = baseDirectiveFile.getScopeTemplate();
      if (filePath != null) {
        directiveFileArray[OverrideOrder.SCOPE.index] = DirectiveFile.getInstance(
            manager, axisID, new File(filePath));
      }
      filePath = baseDirectiveFile.getSystemTemplate();
      if (filePath != null) {
        directiveFileArray[OverrideOrder.SYSTEM.index] = DirectiveFile.getInstance(
            manager, axisID, new File(filePath));
      }
      filePath = baseDirectiveFile.getUserTemplate();
      if (filePath != null) {
        directiveFileArray[OverrideOrder.USER.index] = DirectiveFile.getInstance(manager,
            axisID, new File(filePath));
      }
    }
  }

  public void setBinning(final int input) {
    if (directiveFileArray[OverrideOrder.BATCH.index] != null) {
      directiveFileArray[OverrideOrder.BATCH.index].setBinning(input);
    }
  }

  public void setImageRotation(final String input) {
    if (directiveFileArray[OverrideOrder.BATCH.index] != null) {
      directiveFileArray[OverrideOrder.BATCH.index].setImageRotation(input);
    }
  }

  public void setPixelSize(final double input) {
    if (directiveFileArray[OverrideOrder.BATCH.index] != null) {
      directiveFileArray[OverrideOrder.BATCH.index].setPixelSize(input);
    }
  }

  public void setScopeTemplate(final File file) {
    if (file == null) {
      directiveFileArray[OverrideOrder.SCOPE.index] = null;
    }
    else {
      if (!file.isAbsolute()) {
        System.err
            .println("Error:  the scope template in the batch directive file has a "
                + "relative path.  File path may be wrong:" + file.getAbsolutePath());
      }
      directiveFileArray[OverrideOrder.SCOPE.index] = DirectiveFile.getInstance(manager,
          axisID, file);
    }
  }

  public void setSystemTemplate(final File file) {
    if (file == null) {
      directiveFileArray[OverrideOrder.SYSTEM.index] = null;
    }
    else {
      if (!file.isAbsolute()) {
        System.err
            .println("Error:  the system template in the batch directive file has a "
                + "relative path.  File path may be wrong:" + file.getAbsolutePath());
      }
      directiveFileArray[OverrideOrder.SYSTEM.index] = DirectiveFile.getInstance(manager,
          axisID, file);
    }
  }

  public void setUserTemplate(final File file) {
    if (file == null) {
      directiveFileArray[OverrideOrder.USER.index] = null;
    }
    else {
      if (!file.isAbsolute()) {
        System.err.println("Error:  the user template in the batch directive file has a "
            + "relative path.  File path may be wrong:" + file.getAbsolutePath());
      }
      directiveFileArray[OverrideOrder.USER.index] = DirectiveFile.getInstance(manager,
          axisID, file);
    }
  }

  public boolean validateTiltAngle(final AxisID axisID, final String errorTitle) {
    TiltAngleSpec tiltAngleSpec = new TiltAngleSpec();
    getTiltAngleFields(axisID, tiltAngleSpec, false);
    return DatasetTool.validateTiltAngle(manager, AxisID.ONLY, errorTitle, axisID,
        tiltAngleSpec.getType() == TiltAngleType.RANGE,
        String.valueOf(tiltAngleSpec.getRangeMin()),
        String.valueOf(tiltAngleSpec.getRangeStep()));
  }

  public static final class CopyArgEntrySet {
    private final Map<String, String> pairMap = new HashMap<String, String>();

    /**
     * Don't call constructor directly.
     */
    private CopyArgEntrySet() {
    }

    /**
     * This function should not return null.
     * @return initialized instance
     */
    private static CopyArgEntrySet getInstance(final DirectiveFile[] directiveFileArray) {
      CopyArgEntrySet instance = new CopyArgEntrySet();
      instance.init(directiveFileArray);
      return instance;
    }

    /**
     * Load all of the directive file copyarg values into pairMap.  Pairs with the same
     * name as a previously saved pair overrides the previous pair.  A pair with a blank
     * value is not saved and causes the pair with the same name in the map to be removed.
     * After loading all of copyarg values, load the scan header output from the directive
     * file if scan header is in the map and is set to "1".  Only load pairs with names
     * that are not already in the map, because the directive files all override scan
     * header.
     * @param directiveFileArray
     */
    private void init(final DirectiveFile[] directiveFileArray) {
      for (int i = 0; i < directiveFileArray.length; i++) {
        if (directiveFileArray[i] != null) {
          ReadOnlyAttributeIterator iterator = directiveFileArray[i].getCopyArgIterator();
          while (iterator.hasNext()) {
            ReadOnlyAttribute attribute = iterator.next();
            String name = attribute.getName();
            String value = attribute.getValue();
            pairMap.remove(name);
            // A blank value means remove the previously added pair, otherwise override
            // the previous added pair with the new value.
            if (value != null) {
              pairMap.put(name, value);
            }
          }
        }
      }
      // If scan header (only found in the batch directive file and is not in copyarg) is
      // true, then get values from scanning the header which aren't already in the map.
      if (directiveFileArray[OverrideOrder.BATCH.index] != null
          && directiveFileArray[OverrideOrder.BATCH.index].isScanHeader()) {
        Iterator<Entry<String, String>> iterator = directiveFileArray[OverrideOrder.BATCH.index]
            .getCopyArgExtraValuesIterator();
        while (iterator.hasNext()) {
          Entry<String, String> entry = iterator.next();
          String name = entry.getKey();
          if (!pairMap.containsKey(name)) {
            pairMap.put(name, entry.getValue());
          }
        }
      }
    }

    public Iterator<Entry<String, String>> iterator() {
      return pairMap.entrySet().iterator();
    }
  }

  private static final class OverrideOrder {
    private static final OverrideOrder SCOPE = new OverrideOrder(0);
    private static final OverrideOrder SYSTEM = new OverrideOrder(1);
    private static final OverrideOrder USER = new OverrideOrder(2);
    private static final OverrideOrder BATCH = new OverrideOrder(3);

    private final int index;

    private OverrideOrder(final int index) {
      this.index = index;
    }
  }
}
